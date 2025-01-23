#include "SystemStruct/Misc.h"
#include "Audio/Audio.h"

#pragma comment(lib, "xaudio2.lib")

//// Must match order of g_PRESET_NAMES
//XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[30] =
//{
//    XAUDIO2FX_I3DL2_PRESET_FOREST,
//    XAUDIO2FX_I3DL2_PRESET_DEFAULT,
//    XAUDIO2FX_I3DL2_PRESET_GENERIC,
//    XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
//    XAUDIO2FX_I3DL2_PRESET_ROOM,
//    XAUDIO2FX_I3DL2_PRESET_BATHROOM,
//    XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
//    XAUDIO2FX_I3DL2_PRESET_STONEROOM,
//    XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
//    XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
//    XAUDIO2FX_I3DL2_PRESET_CAVE,
//    XAUDIO2FX_I3DL2_PRESET_ARENA,
//    XAUDIO2FX_I3DL2_PRESET_HANGAR,
//    XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
//    XAUDIO2FX_I3DL2_PRESET_HALLWAY,
//    XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
//    XAUDIO2FX_I3DL2_PRESET_ALLEY,
//    XAUDIO2FX_I3DL2_PRESET_CITY,
//    XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
//    XAUDIO2FX_I3DL2_PRESET_QUARRY,
//    XAUDIO2FX_I3DL2_PRESET_PLAIN,
//    XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
//    XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
//    XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
//    XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
//    XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
//    XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
//    XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
//    XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
//    XAUDIO2FX_I3DL2_PRESET_PLATE,
//};
//
// コンストラクタ
Audio::Audio()
{
    HRESULT hr;

    // COMの初期化
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

    // XAudio2 の初期化
    hr = XAudio2Create(&xaudio_, createFlags);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to initialize XAudio2.");
    }

    // マスタリングボイス生成
    hr = xaudio_->CreateMasteringVoice(&masteringVoice_);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create mastering voice.");
    }

    // スピーカーチャネルマスクの取得
    speakerChannelMask = SPEAKER_STEREO;
    hr = masteringVoice_->GetChannelMask(&speakerChannelMask);
    if (FAILED(hr) || speakerChannelMask == 0)
    {
        speakerChannelMask = SPEAKER_STEREO;  // 取得失敗時にデフォルトを再設定
        throw std::runtime_error("Failed to retrieve speaker channel mask or invalid channel mask.");
    }

    //サブミックス
    DWORD dwChannelMask = 0;
    UINT32 nSampleRate = 0;

    XAUDIO2_VOICE_DETAILS details;
    masteringVoice_->GetVoiceDetails(&details);

    nSampleRate = details.InputSampleRate;
    InputChannels = details.InputChannels;



    FXMASTERINGLIMITER_PARAMETERS params = {};
    params.Release = FXMASTERINGLIMITER_DEFAULT_RELEASE;
    params.Loudness = FXMASTERINGLIMITER_DEFAULT_LOUDNESS;

    hr = CreateFX(__uuidof(FXMasteringLimiter), &pVolumeLimiter, &params, sizeof(params));

    XAUDIO2_EFFECT_DESCRIPTOR desc = {};
    desc.InitialState = TRUE;
    desc.OutputChannels = InputChannels;
    desc.pEffect = pVolumeLimiter.Get();

    XAUDIO2_EFFECT_CHAIN chain = { 1, &desc };
    hr = masteringVoice_->SetEffectChain(&chain);



    UINT32 rflags = 0;
    hr = XAudio2CreateReverb(&pReverbEffect, rflags);

    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { pReverbEffect.Get(), TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    hr = xaudio_->CreateSubmixVoice(&submixVoice_, 1,
        nSampleRate, 0, 0,
        nullptr, &effectChain);

        // Set default FX params
    XAUDIO2FX_REVERB_PARAMETERS native;
    //ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS[0], &native);
    submixVoice_->SetEffectParameters(0, &native, sizeof(native));



    // X3DAudio 初期化
    hr = X3DAudioInitialize(speakerChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle_);
    _ASSERT_EXPR(SUCCEEDED(hr), "Failed to initialize X3DAudio.");

    // BGMとSEを一括登録
    RegisterAudioSources();

    RegisterAudioSourcesTest();
}

// デストラクタ
Audio::~Audio()
{
    // マスタリングボイス破棄
    if (masteringVoice_ != nullptr)
    {
        masteringVoice_->DestroyVoice();
        masteringVoice_ = nullptr;
    }

    // サブミックス破棄
    if (submixVoice_ != nullptr)
    {
        submixVoice_->DestroyVoice();
        submixVoice_ = nullptr;
    }

    // XAudio終了化
    if (xaudio_ != nullptr)
    {
        xaudio_->Release();
        xaudio_ = nullptr;
    }

    pReverbEffect.Reset();

    // COM終了化
    CoUninitialize();
}

