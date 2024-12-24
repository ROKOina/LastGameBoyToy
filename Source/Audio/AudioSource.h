#pragma once

#include <X3DAudio.h>
#include <memory>
#include <xaudio2.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Component\System\Component.h"

// ���X�i�[���
struct Listener
{
    X3DAUDIO_LISTENER x3dListener = {};
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 front = { 0.0f, 0.0f, 1.0f }; // �O����
    DirectX::XMFLOAT3 top = { 0.0f, 1.0f, 0.0f };   // �����

    void Update()
    {
        // �x�N�g���̐��K��
        DirectX::XMStoreFloat3(&front, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
        DirectX::XMStoreFloat3(&top, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&top)));

        // top �� front �ɒ���������
        DirectX::XMVECTOR frontVec = DirectX::XMLoadFloat3(&front);
        DirectX::XMVECTOR topVec = DirectX::XMLoadFloat3(&top);
        topVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(frontVec, DirectX::XMVector3Cross(topVec, frontVec)));
        DirectX::XMStoreFloat3(&top, topVec);

        // X3DAUDIO_LISTENER �ɔ��f
        x3dListener.Position = { position.x, position.y, position.z };
        x3dListener.Velocity = { velocity.x, velocity.y, velocity.z };
        x3dListener.OrientFront = { front.x, front.y, front.z };
        x3dListener.OrientTop = { top.x, top.y, top.z };
    }
};

// �G�~�b�^�[���
struct Emitter
{
    X3DAUDIO_EMITTER x3dEmitter = {};
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };

    void Initialize(const WAVEFORMATEX& waveFormat)
    {
        x3dEmitter.CurveDistanceScaler = 1.0f; // ���������X�P�[��
        x3dEmitter.DopplerScaler = 1.0f;       // �h�b�v���[���ʃX�P�[��
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

// �I�[�f�B�I�\�[�X
class AudioSource : public Component
{
public:
    AudioSource() = default;
    ~AudioSource() override { Stop(); }

    void Start() override {}
    void Update(float elapsedTime) override {};

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

    // 3D�I�[�f�B�I�̍X�V
    void Update3DAudio();

private:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

    Listener listener_;   // ���X�i�[���
    Emitter emitter_;     // �G�~�b�^�[���

    float volumeControl = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;

    const char* name = "";
};
