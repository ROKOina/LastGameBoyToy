#include "InazawaCharacterState.h"
#include "Input\Input.h"
#include "Component\Camera\CameraCom.h"
#include "BaseCharacterState.h"
#include "Math/Collision.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Audio\AudioCom.h"

InazawaCharacter_BaseState::InazawaCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(InazawaCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region Attack

void InazawaCharacter_AttackState::Enter()
{
    attackPower = 0;

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& charge = arm->GetChildFind("chargeEff");
        charge->GetComponent<GPUParticle>()->SetLoop(true);

        //音
        owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_CHARGE", false, 10);
    }
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{
    auto& moveCmp = owner->GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetSubMoveMaxSpeed(attackMaxMoveSpeed);

    //攻撃威力
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;

        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& charge = arm->GetChildFind("chargeEff");
            charge->GetComponent<GPUParticle>()->SetLoop(false);
            auto& chargeMax = arm->GetChildFind("chargeMaxEff");
            chargeMax->GetComponent<GPUParticle>()->SetLoop(true);
        }
    }

    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");

            auto& charge = arm->GetChildFind("chargeEff");
            charge->GetComponent<GPUParticle>()->SetLoop(false);
            auto& chargeMax = arm->GetChildFind("chargeMaxEff");
            chargeMax->GetComponent<GPUParticle>()->SetLoop(false);
        }

        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //弾減らさないとリロードしない
        charaCom.lock()->AddCurrentBulletNum(-1);

        //攻撃処理
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, attackPower / maxAttackPower, maxDamage * attackPower);

        //音
        owner->GetGameObject()->GetComponent<AudioCom>()->Stop("P_CHARGE");
        owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_SHOOT", false, 10);

        //射撃間隔タイマー起動
        owner->GetGameObject()->GetComponent<InazawaCharacterCom>()->ResetShootTimer();

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void InazawaCharacter_AttackState::Exit()
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");

        auto& charge = arm->GetChildFind("chargeEff");
        charge->GetComponent<GPUParticle>()->SetLoop(false);
        auto& chargeMax = arm->GetChildFind("chargeMaxEff");
        chargeMax->GetComponent<GPUParticle>()->SetLoop(false);
    }
}

void InazawaCharacter_AttackState::ImGui()
{
    ImGui::DragFloat("attackPower", &attackPower);
    ImGui::DragFloat("arrowSpeed", &arrowSpeed);
}

#pragma endregion

#pragma region ESkill

void InazawaCharacter_ESkillState::Enter()
{
    arrowCount = 8;
    skillTimer = skillTime;
    intervalTimer = 0.0f;

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& eff = arm->GetChildFind("eSkillEff");
        eff->GetComponent<GPUParticle>()->SetLoop(true);
    }
}

void InazawaCharacter_ESkillState::Execute(const float& elapsedTime)
{
    //タイマー
    skillTimer -= elapsedTime;
    //時間か矢数で終了
    if (skillTimer < 0 || arrowCount <= 0)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }

    intervalTimer += elapsedTime;
    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButton() && intervalTimer >= interval)
    {
        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //攻撃処理
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, 1, damage);
        arrowCount--;
        intervalTimer = 0;
        isShot = true;

        //音
        owner->GetGameObject()->GetComponent<AudioCom>()->Stop("P_SHOOT");
        owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_SHOOT", false, 10);
    }
}

void InazawaCharacter_ESkillState::Exit()
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& eff = arm->GetChildFind("eSkillEff");
        eff->GetComponent<GPUParticle>()->SetLoop(false);
    }
}

void InazawaCharacter_ESkillState::ImGui()
{
    ImGui::DragFloat("arrowSpeed", &arrowSpeed);
    ImGui::DragInt("arrowCount", &arrowCount);
    ImGui::DragFloat("interval", &interval);
    ImGui::DragFloat("skillTimerEnd", &skillTimer);
}

#pragma endregion