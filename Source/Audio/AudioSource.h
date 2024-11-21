#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"

// �I�[�f�B�I�\�[�X
class AudioSource : public Component
{
public:
    AudioSource() = default;
    ~AudioSource() override { Stop(); }

    void Start() override {}
    void Update(float elapsedTime) override {}

    const char* GetName() const override { return "Audio"; }
    void OnGUI() override;

    // �I�[�f�B�I�Ăяo���֐�
    void SetAudio(int id);
    // �e�I�[�f�B�I���ݒ�
    void SetAudioName(const char* setName) { this->name = setName; }

    // �Đ�
    void Play(bool loop, float volume = 1.0f);
    void Play();

    // ��~
    void Stop();

    bool Feed(float targetValue, float elapsedTime);

    IXAudio2SourceVoice* GetSourceVoice() { return sourceVoice_; }
    void SetVolume(float volume) { sourceVoice_->SetVolume(volume); }
    void SetPitch(float value) { sourceVoice_->SetFrequencyRatio(value); }
    void AudioRelease();

private:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

    float volumeControl = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;

    const char* name = "";
};
