#pragma once

#include <memory>
#include <xaudio2.h>
#include <x3daudio.h>

#include "Audio/AudioResource.h"
#include "Components/System/Component.h"

// �I�[�f�B�I�\�[�X
class AudioSource : public Component
{
public:
	AudioSource();
	~AudioSource() override { Stop(); }

	void Start() override {}
	void Update(float elapsedTime) override {}

	const char* GetName() const override { return "Audio"; }
	void OnGUI() override;

public:
	// �Đ�
	void Play(bool loop, float volume = 1.0f);
	// �G�~�b�^�[�Đ�  curveDistanceScaler �����͈�
	void Play3D(bool loop, float curveDistanceScaler, float volume = 1.0f);
	// ��~
	void Stop();

	// �e�I�[�f�B�I���ݒ� 
	void SetAudioName(const char* setName) { this->name = setName; }

	// �I�[�f�B�I�Ăяo���֐�
	void SetAudioResourceId(int id);
	// ���X�i�[�ݒ�
	void SetListener(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top);
	// �G�~�b�^�[�ݒ�
	void SetEmitter(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top);
	// �{�����[���ݒ�BGM
	void SetVolume(float volume);

	void AudioRelease();

private:
	// �G�~�b�^�[�̏����ݒ�
	void PreferenceEmitter();
	// 3D�I�[�f�B�I�X�V����
	void Audio3dUpdate();
private:
	IXAudio2SourceVoice*									sourceVoice_ = nullptr;
	std::shared_ptr<AudioResource>					resource_		= nullptr;
	std::shared_ptr<X3DAUDIO_DSP_SETTINGS> dspSettings	= nullptr;

	// ���X�i�[���
	std::shared_ptr<X3DAUDIO_LISTENER> listener_ = nullptr;

	// �G�~�b�^�[���
	std::shared_ptr<X3DAUDIO_EMITTER>	emitter_ = nullptr;
	X3DAUDIO_VECTOR emitterFront	 = {};
	X3DAUDIO_VECTOR emitterPosition = {};
	X3DAUDIO_VECTOR emitterTop		 = {};

	float volumeControl		 = 1.0f;
	float curveDistanceScaler = 100.0f;

	bool isPlaying  = false;
	bool isLooping = false;

	const char* name = "";
};
