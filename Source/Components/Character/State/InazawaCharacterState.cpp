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
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::FAKE_DEPTH), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////

    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 0.98f - 0.95f * power;
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();

    moveCom->SetNonMaxSpeedVelocity(fpsDir * arrowSpeed);
    moveCom->SetIsRaycast(false);

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
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

    //���C
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
}

void InazawaCharacter_AttackState::Execute(const float& elapsedTime)
{
    auto& moveCmp = owner->GetGameObject()->GetComponent<MovementCom>();
    moveCmp->SetSubMoveMaxSpeed(attackMaxMoveSpeed);

    //�U���З�
    attackPower += elapsedTime;
    if (attackPower > maxAttackPower) {
        attackPower = maxAttackPower;
    }

    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
        owner->GetGameObject()->GetComponent<AnimationCom>()->PlayAnimation(
            owner->GetGameObject()->GetComponent<AnimationCom>()->FindAnimation("Single_Shot"), false
        );

        //�U������
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, attackPower, 50);
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
        BulletCreate::DamageFire(owner->GetGameObject(), arrowSpeed, 1, 20);
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