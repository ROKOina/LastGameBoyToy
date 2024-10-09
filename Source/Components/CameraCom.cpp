#include "CameraCom.h"

#include "TransformCom.h"
#include "Graphics/Graphics.h"
#include "GameSource/Math/Mathf.h"
#include "GameSource/Scene/SceneManager.h"
#include <imgui.h>
#include <cmath>

//�R���X�g���N�^�Œl����
CameraCom::CameraCom(float fovY, float aspect, float nearZ, float farZ)
{
    //�p�[�X�y�N�e�B�u�ݒ�
    SetPerspectiveFov(DirectX::XMConvertToRadians(fovY), aspect, nearZ, farZ);
}

// �J�n����
void CameraCom::Start()
{
}

// �X�V����
void CameraCom::Update(float elapsedTime)
{
    //�J�����V�F�C�N
    if (shakeSec_ > 0)
    {
        shakeSec_ -= elapsedTime;

        DirectX::XMFLOAT3 upDir = GetGameObject()->transform_->GetWorldUp();
        DirectX::XMFLOAT3 rightDir = GetGameObject()->transform_->GetWorldRight();

        float random = Mathf::RandomRange(-1, 1) * shakePower_;
        upDir = { upDir.x * random,upDir.y * random,upDir.z * random };
        random = Mathf::RandomRange(-1, 1) * shakePower_;
        rightDir = { rightDir.x * random,rightDir.y * random,rightDir.z * random };

        shakePos_ = { upDir.x + rightDir.x,upDir.y + rightDir.y,upDir.z + rightDir.z };
    }
    else
    {
        shakePos_ = { 0,0,0 };
    }

    //LookAt�֐��g���Ă��Ȃ��Ȃ�X�V����
    if (!isLookAt_)
    {
        //�J�����̃t�H���[�h���t�H�[�J�X����
        DirectX::XMFLOAT3 wPos = GetGameObject()->transform_->GetWorldPosition();

        //�O�����̃x�N�g�����擾
        DirectX::XMFLOAT3 forwardNormalVec = GetGameObject()->transform_->GetWorldFront();

        //eye����`�⊮���Ċ��炩�ɐ��䂷��
        DirectX::XMStoreFloat3(&wPos, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&eye_), DirectX::XMLoadFloat3(&wPos), 1.0f - eyelaperate));

        //focus����`�⊮���Ċ��炩�ɐ��䂷��
        DirectX::XMFLOAT3 forwardPoint = { forwardNormalVec * 2 + wPos + shakePos_ };
        DirectX::XMStoreFloat3(&focus_, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&forwardPoint), DirectX::XMLoadFloat3(&focus_), 1.0f - focuslapelate));

        SetLookAt(focus_, GetGameObject()->transform_->GetWorldUp());
    }

    isLookAt_ = false;

    //�A�N�e�B�u�J�����ύX����
    ChangeActiveProcess();
}

// GUI�`��
void CameraCom::OnGUI()
{
    ImGui::DragFloat3("Focus", &focus_.x);
    ImGui::DragFloat3("Eye", &eye_.x);
    ImGui::DragFloat("focuslapelate", &focuslapelate, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("eyelaperate", &eyelaperate, 0.01f, 0.0f, 1.0f);

    ImGui::DragFloat3("shakePos_", &shakePos_.x);
    ImGui::DragFloat("shakeSec", &shakeSec_);
    ImGui::DragFloat("shakePower", &shakePower_);

    bool active = isActiveCamera;
    ImGui::Checkbox("isActive", &active);
    ImGui::Checkbox("isUiCreate", &isUiCreate);
    if (ImGui::Button("Active"))
    {
        if (!isActiveCamera)
            isNextCamera = true;
    }

}

//�w�����������
void CameraCom::SetLookAt(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    //���_�A�����_�A���������r���[�s����쐬
    DirectX::XMFLOAT3 cameraPos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&cameraPos);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

    //�������W�̏ꍇ�A�������炷
    if (focus.x == cameraPos.x && focus.y == cameraPos.y && focus.z == cameraPos.z)
    {
        cameraPos.y += 0.0001f;
        Eye = DirectX::XMLoadFloat3(&cameraPos);
    }

    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&view_, View);

    //�r���[���t�s�񉻂��A���[���h���W�ɖ߂�
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    if (!std::isfinite(world._11))return;

    //�J�����̕��������o��
    this->right_.x = world._11;
    this->right_.y = world._12;
    this->right_.z = world._13;

    this->up_.x = world._21;
    this->up_.y = world._22;
    this->up_.z = world._23;

    this->front_.x = world._31;
    this->front_.y = world._32;
    this->front_.z = world._33;

    GetGameObject()->transform_->SetWorldTransform(world);

    //���_�A�����_��ۑ�
    this->focus_ = focus;
    this->eye_ = cameraPos;

    isLookAt_ = true;
}

//�p�[�X�y�N�e�B�u�ݒ�
void CameraCom::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    //��p�A��ʔ䗦�A�N���b�v��������v���W�F�N�V�����s����쐬
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);	//�v���W�F�N�V�����s��쐬
    DirectX::XMStoreFloat4x4(&projection_, Projection);	//rc�ɓn��
    scope_ = DirectX::XMFLOAT2(nearZ, farZ);
}

void CameraCom::SetActiveInitialize()
{
    isActiveCamera = true;
}

void CameraCom::ActiveCameraChange()
{
    isNextCamera = true;
}

//�A�N�e�B�u�J�����ύX����
void CameraCom::ChangeActiveProcess()
{
    //�ύX����
    if (SceneManager::Instance().GetCameraChange() &&
        (isActiveCamera || isNextCamera))
    {
        isActiveCamera = false;
        if (isNextCamera)
        {
            isActiveCamera = true;
            isNextCamera = false;
        }
    }

    if (isActiveCamera)
        SceneManager::Instance().SetActiveCamera(GetGameObject());

    if (isActiveCamera || isNextCamera)SceneManager::Instance().AddCameraActiveCount();
}