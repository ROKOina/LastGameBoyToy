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

    //ビットで制御する
    enum EventFlags
    {
        None = 0,
        EnableGPUParticle = 1 << 0,  // ビット 0: GPUパーティクルを有効化
        EnableCPUParticle = 1 << 1,  // ビット 1: CPUパーティクルを有効化
        EnableCollision = 1 << 2,    // ビット 2: コリジョンを有効化
        EnableSpawn = 1 << 3,        // ビット 3: 生成を有効化
    };

    Boss_BaseState(BossCom* owner);

    //乱数の行動制御
    void RandamBehavior();

    //乱数計算
    int ComputeRandom();

    //アニメーションイベント制御
    void AnimtionEventControl(const std::string& eventname, const std::string& nodename, const char* objectname, int eventflags);

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
    virtual void Exit() {};
    void ImGui() override {};
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

//打ち上げ始め
class Boss_UpShotStart : public Boss_BaseState
{
public:
    Boss_UpShotStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotStart"; }
};

//打ち上げチャージ
class Boss_UpShotCharge : public Boss_BaseState
{
public:
    Boss_UpShotCharge(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotCharge"; }

private:
    float time = 0.0f;
};

//打ち上げループ
class Boss_UpShotLoop : public Boss_BaseState
{
public:
    Boss_UpShotLoop(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotLoop"; }

private:
    float time = 0.0f;
};

//打ち上げ終わり
class Boss_UpShotEnd : public Boss_BaseState
{
public:
    Boss_UpShotEnd(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "UpShotEnd"; }
};

//打ち始め
class Boss_ShotStart : public Boss_BaseState
{
public:
    Boss_ShotStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShotStart"; }
};

//チャージ
class Boss_ShotCharge : public Boss_BaseState
{
public:
    Boss_ShotCharge(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "ShotCharge"; }

private:
    float time = 0.0f;
};

//打ちます
class Boss_Shot : public Boss_BaseState
{
public:
    Boss_Shot(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "Shot"; }
};

//ジャンプ攻撃始め
class Boss_JumpAttackStart : public Boss_BaseState
{
public:
    Boss_JumpAttackStart(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpAttackStart"; }
};

//ジャンプ攻撃終わり
class Boss_JumpAttackEnd : public Boss_BaseState
{
public:
    Boss_JumpAttackEnd(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit()override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpAttackEnd"; }
};

////ボンプ攻撃
//class Boss_BompAttackState : public Boss_BaseState
//{
//public:
//    Boss_BompAttackState(BossCom* owner) :Boss_BaseState(owner) {}
//
//    void Enter() override;
//    void Execute(const float& elapsedTime) override;
//    void ImGui() override {};
//    virtual const char* GetName() const override { return "BompAttack"; }
//};
//
////ファイヤーボール
//class Boss_FireBallState : public Boss_BaseState
//{
//public:
//    Boss_FireBallState(BossCom* owner) :Boss_BaseState(owner) {}
//
//    void Enter() override;
//    void Execute(const float& elapsedTime) override;
//    void ImGui() override {};
//    virtual const char* GetName() const override { return "FireBall"; }
//};

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