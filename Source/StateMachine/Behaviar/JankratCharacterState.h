#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\JankratCharacterCom.h"
#include "BaseCharacterState.h"

//ジャンクラット
class JankratCharacter_BaseState : public State<CharacterCom>
{
public:
    JankratCharacter_BaseState(CharacterCom* owner);

    // 腕アニメーション処理
    void HandleArmAnimation() const;

    // 弾丸を発射する処理
    void FireBullet(const GameObj& bullet);

    // 銃の先端位置を取得
    bool GetGunTipPosition(DirectX::XMFLOAT3& outGunPos) const;

protected:
    float mass = 1.0f;
    float restitution = 0.0f;
    float force = 50.0f;

    float bulletLifeTimer = 3.0f;
    float addGravity = -0.8f;
    float fireVecY = 0.35f;
    float explosiontime = 1.5f;

protected:
    std::weak_ptr<JankratCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//通常弾
class JankratCharacter_MainAtkState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_MainAtkState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override;
    virtual const char* GetName() const override { return "MainAttack"; }
};

//地雷設置
class JankratCharacter_SubAttackState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_SubAttackState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override {};
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SubAttack"; }
};

//地雷起爆
class JankratCharacter_MainSkillState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_MainSkillState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override {};
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SubSkill"; }
};

//ult
class JankratCharacter_UltState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_UltState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "ult"; }
};