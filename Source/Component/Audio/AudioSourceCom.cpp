#include "AudioSourceCom.h"

#include "SystemStruct/Misc.h"
#include "Math\easing.h"
#include "Math\Mathf.h"

// Constants to define our world space

void AudioSourceCom::Start()
{
    vListenerPos.x =
        vListenerPos.y =
        vListenerPos.z =
        vEmitterPos.x =
        vEmitterPos.y = 0.f;

    vEmitterPos.z = float(ZMAX);

    fListenerAngle = 0;
    fUseListenerCone = TRUE;
    fUseInnerRadius = TRUE;
    fUseRedirectToLFE = ((Audio::Instance().GetChannelMask() & SPEAKER_LOW_FREQUENCY) != 0);

    //
    // Setup 3D audio structs
    //
    listener.Position.x = vListenerPos.x;
    listener.Position.y = vListenerPos.y;
    listener.Position.z = vListenerPos.z;

    listener.OrientFront.x =
        listener.OrientFront.y =
        listener.OrientTop.x =
        listener.OrientTop.z = 0.f;

    listener.OrientFront.z =
        listener.OrientTop.y = 1.f;

    listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

    emitter.pCone = &emitterCone;
    emitter.pCone->InnerAngle = 0.0f;
    // Setting the inner cone angles to X3DAUDIO_2PI and
    // outer cone other than 0 causes
    // the emitter to act like a point emitter using the
    // INNER cone settings only.
    emitter.pCone->OuterAngle = 0.0f;
    // Setting the outer cone angles to zero causes
    // the emitter to act like a point emitter using the
    // OUTER cone settings only.
    emitter.pCone->InnerVolume = 0.0f;
    emitter.pCone->OuterVolume = 1.0f;
    emitter.pCone->InnerLPF = 0.0f;
    emitter.pCone->OuterLPF = 1.0f;
    emitter.pCone->InnerReverb = 0.0f;
    emitter.pCone->OuterReverb = 1.0f;

    emitter.Position.x = vEmitterPos.x;
    emitter.Position.y = vEmitterPos.y;
    emitter.Position.z = vEmitterPos.z;

    emitter.OrientFront.x =
        emitter.OrientFront.y =
        emitter.OrientTop.x =
        emitter.OrientTop.z = 0.f;

    emitter.OrientFront.z =
        emitter.OrientTop.y = 1.f;

    emitter.ChannelCount = 1;
    emitter.ChannelRadius = 1.0f;

    emitter.InnerRadius = 2.0f;
    emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;;

    emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
    emitter.pLFECurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
    emitter.pLPFDirectCurve = nullptr; // use default curve
    emitter.pLPFReverbCurve = nullptr; // use default curve
    emitter.pReverbCurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
    emitter.CurveDistanceScaler = 14.0f;
    emitter.DopplerScaler = 1.0f;

    dspSettings.SrcChannelCount = 1;
    dspSettings.DstChannelCount = Audio::Instance().GetInputChannel();
    dspSettings.pMatrixCoefficients = matrixCoefficients;

}

static int countFrame = 6;
void AudioSourceCom::Update(float elapsedTime)
{
    for (auto& [id, feedState] : feedStates_)
    {
        if (feedState.isFeeding)
        {
            // フィード処理を実行
            if (Feed(id))
            {
                feedState.isFeeding = false; // フィードが終了したらフラグを下げる
            }
        }
    }

    countFrame--;
    if (countFrame < 0)
    {
        countFrame = 6;
        Update3DAudio();
    }
}

using namespace DirectX;

