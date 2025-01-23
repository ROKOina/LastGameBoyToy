#include "AudioSourceCom.h"

#include "SystemStruct/Misc.h"
#include "Math\easing.h"
#include "Math\Mathf.h"

using namespace DirectX;

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

    // 3Dオーディオの更新
    if (!t)
        Update3DAudio(elapsedTime);
    t = (!t);
}

// 3Dオーディオの更新
void AudioSourceCom::Update3DAudio(float elapsedTime)
{
    if (audio->GetListener().position.x != audio->GetListener().x3dListener.Position.x
        || audio->GetListener().position.z != audio->GetListener().x3dListener.Position.z)
    {
        const XMVECTOR v1 = XMLoadFloat3(&audio->GetListener().position);
        const XMVECTOR v2 = XMVectorSet(audio->GetListener().x3dListener.Position.x, audio->GetListener().x3dListener.Position.y, audio->GetListener().x3dListener.Position.z, 0.f);

        XMVECTOR vDelta = v1 - v2;

        audio->GetListener().listenerAngle = float(atan2(XMVectorGetX(vDelta), XMVectorGetZ(vDelta)));

        vDelta = XMVectorSetY(vDelta, 0.f);
        vDelta = XMVector3Normalize(vDelta);

        XMFLOAT3 tmp;
        XMStoreFloat3(&tmp, vDelta);

        audio->GetListener().x3dListener.OrientFront.x = tmp.x;
        audio->GetListener().x3dListener.OrientFront.y = 0.f;
        audio->GetListener().x3dListener.OrientFront.z = tmp.z;
    }

    if (audio->GetListener().useListenerCone)
    {
        X3DAUDIO_CONE a = audio->GetListener().Listener_DirectionalCone;
        audio->GetListener().pCone = std::make_shared<X3DAUDIO_CONE>(a);
    }
    else
    {
        audio->GetListener().x3dListener.pCone = nullptr;
    }
    if (audio->GetUseInnerRadius())
    {
        audio->GetEmitter().x3dEmitter.InnerRadius = 2.0f;
        audio->GetEmitter().x3dEmitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
    }
    else
    {
        audio->GetEmitter().x3dEmitter.InnerRadius = 0.0f;
        audio->GetEmitter().x3dEmitter.InnerRadiusAngle = 0.0f;
    }

    if (elapsedTime > 0)
    {
        XMVECTOR v1 = XMLoadFloat3(&audio->GetListener().position);
        XMVECTOR v2 = XMVectorSet(audio->GetListener().x3dListener.Position.x, audio->GetListener().x3dListener.Position.y, audio->GetListener().x3dListener.Position.z, 0);

        const XMVECTOR lVelocity = (v1 - v2) / elapsedTime;
        audio->GetListener().x3dListener.Position.x = audio->GetListener().position.x;
        audio->GetListener().x3dListener.Position.y = audio->GetListener().position.y;
        audio->GetListener().x3dListener.Position.z = audio->GetListener().position.z;

        XMFLOAT3 tmp;
        XMStoreFloat3(&tmp, lVelocity);
        audio->GetListener().x3dListener.Velocity.x = tmp.x;
        audio->GetListener().x3dListener.Velocity.y = tmp.y;
        audio->GetListener().x3dListener.Velocity.z = tmp.z;

        v1 = XMLoadFloat3(&audio->GetEmitter().position);
        v2 = XMVectorSet(audio->GetEmitter().x3dEmitter.Position.x, audio->GetEmitter().x3dEmitter.Position.y, audio->GetEmitter().x3dEmitter.Position.z, 0.f);

        const XMVECTOR eVelocity = (v1 - v2) / elapsedTime;
        audio->GetEmitter().x3dEmitter.Position.x = audio->GetEmitter().position.x;
        audio->GetEmitter().x3dEmitter.Position.y = audio->GetEmitter().position.y;
        audio->GetEmitter().x3dEmitter.Position.z = audio->GetEmitter().position.z;

        XMStoreFloat3(&tmp, eVelocity);
        audio->GetEmitter().x3dEmitter.Velocity.x = tmp.x;
        audio->GetEmitter().x3dEmitter.Velocity.y = tmp.y;
        audio->GetEmitter().x3dEmitter.Velocity.z = tmp.z;

    }

    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB;
    if (audio->GetUseRedirectToLFE())
    {
        // On devices with an LFE channel, allow the mono source data
        // to be routed to the LFE destination channel.
        dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
    }

    // 3Dオーディオの更新
    X3DAudioCalculate(*audio->GetX3DAudioHandle(), &audio->GetListener().x3dListener, &audio->GetEmitter().x3dEmitter, dwCalcFlags, audio->GetDspSettings().get());

    // 計算結果を反映
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->SetFrequencyRatio(audio->GetDspSettings()->DopplerFactor);
        voice->SetOutputMatrix(audio->GetMasteringVoice(), INPUTCHANNELS, audio->GetNChannels(), audio->GetMatrixCoefficients());
        voice->SetOutputMatrix(audio->GetSubmixVoice(), 1, 1, &audio->GetDspSettings()->ReverbLevel);

        // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * audio->GetDspSettings()->LPFDirectCoefficient), 1.0f }; 
        voice->SetOutputFilterParameters(audio->GetMasteringVoice(), &FilterParametersDirect);

        // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
        XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * audio->GetDspSettings()->LPFReverbCoefficient), 1.0f }; 
        voice->SetOutputFilterParameters(audio->GetSubmixVoice(), &FilterParametersReverb);
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
void AudioSourceCom::EmitterPlay(int id)
{
    // 音源が存在するか確認
    if (sourceVoices_.find(id) == sourceVoices_.end() || !resources_[id])
    {
        assert("指定されたIDのソースボイスが存在しません");
        return;
    }

    // 対応するソースボイスを取得
    IXAudio2SourceVoice* sourceVoice = sourceVoices_[id];

    // XAUDIO2_BUFFERの設定
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resources_[id]->GetAudioBytes();
    buffer.pAudioData = resources_[id]->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    // ソースボイスのバッファをフラッシュして再送信
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();
    sourceVoice->SubmitSourceBuffer(&buffer);


    // 再生開始
    HRESULT hr = sourceVoice->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
void AudioSourceCom::EmitterPlay(int id, bool loop, float volume)
{
    // 音源が存在するか確認
    if (sourceVoices_.find(id) == sourceVoices_.end() || !resources_[id])
    {
        assert("指定されたIDのソースボイスが存在しません");
        return;
    }

    // 対応するソースボイスを取得
    IXAudio2SourceVoice* sourceVoice = sourceVoices_[id];

    // XAUDIO2_BUFFERの設定
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = resources_[id]->GetAudioBytes();
    buffer.pAudioData = resources_[id]->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    // ループ設定
    if (loop)
    {
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    // ボリューム設定
    volumeControls_[id] = volume;

    // ソースボイスのバッファをフラッシュして再送信
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();
    sourceVoice->SubmitSourceBuffer(&buffer);

    // 再生開始
    HRESULT hr = sourceVoice->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
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
    for (auto& [id, voice] : sourceVoices_)
    {
        voice->DestroyVoice();
    }
    sourceVoices_.clear();
    resources_.clear();
    audioNames_.clear();
    loopFlags_.clear();
    volumeControls_.clear();
    feedStates_.clear();
}