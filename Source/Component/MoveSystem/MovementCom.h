#pragma once

#include "Component/System/Component.h"

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

    //�����_�������ɔ�΂�
    void ApplyRandomForce(float forcestrength, float yforce);

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
    void SetGravityEffect(float gravityeffect) { this->gravityeffect = gravityeffect; }

    //�����X�s�[�h
    const float& GetFallSpeed()const { return fallspeed; }
    void SetFallSpeed(float fallspeed) { this->fallspeed = fallspeed; }

    //�ڒn����
    const bool OnGround() { return onGround_; }
    void SetOnGround(bool flag) { onGround_ = flag; }

    // ���n�����u�Ԃ̃t���O���擾
    bool JustLanded() const { return justLanded_; }

    //���C
    const float& GetFriction()const { return friction_; }
    void SetFriction(float friction) { friction_ = friction; }

    //�ő呬�x
    const float& GetFisrtMoveMaxSpeed()const { return firstMoveMaxSpeed; }  //�����ő呬�x�i��j
    const float& GetMoveMaxSpeed()const { return moveMaxSpeed_; }
    void SetMoveMaxSpeed(float moveMaxSpeed) { moveMaxSpeed = moveMaxSpeed_; }
    void SetAddMoveMaxSpeed(float addSpeed) { moveMaxSpeed_ += addSpeed; }  //����
    void SetSubMoveMaxSpeed(float subSpeed) { moveMaxSpeed_ -= subSpeed; }  //����

    //�����x
    const float& GetMoveAcceleration()const { return moveAcceleration_; }
    void SetMoveAcceleration(float moveAcceleration) { moveAcceleration_ = moveAcceleration; }

    //���C�L���X�g�g�p
    const bool& GetIsRaycast()const { return isRaycast; }
    void SetIsRaycast(float isRaycast) { this->isRaycast = isRaycast; }

#pragma endregion

private:
    DirectX::XMFLOAT3 velocity_ = { 0,0,0 };   //����
    DirectX::XMFLOAT3 nonMaxSpeedVelocity_ = { 0,0,0 }; //�ő�X�s�[�h�𖳎���������
    float gravity_ = GRAVITY_NORMAL;           //�d��
    float gravityeffect = 30.0f;               //�d�͂̉e���x
    float fallspeed = -100.0f;                 //������X�s�[�h
    bool onGround_ = false;                    //�n�ʂɂ��Ă��邩
    bool wasOnGround_ = false;                 // �O�t���[���̒��n���
    bool justLanded_ = false;                  // ���t���[���Œ��n�����u�Ԃ��ǂ���
    float friction_ = 12.620f;                 //���C
    float moveMaxSpeed_ = 8.0f;                //�ő呬�x
    float firstMoveMaxSpeed = 8.0f;             //�����ő呬�x
    float moveAcceleration_ = 3.0f;            //�����x
    inline static float stepOffset = 0.5f;     //���C�L���X�g�p�̃I�t�Z�b�g
    bool isRaycast = true;                      //���C�L���X�g�����邩�itrue�F�g�p����j
};