// 3Dオーディオの更新
void AudioSourceCom::Update3DAudio()
{
    // Calculate listener orientation in x-z plane
    if (vListenerPos.x != listener.Position.x
        || vListenerPos.z != listener.Position.z)
    {
        const XMVECTOR v1 = XMLoadFloat3(&vListenerPos);
        const XMVECTOR v2 = XMVectorSet(listener.Position.x, listener.Position.y, listener.Position.z, 0.f);

        XMVECTOR vDelta = v1 - v2;

        fListenerAngle = float(atan2(XMVectorGetX(vDelta), XMVectorGetZ(vDelta)));

        vDelta = XMVectorSetY(vDelta, 0.f);
        vDelta = XMVector3Normalize(vDelta);

        XMFLOAT3 tmp;
        XMStoreFloat3(&tmp, vDelta);

        listener.OrientFront.x = tmp.x;
        listener.OrientFront.y = 0.f;
        listener.OrientFront.z = tmp.z;
    }

    if (fUseListenerCone)
    {
        listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;
    }
    else
    {
        listener.pCone = nullptr;
    }
    if (fUseInnerRadius)
    {
        emitter.InnerRadius = 2.0f;
        emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
    }
    else
    {
        emitter.InnerRadius = 0.0f;
        emitter.InnerRadiusAngle = 0.0f;
    }

    XMVECTOR v1 = XMLoadFloat3(&vListenerPos);
    XMVECTOR v2 = XMVectorSet(listener.Position.x, listener.Position.y, listener.Position.z, 0);

    const XMVECTOR lVelocity = (v1 - v2) / 0.01666f;
    listener.Position.x = vListenerPos.x;
    listener.Position.y = vListenerPos.y;
    listener.Position.z = vListenerPos.z;

    XMFLOAT3 tmp;
    XMStoreFloat3(&tmp, lVelocity);
    listener.Velocity.x = tmp.x;
    listener.Velocity.y = tmp.y;
    listener.Velocity.z = tmp.z;

    v1 = XMLoadFloat3(&vEmitterPos);
    v2 = XMVectorSet(emitter.Position.x, emitter.Position.y, emitter.Position.z, 0.f);

    const XMVECTOR eVelocity = (v1 - v2) / 0.01666f;
    emitter.Position.x = vEmitterPos.x;
    emitter.Position.y = vEmitterPos.y;
    emitter.Position.z = vEmitterPos.z;

    XMStoreFloat3(&tmp, eVelocity);
    emitter.Velocity.x = tmp.x;
    emitter.Velocity.y = tmp.y;
    emitter.Velocity.z = tmp.z;


    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB;
    if (fUseRedirectToLFE)
    {
        // On devices with an LFE channel, allow the mono source data
        // to be routed to the LFE destination channel.
        dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
    }

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    X3DAudioCalculate(*x3dHandle,
        &listener,
        &emitter,
        dwCalcFlags,
        &dspSettings);

    // 計算結果を反映
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->SetFrequencyRatio(dspSettings.DopplerFactor);

        voice->SetOutputMatrix(Audio::Instance().GetMasterVoice(), 1, Audio::Instance().GetInputChannel(),
            dspSettings.pMatrixCoefficients);

        voice->SetOutputMatrix(Audio::Instance().GetSubmixVoice(), 1, 1, &dspSettings.ReverbLevel);

        XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        voice->SetOutputFilterParameters(Audio::Instance().GetMasterVoice(), &FilterParametersDirect);
        XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        voice->SetOutputFilterParameters(Audio::Instance().GetSubmixVoice(), &FilterParametersReverb);

        // 距離に応じた音量減衰
        float distance = dspSettings.EmitterToListenerDistance;
        float volume = 1.0f / (1.0f + distance);
        voice->SetVolume(volume);
    }

    //// 計算結果を反映
    //for (auto& [id, voice] : sourceVoices_)
    //{
    //    voice->SetFrequencyRatio(dspSettings.DopplerFactor);
    //    voice->SetOutputMatrix(nullptr, dspSettings.SrcChannelCount, nC, dspSettings.pMatrixCoefficients);
    //    //voice->SetOutputMatrix(Audio::Instance().GetMasterVoice(), dspSettings.SrcChannelCount, nC, dspSettings.pMatrixCoefficients);
    //    voice->SetVolume(1);
    //    //voice->SetVolume(volumeControls_[id] * dspSettings.EmitterToListenerDistance);


    //    voice->SetOutputMatrix(Audio::Instance().GetMasterVoice(), 1, nC,
    //        dspSettings.pMatrixCoefficients);

    //    voice->SetOutputMatrix(Audio::Instance().GetSubmixVoice(), 1, 1, &dspSettings.ReverbLevel);

    //    XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
    //    voice->SetOutputFilterParameters(Audio::Instance().GetMasterVoice(), &FilterParametersDirect);
    //    XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f *dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
    //    voice->SetOutputFilterParameters(Audio::Instance().GetSubmixVoice(), &FilterParametersReverb);

    //}
}

