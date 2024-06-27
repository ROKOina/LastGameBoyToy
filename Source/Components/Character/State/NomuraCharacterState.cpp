#include "NomuraCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"

#include "BaseCharacterState.h"

// É}ÉNÉç
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

void BuletFire(std::shared_ptr<GameObject> objPoint, float arrowSpeed = 40, float power = 1)
{
    //íeä€ÉIÉuÉWÉFÉNÉgÇê∂ê¨///////

    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADERMODE::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //íeî≠éÀ
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + arrowSpeed * power));

    //íe
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>();
    bulletCom->SetAliveTime(2.0f);
}

void Reloed()
{

}

NomuraCharacter_BaseState::NomuraCharacter_BaseState(CharacterCom* owner) :State(owner)
{
    //èâä˙ê›íË
    testCharaCom = GetComp(NomuraCharacterCom);
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

    //çUåÇà–óÕ
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //çUåÇèIóπèàóùÅïçUåÇèàóù
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //çUåÇèàóù
        BuletFire(owner->GetGameObject(), 30.0f, attackPower);

        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);

    }
}

void NomuraCharacter_AttackState::ImGui()
{
    //ImGui::DragFloat()
}

#pragma endregion

#pragma region ESkill

void NomuraCharacter_ESkillState::Enter()
{

}

void NomuraCharacter_ESkillState::Execute(const float& elapsedTime)
{
    //É^ÉCÉ}Å[
    skillTimer -= elapsedTime;
    ////éûä‘Ç©ñÓêîÇ≈èIóπ
    //if (skillTimer < 0 || arrowCount <= 0)
    //{
    //    ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    //}

    MoveInputVec(owner->GetGameObject());

    if (moveCom.lock()->OnGround())
        JumpInput(owner->GetGameObject());

    //intervalTimer += elapsedTime;
    //çUåÇèIóπèàóùÅïçUåÇèàóù
    //if (CharacterInput::MainAttackButton & owner->GetButton() && intervalTimer >= interval)
    //{
    //    //çUåÇèàóù
    //   /* Fire(owner->GetGameObject(), arrowSpeed);
    //    arrowCount--;
    //    intervalTimer = 0;*/
    //}
}

void NomuraCharacter_ESkillState::ImGui()
{

}

#pragma endregion