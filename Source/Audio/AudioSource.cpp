#include "SystemStruct/Misc.h"
#include "Audio/AudioSource.h"

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

// ��~
void AudioSource::Stop()
{
    if (!sourceVoice_)return;

    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->Stop();
}

void AudioSource::Feed(float startVolume, float endVolume, float time)
{

}

void AudioSource::AudioRelease()
{
    if (sourceVoice_ != nullptr)
    {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
}