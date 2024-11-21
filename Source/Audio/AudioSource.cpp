#include "Misc.h"
#include "Audio/AudioSource.h"

AudioSource::AudioSource()
{
    listener_ = std::make_shared<X3DAUDIO_LISTENER>();
    emitter_ = std::make_shared<X3DAUDIO_EMITTER>();
    dspSettings = std::make_shared<X3DAUDIO_DSP_SETTINGS>();

    // �R�[���̏�����
    // cone_ = std::make_shared<X3DAUDIO_CONE>(X3DAudioDefault_DirectionalCone);

    //cone_->InnerAngle = X3DAUDIO_PI / 2;	// ���p: 90�x
    //cone_->OuterAngle = X3DAUDIO_PI;		// �O�p: 180�x
    //cone_->InnerVolume = 1.0f;					// ���p�ł̉��ʔ{��
    //cone_->OuterVolume = 0.5f;					// �O�p�ł̉��ʔ{��
    //cone_->InnerLPF = 0.0f;						// ���[�p�X�t�B���^�[ (���p)
    //cone_->OuterLPF = 0.75f;					    // ���[�p�X�t�B���^�[ (�O�p)
    //cone_->InnerReverb = 0.0f;					// ���o�[�u (���p)
    //cone_->OuterReverb = 0.5f;					// ���o�[�u (�O�p)
}

void AudioSource::Start()
{
}

void AudioSource::Update(float elapsedTime)
{
    // 3D�I�[�f�B�I�X�V
    Update3DAudio();
}

void AudioSource::OnGUI()
{
    ImGui::Text("Audio Name: %s", name);

    if (ImGui::TreeNode((char*)u8"���X�i�[")) {
        ImGui::DragFloat3("Position", &listener_->Position.x, 0.1f);
        ImGui::DragFloat3("Front", &listener_->OrientFront.x, 0.1f);
        ImGui::DragFloat3("Top", &listener_->OrientTop.x, 0.1f);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode((char*)u8"�G�~�b�^�[")) {
        ImGui::DragFloat3("Position", &emitter_->Position.x, 0.1f);
        ImGui::DragFloat3("Front", &emitter_->OrientFront.x, 0.1f);
        ImGui::DragFloat3("Top", &emitter_->OrientTop.x, 0.1f);

        ImGui::SliderFloat((char*)u8"�����͈�", &curveDistanceScaler, 0.0f, 10.0f);
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

    // �Đ����̂��̂������,�������f �`�F�b�N�O���Ă����[�v�����Ȃ�
    if (isLooping)
    {
        Play(isLooping, volumeControl);
        EmitterPlay(isLooping, curveDistanceScaler, volumeControl);
    }
}

// 3D�I�[�f�B�I�X�V����
void AudioSource::Update3DAudio()
{
    float matrix[2]; // �X�s�[�J�[�`�����l���̃}�g���b�N�X
    dspSettings->pMatrixCoefficients = matrix;
    dspSettings->SrcChannelCount = 1;
    dspSettings->DstChannelCount = 1; // �X�e���I�o��

    const X3DAUDIO_HANDLE* x3dHandle = Audio::Instance().GetX3DAudioHandle();

    // 3D�I�[�f�B�I�p�����[�^�̌v�Z
    X3DAudioCalculate(*x3dHandle, listener_.get(), emitter_.get(),
        X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
        | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
        | X3DAUDIO_CALCULATE_REVERB, dspSettings.get());

    sourceVoice_->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), emitter_->ChannelCount, 2, dspSettings->pMatrixCoefficients);
    sourceVoice_->SetFrequencyRatio(dspSettings->DopplerFactor);
}
// ���X�i�[�ݒ�
void AudioSource::SetListener(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top)
{
    listener_->Position = position;
    listener_->OrientFront = front;
    listener_->OrientTop = top;
}
// �G�~�b�^�[�ݒ�
void AudioSource::SetEmitter(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& top)
{
    DirectX::XMFLOAT3 eFront = {}, eTop = {};
    // ���K��
    DirectX::XMStoreFloat3(&eFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
    DirectX::XMStoreFloat3(&eTop, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&top)));

    emitterPosition = position;
    emitterFront     = eFront;
    emitterTop       = eTop;
    emitter_->ChannelCount = 1; // ���m����
}

// �I�[�f�B�I�Ăяo���֐�
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

// �G�~�b�^�[�Đ� curveDistanceScaler(�����͈�)
void AudioSource::EmitterPlay(bool loop, float curveDistanceScaler, float volume)
{
    // �G�~�b�^�[�ݒ�
    //emitter_->pCone        = cone_.get();

    emitter_->pCone = NULL;
    emitter_->OrientFront = emitterFront;
    emitter_->OrientTop   = emitterTop;
    emitter_->Position      = emitterPosition;
    emitter_->Velocity      = { 0.0f, 0.0f, 0.0f };

    //�`�����l����
    emitter_->ChannelCount  = 1;
    emitter_->ChannelRadius = 0.0f;

    // ���������J�[�u
    emitter_->CurveDistanceScaler = curveDistanceScaler;
    emitter_->pVolumeCurve = NULL;
    emitter_->DopplerScaler  = 1.0f;

    // �������a�͎g�p���Ȃ�
    emitter_->InnerRadius = 0.0f;
    emitter_->InnerRadiusAngle = 0.0f;

    //�`�����l�����ʃe�[�u��
    float channelAzimuth = 0.0f;
    emitter_->pChannelAzimuths = &channelAzimuth;

    // �\�[�X�{�C�X�Ƀf�[�^�𑗐M
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = resource_->GetAudioBytes();
    buffer.pAudioData = resource_->GetAudioData();
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    if (buffer.pAudioData == nullptr || buffer.AudioBytes == 0) {
        assert("�\�[�X�{�C�X�Ƀf�[�^�ɖ�肠��");
        return;
    }

    // ���ʒ������f�p
    volumeControl = volume;
    volume = volumeControl;

    // ���[�v����
    isLooping = loop;
    loop = isLooping;

    // �G�~�b�^�[�Đ�
    sourceVoice_->Stop();
    sourceVoice_->FlushSourceBuffers();
    sourceVoice_->SubmitSourceBuffer(&buffer);

    HRESULT hr = sourceVoice_->Start();
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    sourceVoice_->SetVolume(volume);
}

// ��~
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
