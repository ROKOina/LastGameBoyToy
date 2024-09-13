#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../UenoCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/CPUParticle.h"

//��b
class FaraState_BaseState : public State<CharacterCom>
{
public:
    FaraState_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<UenoCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//�ҋ@
class FaraState_IdleState : public FaraState_BaseState
{
public:
    FaraState_IdleState(CharacterCom* owner) :FaraState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//�ړ�
class FaraState_MoveState : public FaraState_BaseState
{
public:
    FaraState_MoveState(CharacterCom* owner) :FaraState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
};

//�W�����v
class FaraState_JumpState : public FaraState_BaseState
{
public:
    FaraState_JumpState(CharacterCom* owner) : FaraState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    virtual const char* GetName() const override { return "Jump"; }
};

//�W�����v���[�v
class FaraState_JumpLoopState : public FaraState_BaseState
{
public:
    FaraState_JumpLoopState(CharacterCom* owner) : FaraState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    virtual const char* GetName() const override { return "JumpLoop"; }
};

//�U��(�C��)
//class UenoCharacterState_AttackState : public UenoCharacterState_BaseState
//{
//public:
//    UenoCharacterState_AttackState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}
//
//    void Enter() override;
//    void Execute(const float& elapsedTime) override;
//    void ImGui() override {};
//    virtual const char* GetName() const override { return "Attack"; }
//};

//�V�t�g�X�L��(�W�����v�p�b�N)
class FaraState_ShiftSkillState :public FaraState_BaseState
{
public:
    FaraState_ShiftSkillState(CharacterCom* owner) :FaraState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShiftSkill"; }
};