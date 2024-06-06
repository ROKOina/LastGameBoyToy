#pragma once

#include <xaudio2.h>
#include "Audio/AudioSource.h"

// �I�[�f�B�I
class Audio
{
public:
	Audio();
	~Audio();

public:
	// �C���X�^���X�擾
	static Audio& Instance() { return *instance; }

	// �I�[�f�B�I�\�[�X�ǂݍ���
	std::unique_ptr<AudioSource> LoadAudioSource(const char* filename);

private:
	static Audio*			instance;

	IXAudio2*				xaudio_ = nullptr;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;
};