#pragma once

#include <X3DAudio.h>
#include <memory>
#include <xaudio2.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"

// リスナー情報
struct Listener
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
struct Emitter
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

// オーディオソース
class AudioSource : public Component
{
public:
    AudioSource() = default;
    ~AudioSource() override { Stop(); }

    void Start() override {}
    void Update(float elapsedTime) override {};

    const char* GetName() const override { return "Audio"; }
    void OnGUI() override;

    // オーディオ呼び出し関数
    void SetAudio(int id);
    // 各オーディオ名設定
    void SetAudioName(const char* setName) { this->name = setName; }

    // 再生
    void Play(bool loop, float volume = 1.0f);
    void Play();

    // 停止
    void Stop();

    bool Feed(float targetValue, float elapsedTime);

    IXAudio2SourceVoice* GetSourceVoice() { return sourceVoice_; }
    void SetVolume(float volume) { sourceVoice_->SetVolume(volume); }
    void SetPitch(float value) { sourceVoice_->SetFrequencyRatio(value); }
    void AudioRelease();

    // 3Dオーディオの更新
    void Update3DAudio();

private:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

    Listener listener_;   // リスナー情報
    Emitter emitter_;     // エミッター情報

    float volumeControl = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;

    const char* name = "";
};