// オーディオソース読み込み
std::shared_ptr<AudioResource> Audio::LoadAudioSource(const char* filename)
{
    return std::make_shared<AudioResource>(filename);
}

//BGMとSEを一括登録
void Audio::RegisterAudioSources()
{
    audioResources[AUDIOID::BGM] = LoadAudioSource("Data/AudioData/TestAudio/BGM.wav");
    audioResources[AUDIOID::SE] = LoadAudioSource("Data/AudioData/TestAudio/SE.wav");
    audioResources[AUDIOID::SCENE_GAME1] = LoadAudioSource("Data/AudioData/BGM/BossBattle_start.wav");
    audioResources[AUDIOID::SCENE_GAME2] = LoadAudioSource("Data/AudioData/BGM/BossBattle_clymax.wav");
    audioResources[AUDIOID::SCENE_TITLE] = LoadAudioSource("Data/AudioData/BGM/Indomitable.wav");
    audioResources[AUDIOID::CURSOR] = LoadAudioSource("Data/AudioData/SE/cursorMove.wav");
    audioResources[AUDIOID::ENTER] = LoadAudioSource("Data/AudioData/SE/enter.wav");
    audioResources[AUDIOID::BOSS_JUMPATTACK_START] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_start.wav");
    audioResources[AUDIOID::BOSS_JUMPATTACK_END] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_end.wav");
    audioResources[AUDIOID::BOSS_JUMPATTACK_GROUND] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_ground3.wav");
    audioResources[AUDIOID::BOSS_SHOT] = LoadAudioSource("Data/AudioData/SE/boss_shot.wav");
    audioResources[AUDIOID::BOSS_POWERSHOT] = LoadAudioSource("Data/AudioData/SE/boss_powerShot.wav");
    audioResources[AUDIOID::BOSS_CHARGE] = LoadAudioSource("Data/AudioData/SE/boss_charge.wav");
    audioResources[AUDIOID::BOSS_BULLET] = LoadAudioSource("Data/AudioData/SE/boss_fire.wav");
    audioResources[AUDIOID::BOSS_PUNCH] = LoadAudioSource("Data/AudioData/SE/boss_punch.wav");
    audioResources[AUDIOID::BOSS_LARIAT] = LoadAudioSource("Data/AudioData/SE/boss_lariat.wav");
    audioResources[AUDIOID::BOSS_WALK] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_ground2.wav");

    //プレイヤー
    audioResources[AUDIOID::PLAYER_ATTACKULTBOOM] = LoadAudioSource("Data/AudioData/SE/player/player_Boom.wav");
    audioResources[AUDIOID::PLAYER_ATTACKULTSHOOT] = LoadAudioSource("Data/AudioData/SE/player/player_attackUltShoot.wav");
    audioResources[AUDIOID::PLAYER_CHARGE] = LoadAudioSource("Data/AudioData/SE/player/player_charge.wav");
    audioResources[AUDIOID::PLAYER_DAMAGE] = LoadAudioSource("Data/AudioData/SE/player/player_damage.wav");
    audioResources[AUDIOID::PLAYER_DASH] = LoadAudioSource("Data/AudioData/SE/player/player_dash2.wav");
    audioResources[AUDIOID::PLAYER_SHOOT] = LoadAudioSource("Data/AudioData/SE/player/player_shoot.wav");

    //チュートリアル
    audioResources[AUDIOID::TUTOLINES_01] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/001_L.wav");
    audioResources[AUDIOID::TUTOLINES_02] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/002_L.wav");
    audioResources[AUDIOID::TUTOLINES_03] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/003_L.wav");
    audioResources[AUDIOID::TUTOLINES_04] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/004_L.wav");
    audioResources[AUDIOID::TUTOLINES_05] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/005_L.wav");
    audioResources[AUDIOID::TUTOLINES_06] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/006_L.wav");
    audioResources[AUDIOID::TUTOLINES_07] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/007_L.wav");
    audioResources[AUDIOID::TUTOLINES_08] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/008_L.wav");
    audioResources[AUDIOID::TUTOLINES_09] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/009_L.wav");
    audioResources[AUDIOID::TUTOLINES_10] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/010_L.wav");
    audioResources[AUDIOID::TUTOLINES_11] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/011_L.wav");
    audioResources[AUDIOID::TUTOLINES_12] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/012_L.wav");
    audioResources[AUDIOID::TUTOLINES_13] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/013_L.wav");
    audioResources[AUDIOID::TUTOLINES_14] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/014_L.wav");
    audioResources[AUDIOID::TUTOLINES_15] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/015_L.wav");
    audioResources[AUDIOID::TUTOLINES_16] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/016_L.wav");
    audioResources[AUDIOID::TUTOLINES_17] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/017_L.wav");
    audioResources[AUDIOID::TUTOLINES_18] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/018_L.wav");
    audioResources[AUDIOID::TUTOLINES_19] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/019_L.wav");
    audioResources[AUDIOID::TUTOLINES_20] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/020_L.wav");
    audioResources[AUDIOID::TUTOLINES_21] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/021_L.wav");
    audioResources[AUDIOID::TUTOLINES_22] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/022_L.wav");
    audioResources[AUDIOID::TUTOLINES_23] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/023_L.wav");
    audioResources[AUDIOID::TUTOLINES_24] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/024_L.wav");
    audioResources[AUDIOID::TUTOLINES_25] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/025_L.wav");
}

