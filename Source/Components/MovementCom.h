#pragma once

#include "System\Component.h"

#define GRAVITY_NORMAL 0.98

//Movement�N���X
class MovementCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    MovementCom() {}
    ~MovementCom() {}

    // ���O�擾
    const char* GetName() const override { return "Movement"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    //velocity
    void AddForce(const DirectX::XMFLOAT3& force);

    //nonMaxSpeedVelocity
    void AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force);

private:

    //�c�����ړ��X�V
    void VerticalUpdate(float elapsedTime);

    //�������ړ��X�V
    void HorizonUpdate(float elapsedTime);

    //���͂��X�V( �������� )
    void VelocityApplyPositionVertical(float elapsedTime, const float& moveVec);

    //���͂��X�V( �������� )
    void VelocityApplyPositionHorizontal(float elapsedTime, const DirectX::XMFLOAT3& moveVec);

public:

#pragma �Q�b�^�[�ƃZ�b�^�[
    //velocity
    void ZeroVelocity() { velocity_ = { 0,0,0 }; }
    void ZeroVelocityY() { velocity_ = { velocity_.x,0,velocity_.z }; }
    const DirectX::XMFLOAT3& GetVelocity()const { return velocity_; }
    void SetVelocity(DirectX::XMFLOAT3 velo) { velocity_ = velo; }

    //addnonmaxvelocity
    void ZeroNonMaxSpeedVelocity() { nonMaxSpeedVelocity_ = { 0,0,0 }; }
    const DirectX::XMFLOAT3& GetNonMaxSpeedVelocity()const { return nonMaxSpeedVelocity_; }
    void SetNonMaxSpeedVelocity(DirectX::XMFLOAT3 velo) { nonMaxSpeedVelocity_ = velo; }

    //�d��
    const float& GetGravity()const { return gravity_; }
    void SetGravity(float gravity) { gravity_ = gravity; }

    //�d�͉e���x
    const float& GetGravityEffect()const { return gravityeffect; }
    void SetGravityEffect(float gravityeffect) { gravityeffect = gravityeffect; }

    //�ڒn����
    const bool OnGround() { return onGround_; }
    void SetOnGround(bool flag) { onGround_ = flag; }

    //���C
    const float& GetFriction()const { return friction_; }
    void SetFriction(float friction) { friction_ = friction; }

    //�ő呬�x
    const float& GetMoveMaxSpeed()const { return moveMaxSpeed_; }
    void SetMoveMaxSpeed(float moveMaxSpeed) { moveMaxSpeed_ = moveMaxSpeed; }

    //�����x
    const float& GetMoveAcceleration()const { return moveAcceleration_; }
    void SetMoveAcceleration(float moveAcceleration) { moveAcceleration_ = moveAcceleration; }
#pragma endregion

private:
    DirectX::XMFLOAT3 velocity_ = { 0,0,0 };   //����
    DirectX::XMFLOAT3 nonMaxSpeedVelocity_ = { 0,0,0 }; //�ő�X�s�[�h�𖳎���������
    float gravity_ = GRAVITY_NORMAL;           //�d��
    float gravityeffect = 1.0f;                //�d�͂̉e���x
    float fallspeed = -0.3;                    //������X�s�[�h
    bool onGround_ = false;                    //�n�ʂɂ��Ă��邩
    float friction_ = 12.620f;                 //���C
    float moveMaxSpeed_ = 5.0f;                //�ő呬�x
    float moveAcceleration_ = 1.0f;            //�����x
    inline static float stepOffset = 0.5f;     //���C�L���X�g�p�̃I�t�Z�b�g
};