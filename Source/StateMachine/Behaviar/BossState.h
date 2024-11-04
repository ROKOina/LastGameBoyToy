#pragma once
#include "Component/MoveSystem/MovementCom.h"
#include "StateMachine\State.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component/Character/CharaStatusCom.h"
#include "Component\Particle\CPUParticle.h"
#include <random>

class BossCom;

//基礎
class Boss_BaseState : public State<BossCom>
{
public:
    Boss_BaseState(BossCom* owner);

    //乱数の行動制御
    void RandamBehavior();

    //乱数計算
    int ComputeRandom();

    //アニメーションイベント制御
    void AnimtionEventControl(std::string eventname, std::string nodename, const char* objectname);

protected:
    std::weak_ptr<BossCom> bossCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<CharaStatusCom>characterstatas;

private:

    //乱数
    std::vector<int> availableNumbers = { };
    std::mt19937 gen;
};

//待機
class Boss_IdleState : public Boss_BaseState
{
public:
    Boss_IdleState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//待機行動長時間
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

//移動
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

//近距離攻撃1
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

//近距離攻撃2
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

//ラリアット開始
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

//ラリアットループ
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

//ラリアット終了
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

//ジャンプ
class Boss_JumpState : public Boss_BaseState
{
public:
    Boss_JumpState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Jump"; }
};

//パンチ
class Boss_PunchState : public Boss_BaseState
{
public:
    Boss_PunchState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Punch"; }
};

//キック
class Boss_KickState : public Boss_BaseState
{
public:
    Boss_KickState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Kick"; }
};

//範囲攻撃
class Boss_RangeAttackState : public Boss_BaseState
{
public:
    Boss_RangeAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "RangeAttack"; }
};

//ボンプ攻撃
class Boss_BompAttackState : public Boss_BaseState
{
public:
    Boss_BompAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "BompAttack"; }
};

//ファイヤーボール
class Boss_FireBallState : public Boss_BaseState
{
public:
    Boss_FireBallState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "FireBall"; }
};

//ミサイル攻撃
class Boss_MissileAttackState : public Boss_BaseState
{
public:
    Boss_MissileAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "MissileAttack"; }
};

//死亡
class Boss_DeathState : public Boss_BaseState
{
public:
    Boss_DeathState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Death"; }
};