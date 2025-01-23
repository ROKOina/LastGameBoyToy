#pragma once
#include <memory>
#include <xaudio2.h>

#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"
#include <X3DAudio.h>

class AudioSourceCom : public Component
{
public:
    AudioSourceCom() { std::shared_ptr<Audio> audio = std::make_shared<Audio>(); };
    ~AudioSourceCom() override { StopAll(); }

    void Start() override {}
    void Update(float elapsedTime) override;

    const char* GetName() const override { return "Audio"; }
    void OnGUI() override;

    // オーディオの追加
    void AddAudio(int id);

    // 再生
    void AudioPlay(int id);
    void AudioPlay(int id, bool loop, float volume = 1.0f);

    // 停止
    void Stop(int id);
    void StopAll();

    void SetVolume(int id, float volume);
    void SetLoop(int id, bool loop);

    // エミッター再生
    void EmitterPlay(int id);
    void EmitterPlay(int id, bool loop, float volume);

    // 音量フェード制御
    void FeedStart(int id, float targetValue, float add);
    bool Feed(int id);

    void AudioRelease();

    // 3Dオーディオの更新
    void Update3DAudio(float elapsedTime);
private:
    struct FeedState {
        float currentVolume = 1.0f;
        float targetVolume = 1.0f;
        float feedStep = 0.0f;
        bool isFeeding = false;
    };

private:
    std::map<int, IXAudio2SourceVoice*> sourceVoices_;
    std::map<int, std::shared_ptr<AudioResource>> resources_;
    std::map<int, std::string> audioNames_;
    std::map<int, bool> loopFlags_;
    std::map<int, float> volumeControls_;
    std::map<int, FeedState> feedStates_;  // フィード状態

    bool isPlaying = false;

    std::shared_ptr<Audio> audio = nullptr;

    bool t = false;
};

