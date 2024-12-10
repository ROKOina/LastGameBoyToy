#pragma once

#include <memory>
#include <xaudio2.h>
#include <x3daudio.h>

#include "Audio/AudioResource.h"
#include "Components/System/Component.h"

// オーディオソース
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
	// 再生
	void Play(bool loop, float volume = 1.0f);
	// エミッター再生  curveDistanceScaler 減衰範囲
	void Play3D(bool loop, float curveDistanceScaler, float volume = 1.0f);
	// 停止
	void Stop();

	// 各オーディオ名設定 
	void SetAudioName(const char* setName) { this->name = setName; }

	// オーディオ呼び出し関数
	void SetAudioResourceId(int id);
	// リスナー設定
	void SetListener(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top);
	// エミッター設定
	void SetEmitter(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top);
	// ボリューム設定BGM
	void SetVolume(float volume);

	void AudioRelease();

private:
	// エミッターの初期設定
	void PreferenceEmitter();
	// 3Dオーディオ更新処理
	void Audio3dUpdate();
private:
	IXAudio2SourceVoice*									sourceVoice_ = nullptr;
	std::shared_ptr<AudioResource>					resource_		= nullptr;
	std::shared_ptr<X3DAUDIO_DSP_SETTINGS> dspSettings	= nullptr;

	// リスナー情報
	std::shared_ptr<X3DAUDIO_LISTENER> listener_ = nullptr;

	// エミッター情報
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
