#pragma once

#include "StateMachine\State.h"
#include "Component\Stage\StageGimmickCom.h"
#include "Component\System\SpawnCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"

//�x�[�X�ƂȂ�X�e�[�g
class StageGimmick_BaseState : public State<StageGimmick>
{
public:
    StageGimmick_BaseState(StageGimmick* owner);

protected:
    std::weak_ptr<SpawnCom>spawn;
    std::weak_ptr<CharaStatusCom>status;
    std::weak_ptr<GPUParticle>gpuparticle;
};

//�ҋ@
class StageGimmick_IdleState : public StageGimmick_BaseState
{
public:
    StageGimmick_IdleState(StageGimmick* owner) :StageGimmick_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }

private:
    int spawnChangeNum = 0;
};

//�G�l�~�[����
class StageGimmick_EnemySpawnState : public StageGimmick_BaseState
{
public:
    StageGimmick_EnemySpawnState(StageGimmick* owner) :StageGimmick_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "EnemySpawn"; }

private:
    float time = 0.0f;
};

//�����U��
class StageGimmick_BigAttackState : public StageGimmick_BaseState
{
public:
    StageGimmick_BigAttackState(StageGimmick* owner) :StageGimmick_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "BigAttack"; }
};

//�ׂ��X�e�[�g
class StageGimmick_BreakState : public StageGimmick_BaseState
{
public:
    StageGimmick_BreakState(StageGimmick* owner) :StageGimmick_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Break"; }
};
