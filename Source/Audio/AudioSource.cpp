#include "SystemStruct/Misc.h"
#include "Audio/AudioSource.h"
#include "Math\easing.h"
#include "Math\Mathf.h"

void AudioSource::OnGUI()
{
    ImGui::Text("Audio Name: %s", name);

    if (ImGui::Button("Play")) {
        Play(isLooping, volumeControl);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        Stop();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Loop", &isLooping);
    ImGui::SliderFloat("Volume", &volumeControl, 0.0f, 1.0f);

    // �Đ����̂��̂������,�������f �`�F�b�N�O���Ă����[�v�����Ȃ�
    if (isLooping)
        Play(isLooping, volumeControl);
}

void AudioSource::SetAudio(int id)
{
    resource_ = Audio::Instance().GetAudioResource(static_cast<AUDIOID>(id));

    if (resource_ != nullptr)
    {
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&sourceVoice_, &resource_->GetWaveFormat());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        // �G�~�b�^�[�̏�����
        emitter_.Initialize(resource_->GetWaveFormat());
    }
}

// �Đ�
void AudioSource::Play(bool loop, float volume)
{
    Stop();

    // ���ʒ������f�p
    volumeControl = volume;
    volume = volumeControl;

    // ���[�v����
    isLooping = loop;
    loop = isLooping;

    // �\�[�X�{�C�X�Ƀf�[�^�𑗐M
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    sourceVoice_->SubmitSourceBuffer(&buffer);

    HRESULT hr = sourceVoice_->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    sourceVoice_->SetVolume(volume * 0.1f);
}

void AudioSource::Play()
{
    Play(isLooping, volumeControl);
}

// ��~
void AudioSource::Stop()
{
    if (!sourceVoice_)return;

    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->Stop();
}

// �w�肳�ꂽ�l�ɉ��ʂ��߂Â��Ă���
bool AudioSource::Feed(float targetValue, float add)
{
    //���݂̉��ʂƎw�肳�ꂽ���ʂ������ꍇ�I��
    if (targetValue == volumeControl) { return true; }

    //�w�肳�ꂽ���ʂɋ߂Â���
    if ((targetValue - volumeControl) > 0.0f)
    {
        volumeControl = min((volumeControl + add), targetValue);
        SetVolume(volumeControl);
    }
    else
    {
        volumeControl = max((volumeControl - add), targetValue);
        SetVolume(volumeControl);
    }
}

void AudioSource::AudioRelease()
{
    if (sourceVoice_ != nullptr)
    {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
}

// 3D�I�[�f�B�I�̍X�V
void AudioSource::Update3DAudio()
{
    // ���X�i�[�̍X�V
    listener_.Update();

    // �G�~�b�^�[�̍X�V
    emitter_.Update();

    // X3DAudio�̌v�Z
    X3DAUDIO_DSP_SETTINGS dspSettings = {};
    float matrix[2] = {};
    dspSettings.pMatrixCoefficients = matrix;
    dspSettings.SrcChannelCount = emitter_.x3dEmitter.ChannelCount;
    dspSettings.DstChannelCount = 2; // �X�e���I�o��

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    X3DAudioCalculate(*x3dHandle,
        &listener_.x3dListener,
        &emitter_.x3dEmitter,
        X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER,
        &dspSettings);

    // �v�Z���ʂ𔽉f
    sourceVoice_->SetVolume(volumeControl * dspSettings.EmitterToListenerDistance);
    sourceVoice_->SetFrequencyRatio(dspSettings.DopplerFactor);
}