#include "AudioSourceCom.h"

#include "SystemStruct/Misc.h"
#include "Math\easing.h"
#include "Math\Mathf.h"

void AudioSourceCom::OnGUI()
{

}

void AudioSourceCom::SetAudio(int id)
{
    resource_ = Audio::Instance().GetAudioResourceID(static_cast<AUDIOID>(id));

    if (resource_ != nullptr)
    {
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&sourceVoice_, &resource_->GetWaveFormat());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        // エミッターの初期化
        emitter_.Initialize(resource_->GetWaveFormat());
    }
}

// 再生
void AudioSourceCom::Play(bool loop, float volume)
{
    Stop();

    // 音量調整反映用
    volumeControl = volume;
    // ループ制御
    isLooping = loop;

    // ソースボイスにデータを送信
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = isLooping ? XAUDIO2_LOOP_INFINITE : 0;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) throw std::runtime_error("Failed to submit source buffer.");

    hr = sourceVoice_->Start();
    if (FAILED(hr)) throw std::runtime_error("Failed to start source voice.");

    sourceVoice_->SetVolume(volumeControl * 0.1f);
}

void AudioSourceCom::Play()
{
    Play(isLooping, volumeControl);
}

// エミッター再生
void AudioSourceCom::EmitterPlay(float volume)
{
    // 音量調整反映用
    volumeControl = volume;

    // ソースボイスにデータを送信
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    if (buffer.pAudioData == nullptr || buffer.AudioBytes == 0) {
        assert("ソースボイスにデータに問題あり");
        return;
    }

    // 3Dオーディオの更新
    Update3DAudio();

    // エミッター再生
    sourceVoice_->Stop();
    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->SubmitSourceBuffer(&buffer);

    HRESULT hr = sourceVoice_->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// 停止
void AudioSourceCom::Stop()
{
    if (!sourceVoice_)return;

    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->Stop();
}

// 指定された値に音量を近づけていく
bool AudioSourceCom::Feed(float targetValue, float add)
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

void AudioSourceCom::AudioRelease()
{
    if (sourceVoice_ != nullptr)
    {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
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
    sourceVoice_->SetFrequencyRatio(dspSettings.DopplerFactor);
    sourceVoice_->SetOutputMatrix(nullptr, dspSettings.SrcChannelCount, dspSettings.DstChannelCount, matrix);
    sourceVoice_->SetVolume(volumeControl * dspSettings.EmitterToListenerDistance);
}