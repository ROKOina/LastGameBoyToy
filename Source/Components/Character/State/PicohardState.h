#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

class Picohard_BaseState : public State<CharacterCom> {
public:
    Picohard_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<CharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//ƒnƒ“ƒ}[U‚è‰ñ‚µ
class Picohard_LeftClick : public Picohard_BaseState
{
public:
    Picohard_LeftClick(CharacterCom* owner) : Picohard_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "LeftClick"; }

private:
    //U‚è‰ñ‚µŠp“x
    float rangeAngle = 100;
    float angle;
    //U‚è‰ñ‚µƒXƒs[ƒh
    float speed = 500;
};

//‚½‚Ä“\‚è
class Picohard_RightClick : public Picohard_BaseState
{
public:
    Picohard_RightClick(CharacterCom* owner) : Picohard_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "LeftClick"; }

private:
};
