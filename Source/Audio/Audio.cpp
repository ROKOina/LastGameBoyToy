#include "Misc.h"
#include "Audio/Audio.h"
#include "Components/System/GameObject.h"
#pragma comment(lib, "xaudio2.lib")

// コンストラクタ
Audio::Audio()
{
	HRESULT hr;

	// COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(&xaudio_, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// マスタリングボイス生成
	hr = xaudio_->CreateMasteringVoice(&masteringVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// X3DAudio初期化
	DWORD speakerChannelMask;
	hr = masteringVoice_->GetChannelMask(&speakerChannelMask);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = X3DAudioInitialize(speakerChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// BGMとSEを一括登録
	RegisterAudioSources();
}


// デストラクタ
Audio::~Audio()
{
	// マスタリングボイス破棄
	if (masteringVoice_ != nullptr)
	{
		masteringVoice_->DestroyVoice();
		masteringVoice_ = nullptr;
	}

	// XAudio終了化
	if (xaudio_ != nullptr)
	{
		xaudio_->Release();
		xaudio_ = nullptr;
	}

	// COM終了化
	CoUninitialize();
}

// BGMとSEを一括登録
void Audio::RegisterAudioSources()
{
	audioResources[AUDIOID::BGM] = LoadAudioSource("Data/AudioData/TestAudio/BGM.wav");
	audioResources[AUDIOID::SE] = LoadAudioSource("Data/AudioData/TestAudio/SE.wav");
}

// オーディオソース読み込み
std::shared_ptr<AudioResource> Audio::LoadAudioSource(const char* filename)
{
	return std::make_shared<AudioResource>(filename);
}

// 登録されたオーディオソースを取得
std::shared_ptr<AudioResource> Audio::GetAudioResource(AUDIOID id)
{
	return audioResources.at(id);
}