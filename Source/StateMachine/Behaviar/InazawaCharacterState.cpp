#include "InazawaCharacterState.h"
#include "Input\Input.h"
#include "Component\Camera\CameraCom.h"
#include "BaseCharacterState.h"
#include "Math/Collision.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Particle\CPUParticle.h"

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
    auto& chara = GetComp(CharacterCom);
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{
    auto& moveCmp = owner->GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetSubMoveMaxSpeed(attackMaxMoveSpeed);

    //çUåÇà–óÕ
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
        owner->GetGameObject()->GetComponent<AnimationCom>()->PlayAnimation(
            owner->GetGameObject()->GetComponent<AnimationCom>()->FindAnimation("Single_Shot"), false
        );

        //çUåÇèàóù
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, attackPower, 1);
        //Fire(owner->GetGameObject(), arrowSpeed, attackPower);
        //RayFire(owner->GetGameObject());

        auto& chara = GetComp(CharacterCom);

        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
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
    skillTimer = 5.0f;
    intervalTimer = 0.0f;
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

    //MoveInputVec(owner->GetGameObject());

    //if (moveCom.lock()->OnGround())
    //    JumpInput(owner->GetGameObject());

    intervalTimer += elapsedTime;
    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButton() && intervalTimer >= interval)
    {
        //çUåÇèàóù
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, 1, 1);
        //Fire(owner->GetGameObject(), arrowSpeed);
        arrowCount--;
        intervalTimer = 0;
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