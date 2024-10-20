#pragma once
#include "../../System/State.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/Character/CharaStatusCom.h"
#include "Components\CPUParticle.h"

class BossCom;

//基礎
class Boss_BaseState : public State<BossCom>
{
public:
    Boss_BaseState(BossCom* owner);

    //アニメーション中の当たり判定
    bool AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname);

    //CPUエフェクトの検索
    void CPUEffect(const char* objectname, bool posflag);

    //GPUエフェクトの検索と消去
    void GPUEffect(const char* objectname);

    //乱数で選択された行動を選択する関数
    void RandamBehavior(int one, int two);

protected:
    std::weak_ptr<BossCom> bossCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<CharaStatusCom>characterstatas;
    std::shared_ptr<GameObject>cachedobject;
    std::shared_ptr<GameObject>cpuparticle;
    std::shared_ptr<GameObject>gpuparticle;

private:
    // アニメーションイベント時の当たり判定
    DirectX::XMFLOAT3 nodepos = {};
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
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
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

//ジャンプループ
class Boss_JumpLoopState : public Boss_BaseState
{
public:
    Boss_JumpLoopState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpLoop"; }
};

//着地
class Boss_LandingState : public Boss_BaseState
{
public:
    Boss_LandingState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual void Exit() {};
    virtual const char* GetName() const override { return "Landing"; }
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

//ダメージ
class Boss_DamageState : public Boss_BaseState
{
public:
    Boss_DamageState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Damage"; }
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