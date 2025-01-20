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
        speakerChannelMask = SPEAKER_STEREO;  // 取得失敗時にデフォルトを再設定
        throw std::runtime_error("Failed to retrieve speaker channel mask or invalid channel mask.");
    }

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

    // XAudio終了化
    if (xaudio_ != nullptr)
    {
        xaudio_->Release();
        xaudio_ = nullptr;
    }

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
    audioResourcesTest[AUDIOID::BGM] = LoadAudioSource("Data/AudioData/TestAudio/BGM.wav");
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