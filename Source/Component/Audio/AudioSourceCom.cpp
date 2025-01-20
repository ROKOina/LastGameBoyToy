#include "AudioSourceCom.h"

#include "SystemStruct/Misc.h"
#include "Math\easing.h"
#include "Math\Mathf.h"

void AudioSourceCom::Update(float elapsedTime)
{
    for (auto& [id, feedState] : feedStates_)
    {
        if (feedState.isFeeding)
        {
            // フィード処理を実行
            if (Feed(id))
            {
                feedState.isFeeding = false; // フィードが終了したらフラグを下げる
            }
        }
    }
}

void AudioSourceCom::OnGUI()
{
    for (const auto& [id, name] : audioNames_)
    {
        ImGui::Text("Audio Name: %s", name.c_str());

        // ループ設定
        bool loop = loopFlags_[id];
        std::string loopLabel = "Loop##" + std::to_string(id);
        if (ImGui::Checkbox(loopLabel.c_str(), &loop))
        {
            SetLoop(id, loop);
        }
        ImGui::SameLine();

        // 再生ボタン
        std::string playLabel = "Play##" + std::to_string(id);
        if (ImGui::Button(playLabel.c_str()))
        {
            AudioPlay(id, loopFlags_[id], volumeControls_[id]);
        }
        ImGui::SameLine();

        // 停止ボタン
        std::string stopLabel = "Stop##" + std::to_string(id);
        if (ImGui::Button(stopLabel.c_str()))
        {
            Stop(id);
        }
        ImGui::SameLine();

        // 音量調整
        float volume = volumeControls_[id];
        std::string volumeLabel = "Volume##" + std::to_string(id);
        if (ImGui::SliderFloat(volumeLabel.c_str(), &volume, 0.0f, 1.0f))
        {
            SetVolume(id, volume);
        }

        ImGui::Separator();
    }
}

// オーディオを追加
void AudioSourceCom::AddAudio(int id)
{
    auto resource = Audio::Instance().GetAudioResourceID(static_cast<AUDIOID>(id));
    if (resource)
    {
        IXAudio2SourceVoice* voice = nullptr;
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&voice, &resource->GetWaveFormat());
        if (SUCCEEDED(hr))
        {
            resources_[id] = resource;
            sourceVoices_[id] = voice;
            audioNames_[id] = Audio::Instance().GetAudioName(static_cast<AUDIOID>(id));
            loopFlags_[id] = false;
            volumeControls_[id] = 1.0f;
        }
    }
}

// 再生
void AudioSourceCom::AudioPlay(int id)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end() && resources_[id])
    {
        Stop(id);

        IXAudio2SourceVoice* sourceVoice = it->second;
        auto resource = resources_[id];

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = resource->GetAudioBytes();
        buffer.pAudioData = resource->GetAudioData();
        buffer.LoopCount = loopFlags_[id] ? XAUDIO2_LOOP_INFINITE : 0;
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        sourceVoice->SubmitSourceBuffer(&buffer);
        sourceVoice->SetVolume(volumeControls_[id]);
        sourceVoice->Start();
    }
}

void AudioSourceCom::AudioPlay(int id, bool loop, float volume)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end() && resources_[id])
    {
        Stop(id);

        IXAudio2SourceVoice* sourceVoice = it->second;
        auto resource = resources_[id];

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = resource->GetAudioBytes();
        buffer.pAudioData = resource->GetAudioData();
        buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        sourceVoice->SubmitSourceBuffer(&buffer);
        sourceVoice->SetVolume(volume);
        sourceVoice->Start();

        loopFlags_[id] = loop;
        volumeControls_[id] = volume;
    }
}

// 停止
void AudioSourceCom::Stop(int id)
{
    auto it = sourceVoices_.find(id);
    if (it != sourceVoices_.end())
    {
        it->second->Stop();
        it->second->FlushSourceBuffers();
    }
}

