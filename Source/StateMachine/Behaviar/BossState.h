#pragma once
#include "Component/MoveSystem/MovementCom.h"
#include "StateMachine\State.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component/Character/CharaStatusCom.h"
#include "Component\Particle\CPUParticle.h"
#include <random>

class BossCom;

//��b
class Boss_BaseState : public State<BossCom>
{
public:

    //�r�b�g�Ő��䂷��
    enum EventFlags
    {
        None = 0,
        EnableGPUParticle = 1 << 0,  // �r�b�g 0: GPU�p�[�e�B�N����L����
        EnableCPUParticle = 1 << 1,  // �r�b�g 1: CPU�p�[�e�B�N����L����
        EnableCollision = 1 << 2,    // �r�b�g 2: �R���W������L����
        EnableSpawn = 1 << 3,        // �r�b�g 3: ������L����
    };

    Boss_BaseState(BossCom* owner);

    //�����̍s������
    void RandamBehavior();

    //�����v�Z
    int ComputeRandom();

    //�A�j���[�V�����C�x���g����
    void AnimtionEventControl(const std::string& eventname, const std::string& nodename, const char* objectname, int eventflags);

protected:
    std::weak_ptr<BossCom> bossCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<CharaStatusCom>characterstatas;

private:

    //����
    std::vector<int> availableNumbers = { };
    std::mt19937 gen;
};

//�ҋ@
class Boss_IdleState : public Boss_BaseState
{
public:
    Boss_IdleState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//�ҋ@�s��������
class Boss_IdleStopState : public Boss_BaseState
{
public:
    Boss_IdleStopState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "IdleStop"; }

private:
    float idletime = 0.0f;
};

//�ړ�
class Boss_MoveState : public Boss_BaseState
{
public:
    Boss_MoveState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
};

//�ߋ����U��1
class Boss_SA1 : public Boss_BaseState
{
public:
    Boss_SA1(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SA1"; }
};

//�ߋ����U��2
class Boss_SA2 : public Boss_BaseState
{
public:
    Boss_SA2(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SA2"; }
};

//�����A�b�g�J�n
class Boss_LARIATSTART : public Boss_BaseState
{
public:
    Boss_LARIATSTART(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "LARIATSTART"; }
};

//�����A�b�g���[�v
class Boss_LARIATLOOP : public Boss_BaseState
{
public:
    Boss_LARIATLOOP(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "LARIATLOOP"; }

private:
    float time = 0.0f;
};

//�����A�b�g�I��
class Boss_LARIATEND : public Boss_BaseState
{
public:
    Boss_LARIATEND(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "LARIATEND"; }
};

//�ł��グ�n��
class Boss_UpShotStart : public Boss_BaseState
{
public:
    Boss_UpShotStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotStart"; }
};

//�ł��グ�`���[�W
class Boss_UpShotCharge : public Boss_BaseState
{
public:
    Boss_UpShotCharge(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotCharge"; }

private:
    float time = 0.0f;
};

//�ł��グ���[�v
class Boss_UpShotLoop : public Boss_BaseState
{
public:
    Boss_UpShotLoop(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotLoop"; }

private:
    float time = 0.0f;
};

//�ł��グ�I���
class Boss_UpShotEnd : public Boss_BaseState
{
public:
    Boss_UpShotEnd(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotEnd"; }
};

//�ł��n��
class Boss_ShotStart : public Boss_BaseState
{
public:
    Boss_ShotStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShotStart"; }
};

//�`���[�W
class Boss_ShotCharge : public Boss_BaseState
{
public:
    Boss_ShotCharge(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShotCharge"; }

private:
    float time = 0.0f;
};

//�ł��܂�
class Boss_Shot : public Boss_BaseState
{
public:
    Boss_Shot(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Shot"; }
};

//�W�����v�U���n��
class Boss_JumpAttackStart : public Boss_BaseState
{
public:
    Boss_JumpAttackStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpAttackStart"; }
};

//�W�����v�U���I���
class Boss_JumpAttackEnd : public Boss_BaseState
{
public:
    Boss_JumpAttackEnd(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpAttackEnd"; }
};

////�{���v�U��
//class Boss_BompAttackState : public Boss_BaseState
//{
//public:
//    Boss_BompAttackState(BossCom* owner) :Boss_BaseState(owner) {}
//
//    void Enter() override;
//    void Execute(const float& elapsedTime) override;
//    void ImGui() override {};
//    virtual const char* GetName() const override { return "BompAttack"; }
//};
//
////�t�@�C���[�{�[��
//class Boss_FireBallState : public Boss_BaseState
//{
//public:
//    Boss_FireBallState(BossCom* owner) :Boss_BaseState(owner) {}
//
//    void Enter() override;
//    void Execute(const float& elapsedTime) override;
//    void ImGui() override {};
//    virtual const char* GetName() const override { return "FireBall"; }
//};

//���S
class Boss_DeathState : public Boss_BaseState
{
public:
    Boss_DeathState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Death"; }
};