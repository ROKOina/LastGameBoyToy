#pragma once
#include "Component\Stage\GateGimmickCom.h"

class GateGimmick_BaseState : public State<GateGimmick>
{
public:
    GateGimmick_BaseState(GateGimmick* owner);

protected:
    std::weak_ptr<GateGimmick> gateGimmick;
};

class GateGimmick_IdleState : public GateGimmick_BaseState
{
public:
    GateGimmick_IdleState(GateGimmick* owner) : GateGimmick_BaseState(owner) {}

    void Enter() override {};
    void Execute(const float& elapsedTime) override {};
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

class GateGimmick_UpState : public GateGimmick_BaseState
{
public:
    GateGimmick_UpState(GateGimmick* owner) : GateGimmick_BaseState(owner) {}

    void Enter() override {};
    void Execute(const float& elapsedTime) override;
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Up"; }
};

class GateGimmick_DownState : public GateGimmick_BaseState
{
public:
    GateGimmick_DownState(GateGimmick* owner) : GateGimmick_BaseState(owner) {}

    void Enter() override {};
    void Execute(const float& elapsedTime) override {};
    virtual void Exit() {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};