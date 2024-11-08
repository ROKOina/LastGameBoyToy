#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Particle\CPUParticle.h"

BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region Idle

void BaseCharacter_IdleState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //移動
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

#pragma endregion

#pragma region Move

void BaseCharacter_MoveState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    AnimationCom::PlayLowBodyAnimParam param =
    {
        param.lowerAnimaOneId = animationCom.lock()->FindAnimation("Walk_Forward"),
        param.lowerAnimeTwoId = animationCom.lock()->FindAnimation("Walk_Back"),
        param.lowerAnimeThreeId = animationCom.lock()->FindAnimation("Walk_Right"),
        param.lowerAnimeFourId = animationCom.lock()->FindAnimation("Walk_Left"),
        param.loop = true,
        param.rootFlag = false,
        param.blendType = 2,
        param.animeChangeRate = 0.5f,
        param.animeBlendRate = 0.0f
    };

    animationCom.lock()->PlayLowerBodyOnlyAnimation(param);
    //animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Single_Shot"), false, 0.3f);
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(true);


    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    //移動
    MoveInputVec(owner->GetGameObject());

    //待機
    if (!owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_MoveState::Exit()
{
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}

#pragma endregion

#pragma region Jump

void BaseCharacter_JumpState::Enter()
{
    //ジャンプ
    if (!moveCom.lock()->OnGround())
        return;

    JumpInput(owner->GetGameObject());
    moveVec = SceneManager::Instance().InputVec(owner->GetGameObject());
    moveCom.lock()->SetOnGround(false);

    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    //animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_Enter"), false);
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //空中制御
    DirectX::XMFLOAT3 inputVec = SceneManager::Instance().InputVec(owner->GetGameObject());
    moveVec = Mathf::Lerp(moveVec, inputVec, 0.1f);

    if (moveCom.lock()->GetVelocity().y < 0.05f && HoveringTimer < HoveringTime)
    {
        DirectX::XMFLOAT3 verocity = moveCom.lock()->GetVelocity();
        verocity.y = -GRAVITY_NORMAL * elapsedTime;
        moveCom.lock()->SetVelocity(verocity);

        HoveringTimer += elapsedTime;
    }

    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);

    if (moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
}

void BaseCharacter_JumpState::Exit()
{
    HoveringTimer = 0.0f;
}

#pragma endregion

#pragma region Hitscan

void BaseCharacter_HitscanState::Enter()
{
}

void BaseCharacter_HitscanState::Execute(const float& elapsedTime)
{
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        //視点の向きにレイを飛ばす
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
        {
            //有効に
            rayCol->SetEnabled(true);

            DirectX::XMFLOAT3 pos = ray->transform_->GetWorldPosition();

            //自分か判断する
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetID();
            if (playerNetID == charaCom.lock()->GetNetID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            rayCol->SetStart(pos);
            rayCol->SetEnd(pos + front * rayLength);
        }
    }

    if (!(CharacterInput::MainAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_HitscanState::Exit()
{
    //無効に
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
            rayCol->SetEnabled(false);
    }
}

void BaseCharacter_HitscanState::ImGui()
{
    ImGui::DragFloat("rayLength", &rayLength);
}

#pragma endregion

#pragma region Capsule

void BaseCharacter_CapsuleState::Enter()
{
}

void BaseCharacter_CapsuleState::Execute(const float& elapsedTime)
{
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
        {
            //有効に
            capsuleCol->SetEnabled(true);

            //自分か判断する
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetID();
            if (playerNetID == charaCom.lock()->GetNetID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            capsuleCol->SetPosition1(DirectX::XMFLOAT3(0, 0, 0));
            capsuleCol->SetPosition2(front * capsuleLength);
        }
    }

    //サブよりメインを優先
    if (CharacterInput::MainAttackButton & owner->GetButtonDown())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);

    if (!(CharacterInput::SubAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_CapsuleState::Exit()
{
    //無効に
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
            capsuleCol->SetEnabled(false);
    }
}

void BaseCharacter_CapsuleState::ImGui()
{
    ImGui::DragFloat("capsuleLength", &capsuleLength);
}

#pragma endregion

#pragma region StanBall

void BaseCharacter_StanBallState::Enter()
{
    BulletCreate::DamageFire(owner->GetGameObject(), speed, power);
    //BulletCreate::StanFire(owner->GetGameObject(), speed, power);
}

void BaseCharacter_StanBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_StanBallState::Exit()
{
}

void BaseCharacter_StanBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
}

#pragma endregion

#pragma region KnockbackBall

void BaseCharacter_KnockbackBallState::Enter()
{
    BulletCreate::KnockbackFire(owner->GetGameObject(), speed, power);
}

void BaseCharacter_KnockbackBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_KnockbackBallState::Exit()
{
}

void BaseCharacter_KnockbackBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
}

#pragma endregion

#pragma region ULT_ATTACK

void Ult_Attack_State::Enter()
{
    obj = owner->GetGameObject()->GetChildFind("UltAttackChild");
    if (!obj)return;

    //レイ設定
    auto& ray = obj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = obj->transform_->GetWorldPosition();

    DirectX::XMFLOAT3 front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}

void Ult_Attack_State::Execute(const float& elapsedTime)
{
    if (!obj)return;

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void Ult_Attack_State::Exit()
{
    auto& ray = obj->GetComponent<RayColliderCom>();
    ray->SetEnabled(false);

    obj.reset();
}

void Ult_Attack_State::ImGui()
{
}

#pragma endregion

void BaseCharacter_NoneAttack::Enter()
{
    ////歩きアニメーション再生開始
    //animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    //animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Idle"), true, 0.1f);
}