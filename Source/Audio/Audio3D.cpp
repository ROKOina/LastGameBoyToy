#include "Audio3D.h"
#include "SystemStruct/Misc.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"

// Uncomment to enable the volume limiter on the master voice.
//#define MASTERING_LIMITER

using namespace DirectX;

// リバーブ効果
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS02[NUM_PRESETS] =
{
    XAUDIO2FX_I3DL2_PRESET_FOREST,
    XAUDIO2FX_I3DL2_PRESET_DEFAULT,
    XAUDIO2FX_I3DL2_PRESET_GENERIC,
    XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
    XAUDIO2FX_I3DL2_PRESET_ROOM,
    XAUDIO2FX_I3DL2_PRESET_BATHROOM,
    XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
    XAUDIO2FX_I3DL2_PRESET_STONEROOM,
    XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
    XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
    XAUDIO2FX_I3DL2_PRESET_CAVE,
    XAUDIO2FX_I3DL2_PRESET_ARENA,
    XAUDIO2FX_I3DL2_PRESET_HANGAR,
    XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
    XAUDIO2FX_I3DL2_PRESET_HALLWAY,
    XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
    XAUDIO2FX_I3DL2_PRESET_ALLEY,
    XAUDIO2FX_I3DL2_PRESET_CITY,
    XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
    XAUDIO2FX_I3DL2_PRESET_QUARRY,
    XAUDIO2FX_I3DL2_PRESET_PLAIN,
    XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
    XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
    XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
    XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
    XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
    XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
    XAUDIO2FX_I3DL2_PRESET_PLATE,
};


AudioSource3D::AudioSource3D()
{
    // Clear struct
    g_audioState = {};

    // Initialize XAudio2
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
    }


    UINT32 flags = 0;
#if defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
    flags |= XAUDIO2_DEBUG_ENGINE;
#endif
    hr = XAudio2Create(&g_audioState.pXAudio2, flags);
    if (FAILED(hr))
    {
    }

#if !defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
    // To see the trace output, you need to view ETW logs for this application:
    //    Go to Control Panel, Administrative Tools, Event Viewer.
    //    View->Show Analytic and Debug Logs.
    //    Applications and Services Logs / Microsoft / Windows / XAudio2. 
    //    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
    XAUDIO2_DEBUG_CONFIGURATION debug = {};
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    g_audioState.pXAudio2->SetDebugConfiguration(&debug, 0);
#endif

    // Create a mastering voice
    if (FAILED(hr = g_audioState.pXAudio2->CreateMasteringVoice(&g_audioState.pMasteringVoice)))
    {
        g_audioState.pXAudio2.Reset();
    }

    // Check device details to make sure it's within our sample supported parameters
    DWORD dwChannelMask = 0;
    UINT32 nSampleRate = 0;

    XAUDIO2_VOICE_DETAILS details;
    g_audioState.pMasteringVoice->GetVoiceDetails(&details);

    if (details.InputChannels > OUTPUTCHANNELS)
    {
        g_audioState.pXAudio2.Reset();
    }

    if (FAILED(hr = g_audioState.pMasteringVoice->GetChannelMask(&dwChannelMask)))
    {
        g_audioState.pXAudio2.Reset();
    }

    nSampleRate = details.InputSampleRate;
    g_audioState.nChannels = details.InputChannels;
    g_audioState.dwChannelMask = dwChannelMask;

    // Create reverb effect
    UINT32 rflags = 0;
#if defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
    rflags |= XAUDIO2FX_DEBUG;