void AudioSourceCom::OnGUI()
{
    for (const auto& [id, name] : audioNames_)
    {
        ImGui::Text("Audio Name: %s", name.c_str());

        // ループ設定
        bool loop = loopFlags_[id];
        std::string loopLabel = "Loop##" + std::to_string(id);
        if (ImGui::Checkbox(loopLabel.c_str(), &loop))
        {
            SetLoop(id, loop);
        }
        ImGui::SameLine();

        // 再生ボタン
        std::string playLabel = "Play##" + std::to_string(id);
        if (ImGui::Button(playLabel.c_str()))
        {
            AudioPlay(id, loopFlags_[id], volumeControls_[id]);
        }
        ImGui::SameLine();

        // 停止ボタン
        std::string stopLabel = "Stop##" + std::to_string(id);
        if (ImGui::Button(stopLabel.c_str()))
        {
            Stop(id);
        }
        ImGui::SameLine();

        // 音量調整
        float volume = volumeControls_[id];
        std::string volumeLabel = "Volume##" + std::to_string(id);
        if (ImGui::SliderFloat(volumeLabel.c_str(), &volume, 0.0f, 1.0f))
        {
            SetVolume(id, volume);
        }

        ImGui::Separator();

    }
    //ImGui::DragFloat("emitterF", &emitter_.f);
    //ImGui::DragFloat3("emitterPos", &emitter_.position.x);
    //ImGui::DragFloat3("emitterVe", &emitter_.velocity.x);
    //ImGui::DragFloat("emitterDis", &emitter_.x3dEmitter.CurveDistanceScaler);
    //ImGui::DragFloat("emitterDop", &emitter_.x3dEmitter.DopplerScaler);

    //ImGui::DragFloat3("lisnerPos", &listener_.position.x);
    //ImGui::DragFloat3("lisnerVe", &listener_.velocity.x);

    ImGui::DragFloat3("vListenerPos", &vListenerPos.x,0.1f);
    ImGui::DragFloat3("vEmitterPos", &vEmitterPos.x, 0.01f);
    ImGui::DragFloat("fListenerAngle", &fListenerAngle);
    ImGui::Checkbox("fUseListenerCone", &fUseListenerCone);
    ImGui::Checkbox("fUseInnerRadius", &fUseInnerRadius);
    ImGui::Checkbox("fUseRedirectToLFE", &fUseRedirectToLFE);

}

// オーディオを追加
void AudioSourceCom::AddAudio(int id)
{
    auto resource = Audio::Instance().GetAudioResourceID(static_cast<AUDIOID>(id));
    if (resource)
    {
        XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
        sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
        sendDescriptors[0].pOutputVoice = Audio::Instance().GetMasterVoice();
        sendDescriptors[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
        sendDescriptors[1].pOutputVoice = Audio::Instance().GetSubmixVoice();
        const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };

        IXAudio2SourceVoice* voice = nullptr;
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&voice, &resource->GetWaveFormat(), 0,
            2.0f, nullptr, &sendList);
        if (SUCCEEDED(hr))
        {
            resources_[id] = resource;
            sourceVoices_[id] = voice;
            audioNames_[id] = Audio::Instance().GetAudioName(static_cast<AUDIOID>(id));
            loopFlags_[id] = false;
            volumeControls_[id] = 1.0f;
        }
    }
}

// 再生
void AudioSourceCom::AudioPlay(int id)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end() && resources_[id])
    {
        Stop(id);

        IXAudio2SourceVoice* sourceVoice = it->second;
        auto resource = resources_[id];

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = resource->GetAudioBytes();
        buffer.pAudioData = resource->GetAudioData();
        buffer.LoopCount = loopFlags_[id] ? XAUDIO2_LOOP_INFINITE : 0;
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        sourceVoice->SubmitSourceBuffer(&buffer);
        sourceVoice->SetVolume(volumeControls_[id]);
        sourceVoice->Start();
    }
}
void AudioSourceCom::AudioPlay(int id, bool loop, float volume)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end() && resources_[id])
    {
        Stop(id);

        IXAudio2SourceVoice* sourceVoice = it->second;
        auto resource = resources_[id];

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = resource->GetAudioBytes();
        buffer.pAudioData = resource->GetAudioData();
        buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        sourceVoice->SubmitSourceBuffer(&buffer);
        sourceVoice->SetVolume(volume);
        sourceVoice->Start();

        loopFlags_[id] = loop;
        volumeControls_[id] = volume;
    }
}

