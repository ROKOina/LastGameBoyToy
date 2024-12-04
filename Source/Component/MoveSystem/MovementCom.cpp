#include "MovementCom.h"
#include <imgui.h>
#include "Math/Mathf.h"
#include "Component/System/TransformCom.h"
#include "Component/System/RayCastManager.h"
#include "Graphics/Graphics.h"
#include "Phsix\Physxlib.h"
#include <random>
#include "SystemStruct\TimeManager.h"

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
    ImGui::DragFloat((char*)u8"�㏸���x", &risespeed, 0.01f, 0.0f, 30.0f);
    ImGui::DragFloat((char*)u8"�ő�㏸���x", &maxrisespeed, 0.01f, 0.0f, 70.0f);
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
        PxRaycastBuffer buffer;
        if (isRaycast && PhysXLib::Instance().RayCast_PhysX(start, Mathf::Normalize(end - start), Mathf::Length(end - start), buffer, PhysXLib::CollisionLayer::Stage))
        {
            // �n�ʂɐڒn���Ă���
            position.y = buffer.block.position.y;
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

    // ���n�����u�Ԃ𔻒�
    justLanded_ = !wasOnGround_ && onGround_;

    // ���݂̏�Ԃ����̃t���[���p�ɋL�^
    wasOnGround_ = onGround_;

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
        DirectX::XMFLOAT3 end = {
            position.x + moveVec.x * elapsedTime,
            position.y + stepOffset,
            position.z + moveVec.z * elapsedTime
        };

        //�O��̌��ʂ�ێ�
        wasOnGround_ = onWall_;

        // SphereCast�ɂ��ǔ���
        PxRaycastBuffer buffer;

        static bool wasColliding = false;  // �O�t���[���ŕǂɏՓ˂��Ă�����
        if (isRaycast && PhysXLib::Instance().RayCast_PhysX(
            start, Mathf::Normalize(end - start), Mathf::Length(end - start) + advanceOffset, buffer, PhysXLib::CollisionLayer::Stage))
        {
            DirectX::XMFLOAT3 p = {};
            p.x = buffer.block.position.x;
            p.y = buffer.block.position.y;
            p.z = buffer.block.position.z;

            DirectX::XMFLOAT3 n = {};
            n = Mathf::Normalize(start - end);
            p = p + (n * advanceOffset);

            position.x = p.x;
            position.z = p.z;

            if (useWallSride_)
            {
                // �ǂɏՓ˂����ꍇ�A�ǂɉ������X���C�h�x�N�g�����v�Z
                DirectX::XMVECTOR Start = XMLoadFloat3(&start);
                DirectX::XMVECTOR End = XMLoadFloat3(&end);
                DirectX::XMVECTOR MoveVec = DirectX::XMVectorSubtract(End, Start);

                // �ǂ̖@�����擾
                DirectX::XMFLOAT3 normal = { buffer.block.normal.x, buffer.block.normal.y, buffer.block.normal.z };
                DirectX::XMVECTOR Normal = XMLoadFloat3(&normal);

                // �ǂɉ������X���C�h�x�N�g���̌v�Z
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(MoveVec, Normal);
                DirectX::XMVECTOR SlideVec = DirectX::XMVectorSubtract(MoveVec, DirectX::XMVectorMultiply(Normal, Dot));

                // �X���C�h�x�N�g���ƕ␳�x�N�g�������Z���ĕ␳�ʒu���v�Z
                DirectX::XMFLOAT3 correctedPosition;
                DirectX::XMStoreFloat3(&correctedPosition, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&p), SlideVec));

                position.x = correctedPosition.x;
                position.z = correctedPosition.z;
            }
            onWall_ = true;
        }
        else
        {
            // �ǂɏՓ˂��Ȃ��ꍇ�A�ʏ�̈ړ�
            position.x += moveVec.x * elapsedTime;
            position.z += moveVec.z * elapsedTime;
            wasColliding = false;  // �ǂɏՓ˂��Ă��Ȃ��̂ŁA�␳�����Z�b�g

            onWall_ = false;
        }
    }

    // ���n�����u�Ԃ𔻒�
    justHitWall_ = !wasOnWall_ && onWall_;

    GetGameObject()->transform_->SetWorldPosition(position);
}

// �͂�ǉ� (��ő呬�x�p)
void MovementCom::AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force)
{
    nonMaxSpeedVelocity_.x += force.x;
    nonMaxSpeedVelocity_.z += force.z;
}

//�㏸�֐�
void MovementCom::Rising(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;
    velocity_.y += risespeed * elapsedFrame;
    velocity_.y = (std::min)(velocity_.y, maxrisespeed);
}

// �͂�ǉ�
void MovementCom::AddForce(const DirectX::XMFLOAT3& force)
{
    velocity_.x += force.x * moveAcceleration_;
    velocity_.y += force.y * moveAcceleration_;
    velocity_.z += force.z * moveAcceleration_;
}

//�����_�������ɔ�΂�
void MovementCom::ApplyRandomForce(float forcestrength, float yforce)
{
    // �����_���� X, Z �����𐶐�
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // X, Z �Ń����_�������x�N�g�����쐬
    DirectX::XMVECTOR randomVec = DirectX::XMVectorSet(dist(gen), dist(gen), dist(gen), 0.0f);

    // �[���x�N�g�����ǂ������m�F
    if (DirectX::XMVector3Equal(randomVec, DirectX::XMVectorZero()))
    {
        // �[���x�N�g���̏ꍇ�̓f�t�H���g�̕�����ݒ�i�Ⴆ��Z�������j
        randomVec = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    }

    // �x�N�g���𐳋K��
    randomVec = DirectX::XMVector3Normalize(randomVec);

    // X, Z �����ɗ͂̑傫�����|����
    randomVec = DirectX::XMVectorScale(randomVec, forcestrength);

    // Y������ǉ�
    DirectX::XMFLOAT3 force;
    DirectX::XMStoreFloat3(&force, randomVec);
    force.y = yforce; // Y�����͒��ڐݒ�

    // AddForce ���Ăяo��
    velocity_.y += force.y;
    AddNonMaxSpeedForce(force);
}