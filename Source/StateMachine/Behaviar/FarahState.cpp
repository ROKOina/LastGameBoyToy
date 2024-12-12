#include "FarahState.h"
#include "Component\Bullet\BulletCom.h"

//îêNXÅ·
Farah_BaseState::Farah_BaseState(CharacterCom* owner) : State(owner)
{
    //úÝè
    charaCom = GetComp(FarahCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region CU
void Farah_MainAttackState::Enter()
{
}
void Farah_MainAttackState::Execute(const float& elapsedTime)
{
    //UI¹U
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //rAj[Vð·é
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.3f);
        }

        //ã¼gAj[V?
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);

        //U
        charaCom.lock()->AddBullet(BulletCreate::FarahDamageFire(owner->GetGameObject(), 40.0f));

        //ËÔu^C}[N®
        charaCom.lock()->ResetShootTimer();

        //Xe[gÏX
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
#pragma endregion

#pragma region ultU
void Farah_UltState::Enter()
{
    moveCom.lock()->SetMoveAcceleration(5.0f);
    charaCom.lock()->SetDashGaugeMins(2.0f);
}
#pragma endregion

#pragma region Eskill
void Farah_ESkillState::Enter()
{
}
void Farah_ESkillState::Execute(const float& elapsedTime)
{
    //UI¹U
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //U
    charaCom.lock()->AddBullet(BulletCreate::FarahKnockBack(owner->GetGameObject(), 30.0f, 2.0f));

    //Xe[gÏX
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion