#include "SystemStruct/Misc.h"
#include "Audio/Audio.h"

#pragma comment(lib, "xaudio2.lib")

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
    DWORD speakerChannelMask = SPEAKER_STEREO;
    hr = masteringVoice_->GetChannelMask(&speakerChannelMask);
    if (FAILED(hr) || speakerChannelMask == 0)
    {
        throw std::runtime_error("Failed to retrieve speaker channel mask or invalid channel mask.");
    }

    // X3DAudio 初期化
    hr = X3DAudioInitialize(speakerChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle_);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to initialize X3DAudio.");
    }

    // BGMとSEを一括登録
    RegisterAudioSources();
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

    // XAudio終了化
    if (xaudio_ != nullptr)
    {
        xaudio_->Release();
        xaudio_ = nullptr;
    }

    // COM終了化
    CoUninitialize();
}

// BGMとSEを一括登録
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
}

void Audio::RegisterAudioSources(AUDIOID id, const char* filename)
{
}

// オーディオソース読み込み
std::shared_ptr<AudioResource> Audio::LoadAudioSource(const char* filename)
{
    return std::make_shared<AudioResource>(filename);
}

// 登録されたオーディオソースを取得
std::shared_ptr<AudioResource> Audio::GetAudioResource(AUDIOID id)
{
    return audioResources.at(id);
}