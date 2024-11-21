#pragma once

#include <xaudio2.h>
#include <X3DAudio.h>
#include <cassert>
#include <map>

#include "Audio/AudioSource.h"
#include "Audio/AudioResource.h"

#include "Components/System/GameObject.h"

enum class AUDIOID
{
    BGM,
    SE,
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

    IXAudio2* GetXAudio() const { return xaudio_; }
    IXAudio2MasteringVoice* GetMasteringVoice() const { return masteringVoice_; }
    const X3DAUDIO_HANDLE* GetX3DAudioHandle() const { return &x3dAudioHandle_; }

private:
    static inline std::unique_ptr<Audio> instance;

    IXAudio2* xaudio_ = nullptr;
    IXAudio2MasteringVoice* masteringVoice_ = nullptr;

    // 3D�I�[�f�B�I�֘A
    X3DAUDIO_HANDLE x3dAudioHandle_;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResources;
};