void Audio::RegisterAudioSourcesTest()
{
    audioResourcesTest[AUDIOID::BGM] = LoadAudioSource("Data/AudioData/TestAudio/heli.wav");
    RegisterAudioName(AUDIOID::BGM, "BGM");

    audioResourcesTest[AUDIOID::SE] = LoadAudioSource("Data/AudioData/TestAudio/SE.wav");
    RegisterAudioName(AUDIOID::SE, "SE");

    // 共通
    {
        audioResources[AUDIOID::SCENE_GAME1] = LoadAudioSource("Data/AudioData/BGM/BossBattle_start.wav");
        RegisterAudioName(AUDIOID::SCENE_GAME1, "BGM1");

        audioResources[AUDIOID::SCENE_GAME2] = LoadAudioSource("Data/AudioData/BGM/BossBattle_clymax.wav");
        RegisterAudioName(AUDIOID::SCENE_GAME2, "BGM2");

        audioResources[AUDIOID::SCENE_TITLE] = LoadAudioSource("Data/AudioData/BGM/Indomitable.wav");
        RegisterAudioName(AUDIOID::SCENE_TITLE, "Title");

        audioResources[AUDIOID::CURSOR] = LoadAudioSource("Data/AudioData/SE/cursorMove.wav");
        RegisterAudioName(AUDIOID::CURSOR, "Cursor");

        audioResources[AUDIOID::ENTER] = LoadAudioSource("Data/AudioData/SE/enter.wav");
        RegisterAudioName(AUDIOID::ENTER, "Enter");
    }

    // 敵SE
    {
        audioResources[AUDIOID::BOSS_JUMPATTACK_START] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_start.wav");
        RegisterAudioName(AUDIOID::BOSS_JUMPATTACK_START, "JUMP_ATTACK_START");

        audioResources[AUDIOID::BOSS_JUMPATTACK_END] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_end.wav");
        RegisterAudioName(AUDIOID::BOSS_JUMPATTACK_END, "JUMP_ATTACK_END");

        audioResources[AUDIOID::BOSS_JUMPATTACK_GROUND] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_ground3.wav");
        RegisterAudioName(AUDIOID::BOSS_JUMPATTACK_GROUND, "JUMP_ATTACK_GROUND");

        audioResources[AUDIOID::BOSS_SHOT] = LoadAudioSource("Data/AudioData/SE/boss_shot.wav");
        RegisterAudioName(AUDIOID::BOSS_SHOT, "SHOT");

        audioResources[AUDIOID::BOSS_POWERSHOT] = LoadAudioSource("Data/AudioData/SE/boss_powerShot.wav");
        RegisterAudioName(AUDIOID::BOSS_POWERSHOT, "POWERSHOT");

        audioResources[AUDIOID::BOSS_CHARGE] = LoadAudioSource("Data/AudioData/SE/boss_charge.wav");
        RegisterAudioName(AUDIOID::BOSS_CHARGE, "CHARGE");

        audioResources[AUDIOID::BOSS_BULLET] = LoadAudioSource("Data/AudioData/SE/boss_fire.wav");
        RegisterAudioName(AUDIOID::BOSS_BULLET, "FIRE");

        audioResources[AUDIOID::BOSS_PUNCH] = LoadAudioSource("Data/AudioData/SE/boss_punch.wav");
        RegisterAudioName(AUDIOID::BOSS_PUNCH, "PUNCH");

        audioResources[AUDIOID::BOSS_LARIAT] = LoadAudioSource("Data/AudioData/SE/boss_lariat.wav");
        RegisterAudioName(AUDIOID::BOSS_LARIAT, "LARIAT");

        audioResources[AUDIOID::BOSS_WALK] = LoadAudioSource("Data/AudioData/SE/boss_jumpAttack_ground2.wav");
        RegisterAudioName(AUDIOID::BOSS_WALK, "WALK");
    }

    //プレイヤー
    {
        audioResources[AUDIOID::PLAYER_ATTACKULTBOOM] = LoadAudioSource("Data/AudioData/SE/player/player_Boom.wav");
        RegisterAudioName(AUDIOID::PLAYER_ATTACKULTBOOM, "P_ATTACK_ULT_BOOM");

        audioResources[AUDIOID::PLAYER_ATTACKULTSHOOT] = LoadAudioSource("Data/AudioData/SE/player/player_attackUltShoot.wav");
        RegisterAudioName(AUDIOID::PLAYER_ATTACKULTSHOOT, "P_ATTACKULTSHOOT");

        audioResources[AUDIOID::PLAYER_CHARGE] = LoadAudioSource("Data/AudioData/SE/player/player_charge.wav");
        RegisterAudioName(AUDIOID::PLAYER_CHARGE, "P_CHARGE");

        audioResources[AUDIOID::PLAYER_DAMAGE] = LoadAudioSource("Data/AudioData/SE/player/player_damage.wav");
        RegisterAudioName(AUDIOID::PLAYER_DAMAGE, "P_DAMAGE");

        audioResources[AUDIOID::PLAYER_DASH] = LoadAudioSource("Data/AudioData/SE/player/player_dash2.wav");
        RegisterAudioName(AUDIOID::PLAYER_DASH, "P_DASH");

        audioResources[AUDIOID::PLAYER_SHOOT] = LoadAudioSource("Data/AudioData/SE/player/player_shoot.wav");
        RegisterAudioName(AUDIOID::PLAYER_SHOOT, "P_SHOOT");
    }

    //チュートリアル
    {
        audioResources[AUDIOID::TUTOLINES_01] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/001_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_01, "01");

        audioResources[AUDIOID::TUTOLINES_02] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/002_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_02, "02");

        audioResources[AUDIOID::TUTOLINES_03] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/003_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_03, "03");

        audioResources[AUDIOID::TUTOLINES_04] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/004_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_04, "04");

        audioResources[AUDIOID::TUTOLINES_05] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/005_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_05, "05");

        audioResources[AUDIOID::TUTOLINES_06] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/006_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_06, "06");

        audioResources[AUDIOID::TUTOLINES_07] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/007_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_07, "07");

        audioResources[AUDIOID::TUTOLINES_08] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/008_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_08, "08");

        audioResources[AUDIOID::TUTOLINES_09] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/009_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_09, "09");

        audioResources[AUDIOID::TUTOLINES_10] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/010_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_10, "10");

        audioResources[AUDIOID::TUTOLINES_11] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/011_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_11, "11");

        audioResources[AUDIOID::TUTOLINES_12] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/012_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_12, "12");

        audioResources[AUDIOID::TUTOLINES_13] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/013_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_13, "13");

        audioResources[AUDIOID::TUTOLINES_14] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/014_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_14, "14");

        audioResources[AUDIOID::TUTOLINES_15] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/015_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_15, "15");

        audioResources[AUDIOID::TUTOLINES_16] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/016_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_16, "16");

        audioResources[AUDIOID::TUTOLINES_17] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/017_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_17, "17");

        audioResources[AUDIOID::TUTOLINES_18] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/018_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_18, "18");

        audioResources[AUDIOID::TUTOLINES_19] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/019_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_19, "19");

        audioResources[AUDIOID::TUTOLINES_20] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/020_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_20, "20");

        audioResources[AUDIOID::TUTOLINES_21] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/021_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_21, "21");

        audioResources[AUDIOID::TUTOLINES_22] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/022_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_22, "22");

        audioResources[AUDIOID::TUTOLINES_23] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/023_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_23, "23");

        audioResources[AUDIOID::TUTOLINES_24] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/024_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_24, "24");

        audioResources[AUDIOID::TUTOLINES_25] = LoadAudioSource("Data/AudioData/SE/Tutorial/TutorialLines/025_L.wav");
        RegisterAudioName(AUDIOID::TUTOLINES_25, "25");
    }
}

