#include "Misc.h"
#include "Audio/Audio.h"
#include "Components/System/GameObject.h"
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
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio������
	hr = XAudio2Create(&xaudio_, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �}�X�^�����O�{�C�X����
	hr = xaudio_->CreateMasteringVoice(&masteringVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// X3DAudio������
	DWORD speakerChannelMask;
	hr = masteringVoice_->GetChannelMask(&speakerChannelMask);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = X3DAudioInitialize(speakerChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

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