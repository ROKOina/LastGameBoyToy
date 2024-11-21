#pragma once

#include <xaudio2.h>
#include <cassert>
#include <map>

#include "Audio/AudioSource.h"
#include "Audio/AudioResource.h"

enum class AUDIOID
{
    BGM,
    SE,
    SCENE_GAME1,
    SCENE_GAME2,
    SCENE_TITLE,
    CURSOR,
    ENTER,
    BOSS_JUMPATTACK_START,
    BOSS_JUMPATTACK_END,
    BOSS_JUMPATTACK_GROUND,
    BOSS_SHOT,
    BOSS_POWERSHOT,
    BOSS_CHARGE,
    BOSS_WALK,

    //�v���C���[
    PLAYER_SHOOT,
    PLAYER_ATTACKULTSHOOT,
    PLAYER_ATTACKULTBOOM,
    PLAYER_CHARGE,
    PLAYER_DAMAGE,
    PLAYER_DASH,

    MAX
};

// �I�[�f�B�I
class Audio
{
public:
    Audio();
    ~Audio();

public:
    static void Initialize()
    {
        assert(instance == nullptr);
        instance = std::make_unique<Audio>();
    }

    static void Finalize()
    {
        assert(instance != nullptr);
        instance.reset();
    }

    // �C���X�^���X�擾
    static Audio& Instance() {
        assert(instance != nullptr);
        return *instance.get();
    }

    // �I�[�f�B�I�\�[�X�ǂݍ���
    std::shared_ptr<AudioResource> LoadAudioSource(const char* filename);
    std::shared_ptr<AudioResource> GetAudioResource(AUDIOID id);

    // �I�[�f�B�I�o�^
    void RegisterAudioSources();
    void RegisterAudioSources(AUDIOID id,const char* filename);

    IXAudio2* GetXAudio() const { return xaudio_; }

private:
    static inline std::unique_ptr<Audio> instance;

    IXAudio2* xaudio_ = nullptr;
    IXAudio2MasteringVoice* masteringVoice_ = nullptr;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResources;
};