// 停止
void AudioSourceCom::Stop(int id)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end())
    {
        it->second->Stop();
        it->second->FlushSourceBuffers();
    }
}
void AudioSourceCom::StopAll()
{
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->Stop();
        voice->FlushSourceBuffers();
    }
}

// エミッター再生
void AudioSourceCom::EmitterPlay(int id)
{
    // 音源が存在するか確認
    if (sourceVoices_.find(id) == sourceVoices_.end() || !resources_[id])
    {
        assert("指定されたIDのソースボイスが存在しません");
        return;
    }

    // 対応するソースボイスを取得
    IXAudio2SourceVoice* sourceVoice = sourceVoices_[id];

    // XAUDIO2_BUFFERの設定
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resources_[id]->GetAudioBytes();
    buffer.pAudioData = resources_[id]->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;        
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;


    // ソースボイスのバッファをフラッシュして再送信
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();
    sourceVoice->SubmitSourceBuffer(&buffer);

    //// 3Dオーディオの更新
    //Update3DAudio();

    // 再生開始
    HRESULT hr = sourceVoice->Start(0);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
void AudioSourceCom::EmitterPlay(int id, bool loop, float volume)
{
    // 音源が存在するか確認
    if (sourceVoices_.find(id) == sourceVoices_.end() || !resources_[id])
    {
        assert("指定されたIDのソースボイスが存在しません");
        return;
    }

    // 対応するソースボイスを取得
    IXAudio2SourceVoice* sourceVoice = sourceVoices_[id];

    // XAUDIO2_BUFFERの設定
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resources_[id]->GetAudioBytes();
    buffer.pAudioData = resources_[id]->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    // ループ設定
    if (loop)
    {
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    // ボリューム設定
    volumeControls_[id] = volume;

    // ソースボイスのバッファをフラッシュして再送信
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();
    sourceVoice->SubmitSourceBuffer(&buffer);

    // 3Dオーディオの更新
    Update3DAudio();

    // 再生開始
    HRESULT hr = sourceVoice->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 音量設定
void AudioSourceCom::SetVolume(int id, float volume)
{
    if (sourceVoices_.find(id) != sourceVoices_.end())
    {
        sourceVoices_[id]->SetVolume(volume);
        volumeControls_[id] = volume;
    }
}
// ループ設定
void AudioSourceCom::SetLoop(int id, bool loop)
{
    loopFlags_[id] = loop;
}

void AudioSourceCom::FeedStart(int id, float targetValue, float add)
{
    if (feedStates_.find(id) != feedStates_.end())
    {
        auto& feedState = feedStates_[id];
        feedState.targetVolume = targetValue;
        feedState.feedStep = add;
        feedState.isFeeding = true;
    }
}
// 指定された値に音量を近づけていく
bool AudioSourceCom::Feed(int id)
{
    if (feedStates_.find(id) != feedStates_.end())
    {
        auto& feedState = feedStates_[id];
        float& currentVolume = feedState.currentVolume;
        float targetVolume = feedState.targetVolume;

        // 目標音量と現在音量が等しい場合、終了
        if (currentVolume == targetVolume)
        {
            return true;
        }

        // 音量を目標値に向かって調整
        if (targetVolume > currentVolume)
        {
            currentVolume = min(currentVolume + feedState.feedStep, targetVolume);
        }
        else
        {
            currentVolume = max(currentVolume - feedState.feedStep, targetVolume);
        }

        // 音量を実際のソースに反映
        SetVolume(id, currentVolume);

        // 調整が完了した場合
        return currentVolume == targetVolume;
    }
    return true; // 指定IDが存在しない場合、即終了とする
}

void AudioSourceCom::AudioRelease()
{
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->DestroyVoice();
    }
    sourceVoices_.clear();
    resources_.clear();
    audioNames_.clear();
    loopFlags_.clear();
    volumeControls_.clear();
    feedStates_.clear();
}