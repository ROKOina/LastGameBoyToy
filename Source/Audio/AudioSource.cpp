#include "Audio/Audio.h"
#include "Audio/AudioSource.h"

AudioSource::AudioSource()
{
    listener_ = std::make_shared<X3DAUDIO_LISTENER>();
    emitter_ = std::make_shared<X3DAUDIO_EMITTER>();
    dspSettings = std::make_shared<X3DAUDIO_DSP_SETTINGS>();
}

void AudioSource::OnGUI()
{
    ImGui::Text("Audio Name: %s", name);

    if (ImGui::TreeNode((char*)u8"リスナー")) {
        ImGui::DragFloat3("Position", &listener_->Position.x, 0.1f);
        ImGui::DragFloat3("Front", &listener_->OrientFront.x, 0.1f);
        ImGui::DragFloat3("Top", &listener_->OrientTop.x, 0.1f);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode((char*)u8"エミッター")) {
        ImGui::DragFloat3("Position", &emitter_->Position.x, 0.1f);
        ImGui::DragFloat3("Front", &emitter_->OrientFront.x, 0.1f);
        ImGui::DragFloat3("Top", &emitter_->OrientTop.x, 0.1f);

        ImGui::SliderFloat((char*)u8"減衰範囲", &curveDistanceScaler, 0.0f, 10.0f);
        ImGui::TreePop();
    }

    if (ImGui::Button("Play")) {
        Play(isLooping, volumeControl);
    }
    ImGui::SameLine();
    if (ImGui::Button("Play3D")) {
        Play3D(isLooping, curveDistanceScaler, volumeControl);
    }
    if (ImGui::Button("Stop")) {
        Stop();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Loop", &isLooping);
    ImGui::SliderFloat("Volume", &volumeControl, 0.0f, 1.0f);

    // 再生中のものがあれば,即時反映 チェック外してもループ抜けない
    if (isLooping)
    {
        Play(isLooping, volumeControl);
        Play3D(isLooping, curveDistanceScaler, volumeControl);
    }
}

// 通常再生
void AudioSource::Play(bool loop, float volume)
{
    if (!sourceVoice_ || !resource_) return;

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

    HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = sourceVoice_->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    sourceVoice_->SetVolume(volume * 0.1f);
}
// 3Dオーディオ再生 curveDistanceScaler(減衰範囲)
void AudioSource::Play3D(bool loop, float curveDistanceScaler, float volume)
{
    // エミッターの初期設定
    PreferenceEmitter();

    // ソースボイスにデータを送信
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    if (buffer.pAudioData == nullptr || buffer.AudioBytes == 0) {
        assert("ソースボイスにデータに問題あり");
        return;
    }

    // 音量調整反映用
    volumeControl = volume;
    volume = volumeControl;

    // ループ制御
    isLooping = loop;
    loop = isLooping;
    
    // 3Dオーディオ更新処理
    Audio3dUpdate();

    // エミッター再生
    sourceVoice_->Stop();
    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->SubmitSourceBuffer(&buffer);

    HRESULT hr = sourceVoice_->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    sourceVoice_->SetVolume(volume);
}
// 停止
void AudioSource::Stop()
{
    if (!sourceVoice_)return;

    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->Stop();
}

void AudioSource::AudioRelease()
{
    if (sourceVoice_ != nullptr)
    {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
}

// エミッターの初期設定
void AudioSource::PreferenceEmitter()
{
    // エミッター設定
    emitter_->pCone = nullptr;
    emitter_->OrientFront = emitterFront;
    emitter_->OrientTop = emitterTop;
    emitter_->Position = emitterPosition;
    emitter_->Velocity = { 0.0f, 0.0f, 0.0f };

    //チャンネル数
    emitter_->ChannelCount = 1;
    emitter_->ChannelRadius = 0.0f;

    // 距離減衰カーブ
    emitter_->pVolumeCurve = nullptr;
    emitter_->pLFECurve = nullptr;
    emitter_->pLPFDirectCurve = nullptr;
    emitter_->pLPFReverbCurve = nullptr;
    emitter_->pReverbCurve = nullptr;

    emitter_->CurveDistanceScaler = curveDistanceScaler;
    emitter_->DopplerScaler = 1.0f;

    // 内部半径は使用しない
    emitter_->InnerRadius = 0.0f;
    emitter_->InnerRadiusAngle = 0.0f;

    //チャンネル方位テーブル
    float channelAzimuth = 0.0f;
    emitter_->pChannelAzimuths = &channelAzimuth;
}
// 3Dオーディオ更新処理
void AudioSource::Audio3dUpdate()
{
    const UINT32 SrcChannelCount = 2; // ステレオ音源
    const UINT32 DstChannelCount = 6; // 5.1ch スピーカー
    float pMatrixCoefficients[SrcChannelCount * DstChannelCount];

    dspSettings->pMatrixCoefficients = pMatrixCoefficients;
    dspSettings->SrcChannelCount = SrcChannelCount;
    dspSettings->DstChannelCount = DstChannelCount; // ステレオ出力

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    // 3Dオーディオパラメータの計算
    X3DAudioCalculate(*x3dHandle, listener_.get(), emitter_.get(),
        X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB, dspSettings.get());

    sourceVoice_->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), emitter_->ChannelCount, 2, dspSettings->pMatrixCoefficients);
    sourceVoice_->SetFrequencyRatio(dspSettings->DopplerFactor);
}


// オーディオ呼び出し関数
void AudioSource::SetAudioResourceId(int id)
{
    resource_ = Audio::Instance().GetAudioResource(static_cast<AUDIOID>(id));

    if (resource_ != nullptr)
    {
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&sourceVoice_, &resource_->GetWaveFormat());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}
// リスナー設定
void AudioSource::SetListener(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top)
{
    listener_->Position = position;
    listener_->OrientFront = front;
    listener_->OrientTop = top;
}
// エミッター設定
void AudioSource::SetEmitter(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top)
{
    DirectX::XMFLOAT3 eFront = {}, eTop = {};
    // 正規化
    DirectX::XMStoreFloat3(&eFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
    DirectX::XMStoreFloat3(&eTop, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&top)));

    emitterPosition = position;
    emitterFront = eFront;
    emitterTop = eTop;
    emitter_->ChannelCount = 1; // モノラル
}
// ボリューム設定 
void AudioSource::SetVolume(float volume)
{
    if (!sourceVoice_)return;

    volumeControl = volume;
    sourceVoice_->SetVolume(volume);
}