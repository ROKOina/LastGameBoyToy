#include "Misc.h"
#include "Audio/AudioSource.h"

AudioSource::AudioSource()
{
    listener_ = std::make_shared<X3DAUDIO_LISTENER>();
    emitter_ = std::make_shared<X3DAUDIO_EMITTER>();
    dspSettings = std::make_shared<X3DAUDIO_DSP_SETTINGS>();

    // コーンの初期化
    // cone_ = std::make_shared<X3DAUDIO_CONE>(X3DAudioDefault_DirectionalCone);

    //cone_->InnerAngle = X3DAUDIO_PI / 2;	// 内角: 90度
    //cone_->OuterAngle = X3DAUDIO_PI;		// 外角: 180度
    //cone_->InnerVolume = 1.0f;					// 内角での音量倍率
    //cone_->OuterVolume = 0.5f;					// 外角での音量倍率
    //cone_->InnerLPF = 0.0f;						// ローパスフィルター (内角)
    //cone_->OuterLPF = 0.75f;					    // ローパスフィルター (外角)
    //cone_->InnerReverb = 0.0f;					// リバーブ (内角)
    //cone_->OuterReverb = 0.5f;					// リバーブ (外角)
}

void AudioSource::Start()
{
}

void AudioSource::Update(float elapsedTime)
{
    // 3Dオーディオ更新
    Update3DAudio();
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
    if (ImGui::Button("EmitterPlay")) {
        EmitterPlay(isLooping, curveDistanceScaler, volumeControl);
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
        EmitterPlay(isLooping, curveDistanceScaler, volumeControl);
    }
}

// 3Dオーディオ更新処理
void AudioSource::Update3DAudio()
{
    float matrix[2]; // スピーカーチャンネルのマトリックス
    dspSettings->pMatrixCoefficients = matrix;
    dspSettings->SrcChannelCount = 1;
    dspSettings->DstChannelCount = 1; // ステレオ出力

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    // 3Dオーディオパラメータの計算
    X3DAudioCalculate(*x3dHandle, listener_.get(), emitter_.get(),
        X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB, dspSettings.get());

    sourceVoice_->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), emitter_->ChannelCount, 2, dspSettings->pMatrixCoefficients);
    sourceVoice_->SetFrequencyRatio(dspSettings->DopplerFactor);
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
    emitterFront     = eFront;
    emitterTop       = eTop;
    emitter_->ChannelCount = 1; // モノラル
}

// オーディオ呼び出し関数
void AudioSource::SetAudio(int id)
{
    resource_ = Audio::Instance().GetAudioResource(static_cast<AUDIOID>(id));

    if (resource_ != nullptr)
    {
        HRESULT hr = Audio::Instance().GetXAudio()->CreateSourceVoice(&sourceVoice_, &resource_->GetWaveFormat());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
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

// エミッター再生 curveDistanceScaler(減衰範囲)
void AudioSource::EmitterPlay(bool loop, float curveDistanceScaler, float volume)
{
    // エミッター設定
    //emitter_->pCone        = cone_.get();

    emitter_->pCone = NULL;
    emitter_->OrientFront = emitterFront;
    emitter_->OrientTop   = emitterTop;
    emitter_->Position      = emitterPosition;
    emitter_->Velocity      = { 0.0f, 0.0f, 0.0f };

    //チャンネル数
    emitter_->ChannelCount  = 1;
    emitter_->ChannelRadius = 0.0f;

    // 距離減衰カーブ
    emitter_->CurveDistanceScaler = curveDistanceScaler;
    emitter_->pVolumeCurve = NULL;
    emitter_->DopplerScaler  = 1.0f;

    // 内部半径は使用しない
    emitter_->InnerRadius = 0.0f;
    emitter_->InnerRadiusAngle = 0.0f;

    //チャンネル方位テーブル
    float channelAzimuth = 0.0f;
    emitter_->pChannelAzimuths = &channelAzimuth;

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
