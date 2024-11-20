#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"

// オーディオソース
class AudioSource : public Component
{
public:
    AudioSource() = default;
    ~AudioSource() override { Stop(); }

    void Start() override {}
    void Update(float elapsedTime) override {}

    const char* GetName() const override { return "Audio"; }
    void OnGUI() override;

    // オーディオ呼び出し関数
    void SetAudio(int id);
    // 各オーディオ名設定
    void SetAudioName(const char* setName) { this->name = setName; }

    // 再生
    void Play(bool loop, float volume = 1.0f);
    // 停止
    void Stop();

    void Feed(float startVolume, float endVolume, float time);

    IXAudio2SourceVoice* GetSourceVoice() { return sourceVoice_; }
    void AudioRelease();

private:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

    float volumeControl = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;

    //音量変更用変数
    float feedStartValue = 0.0f;
    float feedEndValue = 0.0f;
    float feedTime = 0.0f;
    bool feedFlag = false;

    const char* name = "";
};