#endif
    if (FAILED(hr = XAudio2CreateReverb(&g_audioState.pReverbEffect, rflags)))
    {
        g_audioState.pXAudio2.Reset();
    }

    // Create a submix voice

    // Performance tip: you need not run global FX with the sample number
    // of channels as the final mix.  For example, this sample runs
    // the reverb in mono mode, thus reducing CPU overhead.
    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { g_audioState.pReverbEffect.Get(), TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    if (FAILED(hr = g_audioState.pXAudio2->CreateSubmixVoice(&g_audioState.pSubmixVoice, 1,
        nSampleRate, 0, 0,
        nullptr, &effectChain)))
    {
        g_audioState.pXAudio2.Reset();
        g_audioState.pReverbEffect.Reset();
    }

    // Set default FX params
    XAUDIO2FX_REVERB_PARAMETERS native;
    ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS02[0], &native);
    g_audioState.pSubmixVoice->SetEffectParameters(0, &native, sizeof(native));

    // Initialize X3DAudio
    //  Speaker geometry configuration on the final mix, specifies assignment of channels
    //  to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask
    //
    //  SpeedOfSound - speed of sound in user-defined world units/second, used
    //  only for doppler calculations, it must be >= FLT_MIN
    constexpr float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;

    X3DAudioInitialize(dwChannelMask, SPEEDOFSOUND, g_audioState.x3DInstance);

    g_audioState.vListenerPos.x =
        g_audioState.vListenerPos.y =
        g_audioState.vListenerPos.z =
        g_audioState.vEmitterPos.x =
        g_audioState.vEmitterPos.y = 0.f;

    g_audioState.vEmitterPos.z = float(ZMAX);

    g_audioState.fListenerAngle = 0;
    g_audioState.fUseListenerCone = TRUE;
    g_audioState.fUseInnerRadius = TRUE;
    g_audioState.fUseRedirectToLFE = ((dwChannelMask & SPEAKER_LOW_FREQUENCY) != 0);

    // Setup 3D audio structs
    g_audioState.listener.Position.x = g_audioState.vListenerPos.x;
    g_audioState.listener.Position.y = g_audioState.vListenerPos.y;
    g_audioState.listener.Position.z = g_audioState.vListenerPos.z;

    g_audioState.listener.OrientFront.x =
        g_audioState.listener.OrientFront.y =
        g_audioState.listener.OrientTop.x =
        g_audioState.listener.OrientTop.z = 0.f;

    g_audioState.listener.OrientFront.z =
        g_audioState.listener.OrientTop.y = 1.f;

    g_audioState.listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

    g_audioState.emitter.pCone = &g_audioState.emitterCone;
    g_audioState.emitter.pCone->InnerAngle = 0.0f;
    // Setting the inner cone angles to X3DAUDIO_2PI and
    // outer cone other than 0 causes
    // the emitter to act like a point emitter using the
    // INNER cone settings only.
    g_audioState.emitter.pCone->OuterAngle = 0.0f;
    // Setting the outer cone angles to zero causes
    // the emitter to act like a point emitter using the
    // OUTER cone settings only.
    g_audioState.emitter.pCone->InnerVolume = 0.0f;
    g_audioState.emitter.pCone->OuterVolume = 1.0f;
    g_audioState.emitter.pCone->InnerLPF = 0.0f;
    g_audioState.emitter.pCone->OuterLPF = 1.0f;
    g_audioState.emitter.pCone->InnerReverb = 0.0f;
    g_audioState.emitter.pCone->OuterReverb = 1.0f;

    g_audioState.emitter.Position.x = g_audioState.vEmitterPos.x;
    g_audioState.emitter.Position.y = g_audioState.vEmitterPos.y;
    g_audioState.emitter.Position.z = g_audioState.vEmitterPos.z;

    g_audioState.emitter.OrientFront.x =
        g_audioState.emitter.OrientFront.y =
        g_audioState.emitter.OrientTop.x =
        g_audioState.emitter.OrientTop.z = 0.f;

    g_audioState.emitter.OrientFront.z =
        g_audioState.emitter.OrientTop.y = 1.f;

    g_audioState.emitter.ChannelCount = INPUTCHANNELS;
    g_audioState.emitter.ChannelRadius = 1.0f;

    static_assert(INPUTCHANNELS == 1 || g_audioState.emitter.pChannelAzimuths != nullptr, "Multi-channel sources require emitter azimuths");
    // For examples of how to configure emitter azimuths for multi-channel sources, see DirectX Tool Kit for Audio
    // helper method AudioEmitter::EnableDefaultMultiChannel
    // http://go.microsoft.com/fwlink/?LinkId=248929

    // Use of Inner radius allows for smoother transitions as
    // a sound travels directly through, above, or below the listener.
    // It also may be used to give elevation cues.
    g_audioState.emitter.InnerRadius = 2.0f;
    g_audioState.emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;;

    g_audioState.emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
    g_audioState.emitter.pLFECurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
    g_audioState.emitter.pLPFDirectCurve = nullptr; // use default curve
    g_audioState.emitter.pLPFReverbCurve = nullptr; // use default curve
    g_audioState.emitter.pReverbCurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
    g_audioState.emitter.CurveDistanceScaler = 14.0f;
    g_audioState.emitter.DopplerScaler = 1.0f;

    g_audioState.dspSettings.SrcChannelCount = INPUTCHANNELS;
    g_audioState.dspSettings.DstChannelCount = g_audioState.nChannels;
    g_audioState.dspSettings.pMatrixCoefficients = g_audioState.matrixCoefficients;

    // Done
    g_audioState.bInitialized = true;

    // オーディオ登録情報
    //RegisterAudio();

    //音源ロード
    //SetAudio(AUDIOIDTEST::SE);


    // オーディオ登録情報
    RegisterAudio();
}

