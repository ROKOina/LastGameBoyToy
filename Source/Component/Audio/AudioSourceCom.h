#pragma once
#include <memory>
#include <xaudio2.h>

#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"
#include <X3DAudio.h>

// リスナー情報
struct Listener_
{
    X3DAUDIO_LISTENER x3dListener = {};
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 front = { 0.0f, 0.0f, 1.0f }; // 前方向
    DirectX::XMFLOAT3 top = { 0.0f, 1.0f, 0.0f };   // 上方向

    void Update()
    {
        // ベクトルの正規化
        DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
        DirectX::XMStoreFloat3(&top, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&top)));

        // top を front に直交させる
        DirectX::XMVECTOR frontVec = DirectX::XMLoadFloat3(&front);
        DirectX::XMVECTOR topVec = DirectX::XMLoadFloat3(&top);
        topVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frontVec, DirectX::XMVector3Cross(topVec, frontVec)));
        DirectX::XMStoreFloat3(&top, topVec);

        // X3DAUDIO_LISTENER に反映
        x3dListener.Position = { position.x, position.y, position.z };
        x3dListener.Velocity = { velocity.x, velocity.y, velocity.z };
        x3dListener.OrientFront = { front.x, front.y, front.z };
        x3dListener.OrientTop = { top.x, top.y, top.z };
    }
};

// エミッター情報
struct Emitter_
{
    X3DAUDIO_EMITTER x3dEmitter = {};
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };

    void Initialize(const WAVEFORMATEX& waveFormat)
    {
        x3dEmitter.CurveDistanceScaler = 1.0f; // 距離減衰スケール
        x3dEmitter.DopplerScaler = 1.0f;       // ドップラー効果スケール
        x3dEmitter.ChannelCount = waveFormat.nChannels;
        x3dEmitter.Position = { position.x, position.y, position.z };
        x3dEmitter.Velocity = { velocity.x, velocity.y, velocity.z };
    }

    void Update()
    {
        x3dEmitter.Position = { position.x, position.y, position.z };
        x3dEmitter.Velocity = { velocity.x, velocity.y, velocity.z };
    }
};

class AudioSourceCom : public Component
{
public:
    AudioSourceCom() = default;
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
    void Update3DAudio();

    Listener_ GetListener() { return listener_; }
    Emitter_ GetEmitter_() { return emitter_; }
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

    Listener_ listener_;   // リスナー情報
    Emitter_ emitter_;    // エミッター情報

    bool isPlaying = false;
};

