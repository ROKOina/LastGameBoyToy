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

// 指定された値に音量を近づけていく
bool AudioObj::Feed()
{
    if (feedFlag)
    {
        //現在の音量と指定された音量が同じ場合終了
        if (feedTargetValue == volume) {
            feedFlag = false; return true; }

        //指定された音量に近づける
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
