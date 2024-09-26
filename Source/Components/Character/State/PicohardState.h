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

//�n���}�[�U���
class Picohard_LeftClick : public Picohard_BaseState
{
public:
    Picohard_LeftClick(CharacterCom* owner) : Picohard_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "LeftClick"; }

private:
    //�U��񂵊p�x
    float rangeAngle = 100;
    float angle;
    //�U��񂵃X�s�[�h
    float speed = 500;
};

//���ē\��
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

//�`���[�W
class Picohard_LeftShift : public Picohard_BaseState
{
public:
    Picohard_LeftShift(CharacterCom* owner) : Picohard_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "LeftShift"; }
    void ImGui()override;
private:
    float timer;
    float time = 10;
    float moveSpeed = 5;
    float angleSpeed = 30;

    int hitID = -1; //���Ă�����ۑ�
};

