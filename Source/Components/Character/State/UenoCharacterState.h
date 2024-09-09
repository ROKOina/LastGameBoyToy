#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../UenoCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/CPUParticle.h"

//���̃^���N
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

//�U��(�C��)
class UenoCharacterState_AttackState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_AttackState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Attack"; }
};

//�V�t�g�X�L��(�W�����v�p�b�N)
class UenoCharacterState_ShiftSkillState :public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_ShiftSkillState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShiftSkill"; }
};