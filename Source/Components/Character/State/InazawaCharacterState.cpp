#include "InazawaCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\BulletCom.h"

#include "BaseCharacterState.h"

void Fire(std::shared_ptr<GameObject> objPoint, float arrowSpeed = 40, float power = 1)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + arrowSpeed * power));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    coll->SetJudgeTag(COLLIDER_TAG::Enemy);

    //�e
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

    rayPoint->GetComponent<RayColliderCom>()->SetStart(start);
    rayPoint->GetComponent<RayColliderCom>()->SetEnd(end);
}

InazawaCharacter_BaseState::InazawaCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
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

    //�U���З�
    attackPower+=elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //�U������
        //Fire(owner->GetGameObject(), arrowSpeed, attackPower);
        RayFire(owner->GetGameObject());

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
    //�^�C�}�[
    skillTimer -= elapsedTime;
    //���Ԃ���ŏI��
    if (skillTimer < 0 || arrowCount <= 0)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }

    //MoveInputVec(owner->GetGameObject());

    //if (moveCom.lock()->OnGround())
    //    JumpInput(owner->GetGameObject());

    intervalTimer += elapsedTime;
    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButton() && intervalTimer >= interval)
    {
        //�U������
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
