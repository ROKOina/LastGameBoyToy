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
    for (auto& audioObj : audioSources)
    {
        const std::string& name = audioObj.first;  // オーディオの名前
        AudioObj& obj = audioObj.second;

        ImGui::Text("Audio Name: %s", name.c_str());

        // Loopチェックボックス（識別子付与）
        bool loopFlag = obj.GetLoopFlag();
        std::string loopLabel = "Loop##" + name;  // 識別子付きラベル
        if (ImGui::Checkbox(loopLabel.c_str(), &obj.GetLoopFlag())) {
            obj.SetLoopFlag(obj.GetLoopFlag());
        }
        ImGui::SameLine();

        // Playボタン（識別子付与）
        std::string playLabel = "Play##" + name;
        if (ImGui::Button(playLabel.c_str())) {
            Play(name, obj.GetLoopFlag(), obj.GetVolume());
        }
        ImGui::SameLine();

        // Stopボタン（識別子付与）
        std::string stopLabel = "Stop##" + name;
        if (ImGui::Button(stopLabel.c_str())) {
            Stop(name);
        }
        ImGui::SameLine();

        // Volumeスライダー（識別子付与）
        float volume = obj.GetVolume();
        std::string volumeLabel = "Volume##" + name;
        if (ImGui::SliderFloat(volumeLabel.c_str(), &volume, 0.0f, 1.0f)) {
            obj.SetVolume(volume);
        }

        ImGui::Separator();  // 各オーディオオブジェクトの区切り
    }
}

void AudioCom::RegisterSource(AUDIOID id, std::string name)
{
    if (!audioSources[name].GetIsSource())
    {
        std::shared_ptr<AudioSource> source = std::make_shared<AudioSource>();
        source->SetAudio((int)id);
        audioName = name;

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
            feedFlag = false; return true;
        }

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