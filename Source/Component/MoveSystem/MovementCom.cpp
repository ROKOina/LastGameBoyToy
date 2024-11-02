#include "MovementCom.h"
#include <imgui.h>
#include "Math/Mathf.h"
#include "Component/System/TransformCom.h"
#include "Component/System/RayCastManager.h"
#include "Graphics/Graphics.h"

// �X�V����
void MovementCom::Update(float elapsedTime)
{
    // �������ړ��X�V
    HorizonUpdate(elapsedTime);

    // ���E�̃X�s�[�h
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    // �I�u�W�F�N�g�̃X�s�[�h
    float objSpeed = GetGameObject()->GetObjSpeed();

    // �ړ���
    DirectX::XMFLOAT3 velocity;
    velocity.x = (velocity_.x + nonMaxSpeedVelocity_.x) * (worldSpeed * objSpeed);
    velocity.y = (velocity_.y + nonMaxSpeedVelocity_.y) * (worldSpeed * objSpeed);
    velocity.z = (velocity_.z + nonMaxSpeedVelocity_.z) * (worldSpeed * objSpeed);

    VelocityApplyPositionHorizontal(elapsedTime, velocity);

    // �c�����ړ��X�V
    VerticalUpdate(elapsedTime);

    VelocityApplyPositionVertical(elapsedTime, velocity.y);

    //�Ō�ɍő�X�s�[�h������
    moveMaxSpeed_ = firstMoveMaxSpeed;
}

// GUI�`��
void MovementCom::OnGUI()
{
    ImGui::DragFloat3((char*)u8"����", &velocity_.x, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat3((char*)u8"�ő�����x", &nonMaxSpeedVelocity_.x);
    ImGui::DragFloat((char*)u8"�d��", &gravity_, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat((char*)u8"�d�͉e���x", &gravityeffect, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"�����X�s�[�h", &fallspeed, 0.1f, -100.0f, 0.0f);
    ImGui::DragFloat((char*)u8"���C", &friction_, 0.01f, 0.0f, 40.0f);
    ImGui::DragFloat((char*)u8"�ő呬�x", &moveMaxSpeed_, 0.1f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"�����x", &moveAcceleration_, 0.01f, 0.0f, 10.0f);
    ImGui::Checkbox((char*)u8"�ڒn", &onGround_);
    ImGui::Checkbox((char*)u8"���C�L���X�g", &isRaycast);
}

// �c�����ړ��X�V
void MovementCom::VerticalUpdate(float elapsedTime)
{
    // �d�͂ɂ�鑬�͂̕ω�
    velocity_.y -= gravity_ * gravityeffect * elapsedTime;
    velocity_.y = (std::max)(velocity_.y, fallspeed);

    // �ő呬�x�̖������C�v�Z
    // ���C��
    float friction = friction_ * (elapsedTime * Graphics::Instance().GetWorldSpeed() * GetGameObject()->GetObjSpeed());

    nonMaxSpeedVelocity_.y -= friction;
    if (nonMaxSpeedVelocity_.y * nonMaxSpeedVelocity_.y < 0.1f)
        nonMaxSpeedVelocity_.y = 0;
}

// �������ړ��X�V
void MovementCom::HorizonUpdate(float elapsedTime)
{
    DirectX::XMFLOAT3 horizonVelocity = { velocity_.x, 0, velocity_.z };
    DirectX::XMVECTOR HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    float horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // �ő呬�x�𒴂��Ă���ꍇ�͐�������
    if (horiLengthSq > moveMaxSpeed_ * moveMaxSpeed_)
    {
        DirectX::XMVECTOR MaxSpeed = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), moveMaxSpeed_);
        DirectX::XMFLOAT3 newMaxVelocity;
        DirectX::XMStoreFloat3(&newMaxVelocity, MaxSpeed);
        velocity_.x = newMaxVelocity.x;
        velocity_.z = newMaxVelocity.z;
    }

    horizonVelocity = { velocity_.x, 0, velocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // ���C��
    float friction = friction_ * (elapsedTime * Graphics::Instance().GetWorldSpeed() * GetGameObject()->GetObjSpeed());

    // ���C�͓K�p
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(HorizonVelocity, -friction);
        DirectX::XMStoreFloat3(&velocity_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity_), FriVelocity));
    }
    else
    {
        velocity_.x = 0;
        velocity_.z = 0;
    }

    // �ő呬�x�̖������C�v�Z
    horizonVelocity = { nonMaxSpeedVelocity_.x, 0, nonMaxSpeedVelocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // ���C��
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(HorizonVelocity, -friction);
        DirectX::XMFLOAT3 newVelocity;
        DirectX::XMStoreFloat3(&newVelocity, FriVelocity);
        AddNonMaxSpeedForce(newVelocity);
    }
    else
    {
        nonMaxSpeedVelocity_.x = 0;
        nonMaxSpeedVelocity_.z = 0;
    }
}