AudioSource3D::~AudioSource3D()
{
    if (sourceVoice_)
    {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }

    resource_.reset();
    audioResources.clear();

    if (!g_audioState.bInitialized)
        return;
    if (g_audioState.pSubmixVoice)
    {
        g_audioState.pSubmixVoice->DestroyVoice();
        g_audioState.pSubmixVoice = nullptr;
    }

    if (g_audioState.pMasteringVoice)
    {
        g_audioState.pMasteringVoice->DestroyVoice();
        g_audioState.pMasteringVoice = nullptr;
    }

    g_audioState.pXAudio2->StopEngine();
    g_audioState.pXAudio2.Reset();
    g_audioState.pVolumeLimiter.Reset();
    g_audioState.pReverbEffect.Reset();


#ifdef USING_XAUDIO2_7_DIRECTX
    if (g_audioState.mXAudioDLL)
    {
        FreeLibrary(g_audioState.mXAudioDLL);
        g_audioState.mXAudioDLL = nullptr;
    }
#endif

    CoUninitialize();

    g_audioState.bInitialized = false;
}

void AudioSource3D::Start()
{
}

void AudioSource3D::Update(float elapsedTime)
{
    g_audioState.vListenerPos = GameObjectManager::Instance().Find("Lisner")->transform_->GetWorldPosition();
    g_audioState.vEmitterPos = GetGameObject()->transform_->GetWorldPosition();
    //g_audioState.vEmitterPos = GameObjectManager::Instance().Find("Emitter")->transform_->GetWorldPosition();

    //g_audioState.vListenerPos = listenerPos;
    //g_audioState.vEmitterPos = emitterPos;

    UpdateAudio3d(elapsedTime);
}

// オーディオ登録情報
void AudioSource3D::RegisterAudio()
{
    audioResources[AUDIOIDTEST::BGM] = std::make_shared<AudioResource>("Data/AudioData/TestAudio/BGM.wav");
    audioResources[AUDIOIDTEST::SE] = std::make_shared<AudioResource>("Data/AudioData/TestAudio/heli.wav");
    audioResources[AUDIOIDTEST::TEST] = std::make_shared<AudioResource>("Data/AudioData/TestAudio/SE.wav");
}

