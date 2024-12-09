#pragma once

#include "Component/System/Component.h"
#include "Math/Mathf.h"

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

    //y������velocity
    void AddForceY(const float& forceY);

    //�����_�������ɔ�΂�
    void ApplyRandomForce(float forcestrength, float yforce);

    //nonMaxSpeedVelocity
    void AddNonMaxSpeedForce(const DirectX::XMFLOAT3& force);

    //�㏸�֐�
    void Rising(float elapsedTime);

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
    void AddNonMaxSpeedVelocity(DirectX::XMFLOAT3 velo) { nonMaxSpeedVelocity_ += velo; }

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

    //�ǂ̔���n
    bool GetOnWall() { return onWall_; }
    bool GetWasOnWall() { return wasOnWall_; }
    bool GetJustHitWall() { return justHitWall_; }
    void SetUseWallSride(bool flag) { useWallSride_ = flag; }

    //���C
    const float& GetFriction()const { return friction_; }
    void SetFriction(float friction) { friction_ = friction; airForce = friction; }

    const float& GetAirForce()const { return airForce; }
    void SetAirForce(float force) { airForce = force; }

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

    //���C�L���X�g�Ɏg�p����I�t�Z�b�g
    void SetStepOffset(float num) { stepOffset = num; }
    void SetAdvanceOffset(float num) { advanceOffset = num; }

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
    bool onWall_ = false;                      //�ǂɂ��Ă邩
    bool wasOnWall_ = false;                    //�O�t���[���̕ǂ����
    bool justHitWall_ = false;                  //���t���[���ŕǂɓ����������ǂ���
    bool useWallSride_ = true;                 //�ǎC��g�p�t���O
    float friction_ = 12.620f;                 //���C
    float airForce = 12.620f;                  //��C��R
    float moveMaxSpeed_ = 9.5f;                //�ő呬�x
    float firstMoveMaxSpeed = 8.0f;            //�����ő呬�x
    float moveAcceleration_ = 3.0f;            //�����x
    float stepOffset = 0.5f;                   //���C�L���X�g�p(������)�̃I�t�Z�b�g
    float advanceOffset = 1.0f;                //���C�L���X�g�p(������)�̃I�t�Z�b�g
    bool isRaycast = true;                     //���C�L���X�g�����邩�itrue�F�g�p����j
    float risespeed = 1.5f;                    //�㏸���x
    float maxrisespeed = 8.0f;                 //�ő�㏸���x
};