// ���͂��X�V (��������)
void MovementCom::VelocityApplyPositionVertical(float elapsedTime, const float& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // �n�ʂƂ̒��n����
    {
        // ���C�L���X�g�p�̃I�t�Z�b�g
        TransformCom* transform = GetGameObject()->transform_.get();

        // ���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = transform->GetWorldPosition();
        start.y += stepOffset;

        // ���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = transform->GetWorldPosition();
        end.y += (moveVec * elapsedTime) - 0.1f;

        // ����
        RayCastManager::Result hit;
        if (isRaycast && RayCastManager::Instance().RayCast(start, end, hit))
        {
            // �n�ʂɐڒn���Ă���
            position.y = hit.position.y;
            velocity_.y = 0;
            nonMaxSpeedVelocity_.y = 0;
            onGround_ = true;
        }
        else
        {
            position.y += moveVec * elapsedTime;
            onGround_ = false;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

// ���͂��X�V (��������)
void MovementCom::VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // �������͗ʌv�Z
    DirectX::XMFLOAT3 totalVelocity = {
        velocity_.x + nonMaxSpeedVelocity_.x,
        velocity_.y,
        velocity_.z + nonMaxSpeedVelocity_.z
    };

    DirectX::XMFLOAT2 Speed = { totalVelocity.x, totalVelocity.z };
    float velocityLengthXZ = Mathf::Dot(Speed, Speed);

    if (velocityLengthXZ > 0.0f)
    {
        // ���C�̎n�_�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + totalVelocity.x * elapsedTime, position.y + stepOffset, position.z + totalVelocity.z * elapsedTime };

        // ���C�L���X�g�ɂ��ǔ���
        RayCastManager::Result hit;
        if (isRaycast && RayCastManager::Instance().RayCast(start, end, hit))
        {
            // �ړ���̈ʒu����ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = XMLoadFloat3(&start);
            DirectX::XMVECTOR End = XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���x�N�g���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

            // �␳�ʒu�̌v�Z
            DirectX::XMFLOAT3 correction;
            DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, Start));

            // �ǂƂ̐ڐG�ʒu����̕␳����
            start = hit.position;
            end = correction;

            // �␳��̈ʒu��ݒ�
            position = start;
        }
        else
        {
            position.x += totalVelocity.x * elapsedTime;
            position.z += totalVelocity.z * elapsedTime;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

// �͂�ǉ� (��ő呬�x�p)
void MovementCom::AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force)
{
    nonMaxSpeedVelocity_.x += force.x;
    nonMaxSpeedVelocity_.z += force.z;
}

// �͂�ǉ�
void MovementCom::AddForce(const DirectX::XMFLOAT3& force)
{
    velocity_.x += force.x * moveAcceleration_;
    velocity_.y += force.y * moveAcceleration_;
    velocity_.z += force.z * moveAcceleration_;
}