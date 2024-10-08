#pragma once

#include <DirectXMath.h>

#include "System\Component.h"

//�J����
class CameraCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    CameraCom(float fovY, float aspect, float nearZ, float farZ);
    ~CameraCom() {}

    // ���O�擾
    const char* GetName() const override { return "Camera"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    //Camera�N���X
public:
    //�w�����������
    void SetLookAt(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up = { 0,1,0 });

    //�p�[�X�y�N�e�B�u�ݒ�
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    //�r���[�s��擾
    const DirectX::XMFLOAT4X4& GetView() const { return view_; }

    //�v���W�F�N�V�����s��擾
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection_; }

    //�`��͈͎擾
    const DirectX::XMFLOAT2& GetScope()const { return scope_; }

    //�����_�擾
    const DirectX::XMFLOAT3& GetFocus()const { return focus_; }

    //������擾
    const DirectX::XMFLOAT3& GetUp()const { return up_; }

    //�O�����擾
    const DirectX::XMFLOAT3& GetFront()const { return front_; }
    void SetFrontX(const float& x) { this->front_.x = x; }

    //�E�����擾
    const DirectX::XMFLOAT3& GetRight()const { return right_; }

    void SetFocus(DirectX::XMFLOAT3 pos) { focus_ = pos;}

    //�J�����V�F�C�N���s
    void CameraShake(float power, float seconds) {
        shakePower_ = power;
        shakeSec_ = seconds;
    }

    //�A�N�e�B�u�J�����ŏ��̃Z�b�g
    void SetActiveInitialize();
    void ActiveCameraChange();
    bool GetIsActive() { return isActiveCamera; }

    //Ui�z�u�p��bool�擾
    bool GetIsUiCreate() { return isUiCreate; }

private:
    //�A�N�e�B�u�J�����ύX����
    void ChangeActiveProcess();

protected:

    //���ݎg���J�����i�J������������΁A�����ON�ɂȂ�Ȃ��j
    bool isActiveCamera = false;
    bool isNextCamera = false;

    //�J�����p��������g�������̂ł����ŏ���
    DirectX::XMFLOAT3 focus_ = { 0,0,0 };
    DirectX::XMFLOAT3 eye_ = { 0,0,0 };

    //���[�v�̒l
    float focuslapelate = 0.0f;
    float eyelaperate = 0.0f;

    //���o�n
    //�J�����V�F�C�N
    float shakeSec_ = {};    //�b��
    float shakePower_ = {};  //����
    DirectX::XMFLOAT3 shakePos_ = {};

private:

    //���W�n
    DirectX::XMFLOAT4X4 view_ = {};
    DirectX::XMFLOAT4X4 projection_ = {};
    DirectX::XMFLOAT2 scope_ = { 0,0 };

    DirectX::XMFLOAT3 up_ = { 0,1,0 };
    DirectX::XMFLOAT3 front_ = { 0,0,1 };
    DirectX::XMFLOAT3 right_ = { 1,0,0 };

    bool isLookAt_ = false;

    //Ui��z�u���鎞�Ƀ}�E�X���Œ肳��Ȃ��悤�ɂ���
    bool isUiCreate = false;
};