#pragma once
#include "../../System/State.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/Character/CharaStatusCom.h"
#include "Components\CPUParticle.h"

class BossCom;

//��b
class Boss_BaseState : public State<BossCom>
{
public:
    Boss_BaseState(BossCom* owner);

    //�A�j���[�V�������̓����蔻��
    bool AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname);

    //CPU�G�t�F�N�g�̌���
    void CPUEffect(const char* objectname, bool posflag);

    //GPU�G�t�F�N�g�̌����Ə���
    void GPUEffect(const char* objectname);

    //�����őI�����ꂽ�s����I������֐�
    void RandamBehavior(int one, int two);

protected:
    std::weak_ptr<BossCom> bossCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<CharaStatusCom>characterstatas;
    std::shared_ptr<GameObject>cachedobject;
    std::shared_ptr<GameObject>cpuparticle;
    std::shared_ptr<GameObject>gpuparticle;

private:
    // �A�j���[�V�����C�x���g���̓����蔻��
    DirectX::XMFLOAT3 nodepos = {};
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
    virtual void Exit();
    void ImGui() override;
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
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
};

//�W�����v
class Boss_JumpState : public Boss_BaseState
{
public:
    Boss_JumpState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Jump"; }
};

//�W�����v���[�v
class Boss_JumpLoopState : public Boss_BaseState
{
public:
    Boss_JumpLoopState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpLoop"; }
};

//���n
class Boss_LandingState : public Boss_BaseState
{
public:
    Boss_LandingState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual void Exit() {};
    virtual const char* GetName() const override { return "Landing"; }
};

//�p���`
class Boss_PunchState : public Boss_BaseState
{
public:
    Boss_PunchState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Punch"; }
};

//�L�b�N
class Boss_KickState : public Boss_BaseState
{
public:
    Boss_KickState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Kick"; }
};

//�͈͍U��
class Boss_RangeAttackState : public Boss_BaseState
{
public:
    Boss_RangeAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "RangeAttack"; }
};

//�{���v�U��
class Boss_BompAttackState : public Boss_BaseState
{
public:
    Boss_BompAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "BompAttack"; }
};

//�_���[�W
class Boss_DamageState : public Boss_BaseState
{
public:
    Boss_DamageState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Damage"; }
};

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