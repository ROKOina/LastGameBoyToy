#include "SystemStruct/Misc.h"
#include "Audio/Audio.h"

#pragma comment(lib, "xaudio2.lib")

// �R���X�g���N�^
Audio::Audio()
{
    HRESULT hr;

    // COM�̏�����
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

    // XAudio2 �̏�����
    hr = XAudio2Create(&xaudio_, createFlags);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to initialize XAudio2.");
    }

    // �}�X�^�����O�{�C�X����
    hr = xaudio_->CreateMasteringVoice(&masteringVoice_);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create mastering voice.");
    }

    // �X�s�[�J�[�`���l���}�X�N�̎擾
    DWORD speakerChannelMask = SPEAKER_STEREO;
    hr = masteringVoice_->GetChannelMask(&speakerChannelMask);
    if (FAILED(hr) || speakerChannelMask == 0)
    {
        throw std::runtime_error("Failed to retrieve speaker channel mask or invalid channel mask.");
    }

    // X3DAudio ������
    hr = X3DAudioInitialize(speakerChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle_);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to initialize X3DAudio.");
    }

    // BGM��SE���ꊇ�o�^
    RegisterAudioSources();
}

// �f�X�g���N�^
Audio::~Audio()
{
    // �}�X�^�����O�{�C�X�j��
    if (masteringVoice_ != nullptr)
    {
        masteringVoice_->DestroyVoice();
        masteringVoice_ = nullptr;
    }

    // XAudio�I����
    if (xaudio_ != nullptr)
    {
        xaudio_->Release();
        xaudio_ = nullptr;
    }

    // COM�I����
    CoUninitialize();
}

// BGM��SE���ꊇ�o�^
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

    //�v���C���[
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

// �I�[�f�B�I�\�[�X�ǂݍ���
std::shared_ptr<AudioResource> Audio::LoadAudioSource(const char* filename)
{
    return std::make_shared<AudioResource>(filename);
}

// �o�^���ꂽ�I�[�f�B�I�\�[�X���擾
std::shared_ptr<AudioResource> Audio::GetAudioResource(AUDIOID id)
{
    return audioResources.at(id);
}