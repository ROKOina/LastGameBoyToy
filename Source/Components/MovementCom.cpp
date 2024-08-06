#include "MovementCom.h"

#include <imgui.h>
#include "GameSource/Math/Mathf.h"
#include "TransformCom.h"
#include "System/RayCastManager.h"
#include "Graphics\Graphics.h"

// �J�n����
void MovementCom::Start()
{
}

// �X�V����
void MovementCom::Update(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    //�c�����ړ��X�V
    VerticalUpdate(elapsedFrame);

    HorizonUpdate(elapsedTime);

    //���E�̃X�s�[�h
    float worldSpeed = Graphics::Instance().GetWorldSpeed();
    //�I�u�W�F�N�g�̃X�s�[�h
    float objSpeed = GetGameObject()->GetObjSpeed();

    // �ړ���
    DirectX::XMFLOAT3 velocity;
    velocity.x = (velocity_.x + nonMaxSpeedVelocity_.x) * (elapsedTime * worldSpeed * objSpeed);
    velocity.y = (velocity_.y + nonMaxSpeedVelocity_.y) * (elapsedTime * worldSpeed * objSpeed);
    velocity.z = (velocity_.z + nonMaxSpeedVelocity_.z) * (elapsedTime * worldSpeed * objSpeed);

    VelocityApplyPositionHorizontal(elapsedTime, velocity);
    VelocityApplyPositionVertical(elapsedTime, velocity.y);
};

// GUI�`��
void MovementCom::OnGUI()
{
    ImGui::DragFloat3("velocity", &velocity_.x);
    ImGui::DragFloat3("nonMaxSpeedVelocity_", &nonMaxSpeedVelocity_.x);
    ImGui::DragFloat("gravity", &gravity_, 0.1f);
    ImGui::DragFloat("gravityeffect", &gravityeffect, 0.1f);
    ImGui::DragFloat("friction", &friction_, 0.1f);
    ImGui::DragFloat("moveMaxSpeed", &moveMaxSpeed_, 0.1f);
    ImGui::DragFloat("moveAcceleration", &moveAcceleration_, 0.1f);
    ImGui::Checkbox("onGround", &onGround_);
}

//�c�����ړ��X�V
void MovementCom::VerticalUpdate(float elapsedTime)
{
    velocity_.y -= gravity_ * gravityeffect * elapsedTime;
    velocity_.y = (std::max)(velocity_.y, -15.0f);
}

void MovementCom::HorizonUpdate(float elapsedTime)
{
    DirectX::XMFLOAT3 horizonVelocity = { velocity_.x,0,velocity_.z };
    DirectX::XMVECTOR HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    float horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    //�ő呬�x�𒴂��Ă���ꍇ�͐�������
    if (horiLengthSq > moveMaxSpeed_ * moveMaxSpeed_)
    {
        DirectX::XMVECTOR MaxSpeed = DirectX::XMVectorScale(DirectX::XMVector3Normalize(HorizonVelocity), moveMaxSpeed_);
        DirectX::XMFLOAT3 newMaxVelocity;
        DirectX::XMStoreFloat3(&newMaxVelocity, MaxSpeed);
        velocity_.x = newMaxVelocity.x;
        velocity_.z = newMaxVelocity.z;
    }

    horizonVelocity = { velocity_.x,0,velocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    // ���C��
    float friction;
    {
        //���E�̃X�s�[�h
        float worldSpeed = Graphics::Instance().GetWorldSpeed();
        //�I�u�W�F�N�g�̃X�s�[�h
        float objSpeed = GetGameObject()->GetObjSpeed();
        friction = friction_ * (elapsedTime * worldSpeed * objSpeed);
    }

    // ���C��
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
        DirectX::XMStoreFloat3(&velocity_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity_), FriVelocity));
    }
    else
    {
        velocity_.x = 0;
        velocity_.z = 0;
    }

    //�ő呬�x�̖������C�v�Z
    horizonVelocity = { nonMaxSpeedVelocity_.x,0,nonMaxSpeedVelocity_.z };
    HorizonVelocity = DirectX::XMLoadFloat3(&horizonVelocity);
    horiLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(HorizonVelocity));

    //���C��
    if (horiLengthSq > 0.0f)
    {
        DirectX::XMVECTOR FriVelocity = DirectX::XMVectorScale(DirectX::XMVectorScale(HorizonVelocity, -1), friction);
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

//���͂��X�V
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
        end.y += moveVec;

        // ����
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit)) {
            // �n�ʂɐڒn���Ă���

#if 1 // �X�Η��������Ɗ��藎���鏈��

      // �X�Η���臒l�ȏ�̏ꍇ�A�@�������ɉ����o��
            if (hit.slopeRate >= slopeThreshold) {
                // ���E�̃X�s�[�h
                float worldSpeed = Graphics::Instance().GetWorldSpeed();

                // �X�Η��ɉ����Ċ��鑬�x��ω�������
                float slip = max(slipPower * hit.slopeRate, 1.0f);
                position += hit.normal * slip * elapsedTime * worldSpeed;

                // �����o�����ꏊ�����܂��Ă����ꍇ
                RayCastManager::Result hit2;
                if (RayCastManager::Instance().RayCast(start, end, hit2))
                {
                    position.y = hit2.position.y;
                }
            }
            else
                position.y = hit.position.y;

#else // ���藎�����肹���Ɉʒu�𔽉f�����鏈��
            position.y = hit.position.y;

#endif // 1

            velocity_.y = 0;
            nonMaxSpeedVelocity_.y = 0;
            onGround_ = true;
        }
        else {
            position.y += moveVec;
            onGround_ = false;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}

void MovementCom::VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec)
{
    DirectX::XMFLOAT3 position = GetGameObject()->transform_->GetWorldPosition();

    // �������͗ʌv�Z
    DirectX::XMFLOAT2 Speed = {
      moveVec.x,
      moveVec.z
    };
    float velocityLengthXZ = Mathf::Dot(Speed, Speed);

    if (velocityLengthXZ > 0.0f)
    {
        // ���C�̎n�_�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveVec.x, position.y + stepOffset, position.z + moveVec.z };

        // ���C�L���X�g�ɂ��ǔ���
        RayCastManager::Result hit;
        if (RayCastManager::Instance().RayCast(start, end, hit))
        {
            // �ړ���̈ʒu����ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = XMLoadFloat3(&start);
            DirectX::XMVECTOR End = XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);

            // �ǂ̖@��
            DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

            // ���˃x�N�g����@���x�N�g���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

            // �␳�ʒu�̌v�Z
            DirectX::XMFLOAT3 correction;
            DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, End));

            start = hit.position;
            end = correction;

            // �␳��̈ʒu���ǂɂ߂荞��ł�����
            RayCastManager::Result hit2;
            if (RayCastManager::Instance().RayCast(start, end, hit2))
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            else
            {
                position.x = correction.x;
                position.z = correction.z;
            }
        }
        else
        {
            position.x += moveVec.x;
            position.z += moveVec.z;
        }
    }

    GetGameObject()->transform_->SetWorldPosition(position);
}