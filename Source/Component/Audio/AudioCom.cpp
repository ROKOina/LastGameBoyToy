#include "AudioCom.h"

AudioCom::AudioCom(AUDIOID id, std::string name)
{
    RegisterSource(id, name);
}

void AudioCom::Update(float elapsedTime)
{
    for (auto& audioObj : audioSources)
    {
        audioObj.second.Update();
    }
}

void AudioCom::OnGUI()
{

}

void AudioCom::RegisterSource(AUDIOID id, std::string name)
{
    if (!audioSources[name].GetIsSource())
    {
        std::shared_ptr<AudioSource> source = std::make_shared<AudioSource>();
        source->SetAudio((int)id);

        audioSources[name].SetSource(source);
    }
}

void AudioObj::Update()
{
    Feed();

    source->SetVolume(volume);
    source->SetPitch(pitch);
}

void AudioObj::FeedStart(float targetValue, float add)
{
    feedFlag = true;
    feedTargetValue = targetValue;
    feedAddValue = add;
}

// �w�肳�ꂽ�l�ɉ��ʂ��߂Â��Ă���
bool AudioObj::Feed()
{
    if (feedFlag)
    {
        //���݂̉��ʂƎw�肳�ꂽ���ʂ������ꍇ�I��
        if (feedTargetValue == volume) {
            feedFlag = false; return true; }

        //�w�肳�ꂽ���ʂɋ߂Â���
        if ((feedTargetValue - volume) > 0.0f)
        {
            volume = min((volume + feedAddValue), feedTargetValue);
        }
        else
        {
            volume = max((volume - feedAddValue), feedTargetValue);
        }  
    }

    return false;
}
