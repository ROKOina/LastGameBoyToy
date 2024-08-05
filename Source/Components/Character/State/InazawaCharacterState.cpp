#include "InazawaCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\BulletCom.h"

#include "BaseCharacterState.h"

#include "GameSource/Math/Collision.h"

void Fire(std::shared_ptr<GameObject> objPoint, float arrowSpeed = 40, float power = 1)
{
    //íeä€ÉIÉuÉWÉFÉNÉgÇê∂ê¨///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //íeî≠éÀ
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + arrowSpeed * power));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //íe
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>();
    bulletCom->SetAliveTime(2.0f);
}

void RayFire(std::shared_ptr<GameObject> objPoint)
{
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;

    auto& rayPoint = objPoint->GetChildFind("rayObj");

    if (!rayPoint)return;

    start = rayPoint->transform_->GetWorldPosition();
    end = start + (objPoint->transform_->GetWorldFront() * 100);

    //ÉåÉCVsÉXÉtÉBÉA
    DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&Mathf::Normalize(end - start));
    auto& sphere = GameObjectManager::Instance().Find("robo");
    DirectX::XMVECTOR Sph = DirectX::XMLoadFloat3(&sphere->transform_->GetWorldPosition());
    HitResult h;
    if (Collision::IntersectRayVsSphere(Start, Dir, 100, Sph, 1.0f, h))
    {
        int i = 0;
    }

    //ÉåÉC

    rayPoint->GetComponent<RayColliderCom>()->SetStart(start);
    rayPoint->GetComponent<RayColliderCom>()->SetEnd(end);
}

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
    chara->SetMoveMaxSpeed(attackMaxMoveSpeed);
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{
    //MoveInputVec(owner->GetGameObject(), 0.5f);

    //if (moveCom.lock()->OnGround())
    //    JumpInput(owner->GetGameObject());

    //çUåÇà–óÕ
    attackPower+=elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
        owner->GetGameObject()->GetComponent<AnimationCom>()->PlayAnimation(
            owner->GetGameObject()->GetComponent<AnimationCom>()->FindAnimation("Single_Shot"),false
        );


        //çUåÇèàóù
        Fire(owner->GetGameObject(), arrowSpeed, attackPower);
        //RayFire(owner->GetGameObject());

        auto& chara = GetComp(CharacterCom);
        chara->SetMoveMaxSpeed(saveMaxSpeed);

        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }

    //auto& rayPoint = owner->GetGameObject()->GetChildFind("rayObj");
    //for (auto& hit : rayPoint->GetComponent<Collider>()->OnHitGameObject())
    //{
    //    ataPos = hit.hitPos;
    //}
    //Graphics::Instance().GetDebugRenderer()->DrawSphere(ataPos, 0.3f, { 0,1,1,1 });
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
        Fire(owner->GetGameObject(), arrowSpeed);
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
