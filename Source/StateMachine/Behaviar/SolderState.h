#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\SoldierCom.h"

//�\���W���[
class Solder_BaseState : public State<CharacterCom>
{
public:
    Solder_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<SoldierCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//���C���U��
class Solder_MainAttackState : public Solder_BaseState
{
public:
    Solder_MainAttackState(CharacterCom* owner) :Solder_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "MainAttack"; }

private:
    std::shared_ptr<GameObject>rayobj;
};

//E�X�L��
class Solder_ESkillState : public Solder_BaseState
{
public:
    Solder_ESkillState(CharacterCom* owner) :Solder_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "ESkill"; }
};

//ult�U��
class Solder_UltState : public Solder_BaseState
{
public:
    Solder_UltState(CharacterCom* owner) :Solder_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "UltAttack"; }
};