void AudioSourceCom::StopAll()
{
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->Stop();
        voice->FlushSourceBuffers();
    }
}

// エミッター再生
void AudioSourceCom::EmitterPlay(float volume)
{
    //// 音量調整反映用
    //volumeControl = volume;

    //// ソースボイスにデータを送信
    //XAUDIO2_BUFFER buffer = { 0 };
    //buffer.AudioBytes = resource_->GetAudioBytes();
    //buffer.pAudioData = resource_->GetAudioData();
    //buffer.Flags = XAUDIO2_END_OF_STREAM;

    //if (buffer.pAudioData == nullptr || buffer.AudioBytes == 0) {
    //    assert("ソースボイスにデータに問題あり");
    //    return;
    //}

    //// 3Dオーディオの更新
    //Update3DAudio();

    //// エミッター再生
    //sourceVoice_->Stop();
    //sourceVoice_->FlushSourceBuffers();
    //sourceVoice_->SubmitSourceBuffer(&buffer);

    //HRESULT hr = sourceVoice_->Start();
    //_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 音量設定
void AudioSourceCom::SetVolume(int id, float volume)
{
    if (sourceVoices_.find(id) != sourceVoices_.end())
    {
        sourceVoices_[id]->SetVolume(volume);
        volumeControls_[id] = volume;
    }
}

// ループ設定
void AudioSourceCom::SetLoop(int id, bool loop)
{
    loopFlags_[id] = loop;
}

void AudioSourceCom::FeedStart(int id, float targetValue, float add)
{
    if (feedStates_.find(id) != feedStates_.end())
    {
        auto& feedState = feedStates_[id];
        feedState.targetVolume = targetValue;
        feedState.feedStep = add;
        feedState.isFeeding = true;
    }
}

// 指定された値に音量を近づけていく
bool AudioSourceCom::Feed(int id)
{
    if (feedStates_.find(id) != feedStates_.end())
    {
        auto& feedState = feedStates_[id];
        float& currentVolume = feedState.currentVolume;
        float targetVolume = feedState.targetVolume;

        // 目標音量と現在音量が等しい場合、終了
        if (currentVolume == targetVolume)
        {
            return true;
        }

        // 音量を目標値に向かって調整
        if (targetVolume > currentVolume)
        {
            currentVolume = min(currentVolume + feedState.feedStep, targetVolume);
        }
        else
        {
            currentVolume = max(currentVolume - feedState.feedStep, targetVolume);
        }

        // 音量を実際のソースに反映
        SetVolume(id, currentVolume);

        // 調整が完了した場合
        return currentVolume == targetVolume;
    }
    return true; // 指定IDが存在しない場合、即終了とする
}

void AudioSourceCom::AudioRelease()
{
    for (auto& pair : sourceVoices_)
    {
        if (pair.second)
        {
            pair.second->DestroyVoice();
        }
    }
    sourceVoices_.clear();
    resources_.clear();
    audioNames_.clear();
}

// 3Dオーディオの更新
void AudioSourceCom::Update3DAudio()
{
    // リスナーの更新
    listener_.Update();

    // エミッターの更新
    emitter_.Update();

    // X3DAudioの計算
    X3DAUDIO_DSP_SETTINGS dspSettings = {};
    float matrix[8] = {};
    dspSettings.pMatrixCoefficients = matrix;
    dspSettings.SrcChannelCount = emitter_.x3dEmitter.ChannelCount;
    dspSettings.DstChannelCount = 2; // ステレオ出力

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    X3DAudioCalculate(*x3dHandle,
        &listener_.x3dListener,
        &emitter_.x3dEmitter,
        X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER,
        &dspSettings);

    // 計算結果を反映
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->SetFrequencyRatio(dspSettings.DopplerFactor);
        voice->SetOutputMatrix(nullptr, dspSettings.SrcChannelCount, dspSettings.DstChannelCount, matrix);
        voice->SetVolume(volumeControls_[id] * dspSettings.EmitterToListenerDistance);
    }
}