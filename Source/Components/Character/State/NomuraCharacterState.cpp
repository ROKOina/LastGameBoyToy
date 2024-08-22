#include "NomuraCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "Components\ColliderCom.h"

#include "GameSource/Scene/SceneManager.h"
#include "BaseCharacterState.h"

void BuletFire(std::shared_ptr<GameObject> objPoint, float arrowSpeed = 40, float power = 1)
{
    //’eŠÛƒIƒuƒWƒFƒNƒg‚ð¶¬///////

    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    std::shared_ptr<SphereColliderCom> sphereCollider = obj->AddComponent<SphereColliderCom>();
    sphereCollider->SetPushBack(false);
    sphereCollider->SetMyTag(COLLIDER_TAG::PlayerAttack);
    sphereCollider->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyAttack);

    ///////////////////////////////


    //’e”­ŽË
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + arrowSpeed * power));

    //’e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);
}



NomuraCharacter_BaseState::NomuraCharacter_BaseState(CharacterCom* owner) :State(owner)
{
    //‰ŠúÝ’è
    charaCom = GetComp(NomuraCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region Attack

void NomuraCharacter_AttackState::Enter()
{

}

void NomuraCharacter_AttackState::Execute(const float& elapsedTime)
{
    

    MoveInputVec(owner->GetGameObject(), 0.5f);

    if (moveCom.lock()->OnGround())
        JumpInput(owner->GetGameObject());

    //UŒ‚ˆÐ—Í
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }
    
    //UŒ‚I—¹ˆ—•UŒ‚ˆ—
    if (CharacterInput::MainAttackButton & owner->GetButtonDown())
    {
        //UŒ‚ˆ—
        BuletFire(owner->GetGameObject(), 30.0f, attackPower);

        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);

    }
}

void NomuraCharacter_AttackState::ImGui()
{
  
}

#pragma endregion

#pragma region SubAttack

void NomuraCharacter_SubAttackState::Enter()
{

}

void NomuraCharacter_SubAttackState::Execute(const float& elapsedTime)
{


    MoveInputVec(owner->GetGameObject(), 0.5f);

    if (moveCom.lock()->OnGround())
        JumpInput(owner->GetGameObject());

    //UŒ‚ˆÐ—Í
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //’e”­ŽË
    if (fireTimer >= fireTime)
    {
        //UŒ‚ˆ—
        BuletFire(owner->GetGameObject(), 30.0f, attackPower);
        charaCom.lock()->MinusMagazin();
        fireTimer = 0.0f;
    }
    else
    {
        fireTimer += elapsedTime;
    }

    //UŒ‚I—¹ˆ—•UŒ‚ˆ—
    if (charaCom.lock()->GetNowMagazin()==0)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void NomuraCharacter_SubAttackState::ImGui()
{

}



#pragma endregion


#pragma region ESkill

void NomuraCharacter_ESkillState::Enter()
{

}

void NomuraCharacter_ESkillState::Execute(const float& elapsedTime)
{
    




}

void NomuraCharacter_ESkillState::ImGui()
{

}

#pragma endregion


#pragma region Reload

void NomuraCharacter_ReloadState::Enter()
{
    animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Shot_Enter"), false, 0.8f);
}

void NomuraCharacter_ReloadState::Execute(const float& elapsedTime)
{

    if (animationCom.lock()->IsPlayUpperAnimation())
    {
        DirectX::XMVECTOR VeloVec=DirectX::XMLoadFloat3(&moveCom.lock()->GetVelocity());

        charaCom.lock()->Reload();
       
        if (DirectX::XMVector3Length(VeloVec).m128_f32[0] > 0.1f)
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
            ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        }
        else if (DirectX::XMVector3Length(VeloVec).m128_f32[0] == 0.0f)
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
            ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        }
    }



}

void NomuraCharacter_ReloadState::ImGui()
{

}

#pragma endregion