void AudioSource3D::UpdateAudio3d(float elapsedTime)
{
    {
        // Calculate listener orientation in x-z plane
        if (g_audioState.vListenerPos.x != g_audioState.listener.Position.x
            || g_audioState.vListenerPos.z != g_audioState.listener.Position.z)
        {
            const XMVECTOR v1 = XMLoadFloat3(&g_audioState.vListenerPos);
            const XMVECTOR v2 = XMVectorSet(g_audioState.listener.Position.x, g_audioState.listener.Position.y, g_audioState.listener.Position.z, 0.f);

            XMVECTOR vDelta = v1 - v2;

            g_audioState.fListenerAngle = float(atan2(XMVectorGetX(vDelta), XMVectorGetZ(vDelta)));

            vDelta = XMVectorSetY(vDelta, 0.f);
            vDelta = XMVector3Normalize(vDelta);

            XMFLOAT3 tmp;
            XMStoreFloat3(&tmp, vDelta);

            g_audioState.listener.OrientFront.x = tmp.x;
            g_audioState.listener.OrientFront.y = 0.f;
            g_audioState.listener.OrientFront.z = tmp.z;
        }

        if (g_audioState.fUseListenerCone)
        {
            g_audioState.listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;
        }
        else
        {
            g_audioState.listener.pCone = nullptr;
        }
        if (g_audioState.fUseInnerRadius)
        {
            g_audioState.emitter.InnerRadius = 2.0f;
            g_audioState.emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
        }
        else
        {
            g_audioState.emitter.InnerRadius = 0.0f;
            g_audioState.emitter.InnerRadiusAngle = 0.0f;
        }

        if (elapsedTime > 0)
        {
            XMVECTOR v1 = XMLoadFloat3(&g_audioState.vListenerPos);
            XMVECTOR v2 = XMVectorSet(g_audioState.listener.Position.x, g_audioState.listener.Position.y, g_audioState.listener.Position.z, 0);

            const XMVECTOR lVelocity = (v1 - v2) / elapsedTime;
            g_audioState.listener.Position.x = g_audioState.vListenerPos.x;
            g_audioState.listener.Position.y = g_audioState.vListenerPos.y;
            g_audioState.listener.Position.z = g_audioState.vListenerPos.z;

            XMFLOAT3 tmp;
            XMStoreFloat3(&tmp, lVelocity);
            g_audioState.listener.Velocity.x = tmp.x;
            g_audioState.listener.Velocity.y = tmp.y;
            g_audioState.listener.Velocity.z = tmp.z;

            v1 = XMLoadFloat3(&g_audioState.vEmitterPos);
            v2 = XMVectorSet(g_audioState.emitter.Position.x, g_audioState.emitter.Position.y, g_audioState.emitter.Position.z, 0.f);

            const XMVECTOR eVelocity = (v1 - v2) / elapsedTime;
            g_audioState.emitter.Position.x = g_audioState.vEmitterPos.x;
            g_audioState.emitter.Position.y = g_audioState.vEmitterPos.y;
            g_audioState.emitter.Position.z = g_audioState.vEmitterPos.z;

            XMStoreFloat3(&tmp, eVelocity);
            g_audioState.emitter.Velocity.x = tmp.x;
            g_audioState.emitter.Velocity.y = tmp.y;
            g_audioState.emitter.Velocity.z = tmp.z;
        }
    }

    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB;
    if (g_audioState.fUseRedirectToLFE)
    {
        // On devices with an LFE channel, allow the mono source data
        // to be routed to the LFE destination channel.
        dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
    }

    X3DAudioCalculate(g_audioState.x3DInstance, &g_audioState.listener, &g_audioState.emitter, dwCalcFlags,
        &g_audioState.dspSettings);

    IXAudio2SourceVoice* voice = sourceVoice_;
    if (voice)
    {
        // Apply X3DAudio generated DSP settings to XAudio2
        voice->SetFrequencyRatio(g_audioState.dspSettings.DopplerFactor);
        voice->SetOutputMatrix(g_audioState.pMasteringVoice, INPUTCHANNELS, g_audioState.nChannels,
            g_audioState.matrixCoefficients);

        voice->SetOutputMatrix(g_audioState.pSubmixVoice, 1, 1, &g_audioState.dspSettings.ReverbLevel);

        XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * g_audioState.dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        voice->SetOutputFilterParameters(g_audioState.pMasteringVoice, &FilterParametersDirect);
        XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * g_audioState.dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        voice->SetOutputFilterParameters(g_audioState.pSubmixVoice, &FilterParametersReverb);
    }
}

