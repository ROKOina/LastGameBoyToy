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
    //èâä˙ê›íË
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

        //âπ
        owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_CHARGE", false, 10);
    }
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{
    auto& moveCmp = owner->GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetSubMoveMaxSpeed(attackMaxMoveSpeed);

    

    //çUåÇà–óÕ
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


    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.3f);

            auto& charge = arm->GetChildFind("chargeEff");
            charge->GetComponent<GPUParticle>()->SetLoop(false);
            auto& chargeMax = arm->GetChildFind("chargeMaxEff");
            chargeMax->GetComponent<GPUParticle>()->SetLoop(false);
        }

        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);

        //çUåÇèàóù
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, attackPower/ maxAttackPower, maxDamage * attackPower);

        //âπ
        owner->GetGameObject()->GetComponent<AudioCom>()->Stop("P_CHARGE");
        owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_SHOOT",false,10);

        //éÀåÇä‘äuÉ^ÉCÉ}Å[ãNìÆ
        owner->GetGameObject()->GetComponent<InazawaCharacterCom>()->ResetShootTimer();

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
    //É^ÉCÉ}Å[
    skillTimer -= elapsedTime;
    //éûä‘Ç©ñÓêîÇ≈èIóπ
    if (skillTimer < 0 || arrowCount <= 0)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }

    intervalTimer += elapsedTime;
    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButton() && intervalTimer >= interval)
    {
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.3f);
        }

        //çUåÇèàóù
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, 1, damage);
        //Fire(owner->GetGameObject(), arrowSpeed);
        arrowCount--;
        intervalTimer = 0;
        isShot = true;  

        //âπ
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