//using namespace DirectX;
//static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };
//
//static int frame3 = 3;
void Audio::Test3DUpdate()
{
    //// Calculate listener orientation in x-z plane
    //if (vListenerPos.x != listener.Position.x
    //    || vListenerPos.z != listener.Position.z)
    //{
    //    const XMVECTOR v1 = XMLoadFloat3(&vListenerPos);
    //    const XMVECTOR v2 = XMVectorSet(listener.Position.x, listener.Position.y, listener.Position.z, 0.f);

    //    XMVECTOR vDelta = v1 - v2;

    //    fListenerAngle = float(atan2(XMVectorGetX(vDelta), XMVectorGetZ(vDelta)));

    //    vDelta = XMVectorSetY(vDelta, 0.f);
    //    vDelta = XMVector3Normalize(vDelta);

    //    XMFLOAT3 tmp;
    //    XMStoreFloat3(&tmp, vDelta);

    //    listener.OrientFront.x = tmp.x;
    //    listener.OrientFront.y = 0.f;
    //    listener.OrientFront.z = tmp.z;
    //}

    //if (fUseListenerCone)
    //{
    //    listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;
    //}
    //else
    //{
    //    listener.pCone = nullptr;
    //}
    //if (fUseInnerRadius)
    //{
    //    emitter.InnerRadius = 2.0f;
    //    emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
    //}
    //else
    //{
    //    emitter.InnerRadius = 0.0f;
    //    emitter.InnerRadiusAngle = 0.0f;
    //}

    //XMVECTOR v1 = XMLoadFloat3(&vListenerPos);
    //XMVECTOR v2 = XMVectorSet(listener.Position.x, listener.Position.y, listener.Position.z, 0);

    //const XMVECTOR lVelocity = (v1 - v2) / 0.01666f;
    //listener.Position.x = vListenerPos.x;
    //listener.Position.y = vListenerPos.y;
    //listener.Position.z = vListenerPos.z;

    //XMFLOAT3 tmp;
    //XMStoreFloat3(&tmp, lVelocity);
    //listener.Velocity.x = tmp.x;
    //listener.Velocity.y = tmp.y;
    //listener.Velocity.z = tmp.z;

    //v1 = XMLoadFloat3(&vEmitterPos);
    //v2 = XMVectorSet(emitter.Position.x, emitter.Position.y, emitter.Position.z, 0.f);

    //const XMVECTOR eVelocity = (v1 - v2) / 0.01666f;
    //emitter.Position.x = vEmitterPos.x;
    //emitter.Position.y = vEmitterPos.y;
    //emitter.Position.z = vEmitterPos.z;

    //XMStoreFloat3(&tmp, eVelocity);
    //emitter.Velocity.x = tmp.x;
    //emitter.Velocity.y = tmp.y;
    //emitter.Velocity.z = tmp.z;


    //DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
    //    | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
    //    | X3DAUDIO_CALCULATE_REVERB;
    //if (fUseRedirectToLFE)
    //{
    //    // On devices with an LFE channel, allow the mono source data
    //    // to be routed to the LFE destination channel.
    //    dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
    //}

    //const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    //X3DAudioCalculate(*x3dHandle,
    //    &listener,
    //    &emitter,
    //    X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER,
    //    &dspSettings);

    //// 計算結果を反映
    //IXAudio2SourceVoice* voice = g_audioState.pSourceVoice;

    //voice->SetOutputMatrix(Audio::Instance().GetMasterVoice(), 1, nC,
    //    dspSettings.pMatrixCoefficients);

    //voice->SetOutputMatrix(Audio::Instance().GetSubmixVoice(), 1, 1, &dspSettings.ReverbLevel);

    //XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
    //voice->SetOutputFilterParameters(Audio::Instance().GetMasterVoice(), &FilterParametersDirect);
    //XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
    //voice->SetOutputFilterParameters(Audio::Instance().GetSubmixVoice(), &FilterParametersReverb);

}

void Audio::RegisterAudioName(AUDIOID id, const std::string& name)
{
    audioNames[id] = name;
}

std::string Audio::GetAudioName(AUDIOID id) const
{
    auto it = audioNames.find(id);
    if (it != audioNames.end()) {
        return it->second;
    }
    return "";
}


//登録されたオーディオソースを取得
std::shared_ptr<AudioResource> Audio::GetAudioResource(AUDIOID id)
{
    return audioResources.at(id);
}

std::shared_ptr<AudioResource> Audio::GetAudioResourceID(AUDIOID id)
{
    auto it = audioResourcesTest.find(id);
    if (it != audioResourcesTest.end()) {
        return it->second;
    }
    return nullptr;
}