void AudioSource3D::SetAudio(AUDIOIDTEST id)
{
    resource_ = audioResources[id];

    if (resource_ != nullptr)
    {
        if (sourceVoice_)
        {
            sourceVoice_->Stop(0);
            sourceVoice_->DestroyVoice();
            sourceVoice_ = 0;
        }

        // Read in the wave file
        const WAVEFORMATEX* pwfx = &resource_->GetWaveFormat();
        const uint8_t* sampleData = resource_->GetAudioData();
        uint32_t waveSize = resource_->GetAudioBytes();

        // Play the wave using a source voice that sends to both the submix and mastering voices
        XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
        sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
        sendDescriptors[0].pOutputVoice = g_audioState.pMasteringVoice;
        sendDescriptors[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
        sendDescriptors[1].pOutputVoice = g_audioState.pSubmixVoice;
        const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };

        // create the source voice
        HRESULT hr = g_audioState.pXAudio2->CreateSourceVoice(&sourceVoice_, pwfx, 0, 2.0f, nullptr, &sendList);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}

void AudioSource3D::AudioPlay()
{
    if (!g_audioState.bInitialized) return;

    // Submit the wave sample data using an XAUDIO2_BUFFER structure
    XAUDIO2_BUFFER buffer = {};

    buffer.pAudioData = resource_->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    sourceVoice_->SubmitSourceBuffer(&buffer);

    sourceVoice_->Start(0);

    g_audioState.nFrameToApply3DAudio = 0;
}


AudioSource2D::AudioSource2D()
{
    HRESULT hr;
    // COMの初期化
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    //createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif
    // XAudio初期化
    hr = XAudio2Create(&xaudio, createFlags);
    // マスタリングボイス生成
    hr = xaudio->CreateMasteringVoice(&masteringVoice);

    // オーディオ登録情報
    RegisterAudio2D();
}

AudioSource2D::~AudioSource2D()
{
    resource_.reset();

    for (auto& pair : audioResources)
    {
        pair.second.reset();
    }
    audioResources.clear();

    // マスタリングボイス破棄
    if (masteringVoice != nullptr)
    {
        masteringVoice->DestroyVoice();
        masteringVoice = nullptr;
    }

    // XAudio終了化
    if (xaudio != nullptr)
    {
        xaudio->Release();
        xaudio = nullptr;
    }

    // COM終了化
    CoUninitialize();
}

void AudioSource2D::SetAudio2D(AUDIOID2D id)
{
    resource_ = audioResources[id];

    if (resource_ == nullptr) return;

    if (sourceVoice_)
    {
        sourceVoice_->Stop(0);
        sourceVoice_->DestroyVoice();
        sourceVoice_ = 0;
    }

    const WAVEFORMATEX* pwfx = &resource_->GetWaveFormat();
    const uint8_t* sampleData = resource_->GetAudioData();
    uint32_t waveSize = resource_->GetAudioBytes();

    // 2Dオーディオ
    HRESULT hr = xaudio->CreateSourceVoice(&sourceVoice_, &resource_->GetWaveFormat());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

void AudioSource2D::Audio2DPlay()
{
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    sourceVoice_->SubmitSourceBuffer(&buffer);
    sourceVoice_->Start(0);
}

void AudioSource2D::Audio2DPlay(float volume, bool loop)
{
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    sourceVoice_->SubmitSourceBuffer(&buffer);
    sourceVoice_->Start();
    sourceVoice_->SetVolume(volume * 0.1f);
}

void AudioSource2D::RegisterAudio2D()
{
    audioResources[AUDIOID2D::BGM2D] = std::make_shared<AudioResource>("Data/AudioData/TestAudio/BGM.wav");
    audioResources[AUDIOID2D::SE2D] = std::make_shared<AudioResource>("Data/AudioData/TestAudio/heli.wav");
}