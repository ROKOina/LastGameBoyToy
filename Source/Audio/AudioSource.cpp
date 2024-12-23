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

    // 再生中のものがあれば,即時反映 チェック外してもループ抜けない
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

        // エミッターの初期化
        emitter_.Initialize(resource_->GetWaveFormat());
    }
}

// 再生
void AudioSource::Play(bool loop, float volume)
{
    Stop();

    // 音量調整反映用
    volumeControl = volume;
    volume = volumeControl;

    // ループ制御
    isLooping = loop;
    loop = isLooping;

    // ソースボイスにデータを送信
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

// 停止
void AudioSource::Stop()
{
    if (!sourceVoice_)return;

    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->Stop();
}

// 指定された値に音量を近づけていく
bool AudioSource::Feed(float targetValue, float add)
{
    //現在の音量と指定された音量が同じ場合終了
    if (targetValue == volumeControl) { return true; }

    //指定された音量に近づける
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

// 3Dオーディオの更新
void AudioSource::Update3DAudio()
{
    // リスナーの更新
    listener_.Update();

    // エミッターの更新
    emitter_.Update();

    // X3DAudioの計算
    X3DAUDIO_DSP_SETTINGS dspSettings = {};
    float matrix[2] = {};
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
    sourceVoice_->SetVolume(volumeControl * dspSettings.EmitterToListenerDistance);
    sourceVoice_->SetFrequencyRatio(dspSettings.DopplerFactor);
}