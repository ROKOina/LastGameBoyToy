#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../UenoCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/CPUParticle.h"

//��b
class UenoCharacterState_BaseState : public State<CharacterCom>
{
public:
    UenoCharacterState_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<UenoCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//�ҋ@
class UenoCharacterState_IdleState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_IdleState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//�ړ�
class UenoCharacterState_MoveState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_MoveState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
};

//�W�����v
class UenoCharacterState_JumpState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_JumpState(CharacterCom* owner) : UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    virtual const char* GetName() const override { return "Jump"; }
};

//�W�����v���[�v
class UenoCharacterState_JumpLoopState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_JumpLoopState(CharacterCom* owner) : UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    virtual const char* GetName() const override { return "JumpLoop"; }
};

//�V�t�g�X�L��(�W�����v�p�b�N)
class UenoCharacterState_ShiftSkillState :public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_ShiftSkillState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override;
    virtual const char* GetName() const override { return "ShiftSkill"; }

private:

    float a = 0.2f;
    float b = 5.0f;
    float rate = 0.3f;
    float startbust = 2.0f;
    float endbust = 25.0f;
    float influence = 3.0f;
};