#include "UenoCharacterState.h"
#include "Input\Input.h"
#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "BaseCharacterState.h"
#include "Components/ColliderCom.h"

//基底クラスのコンポーネント
UenoCharacterState_BaseState::UenoCharacterState_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(UenoCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma 待機
void UenoCharacterState_IdleState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void UenoCharacterState_IdleState::Execute(const float& elapsedTime)
{
    //ステック入力があれば移動ステートに遷移
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }

    //ジャンプ入力
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //シフトスキル
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma 移動
void UenoCharacterState_MoveState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    //4方向の歩きアニメーション
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
    animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("shot"), true, 0.3f);
}

void UenoCharacterState_MoveState::Execute(const float& elapsedTime)
{
    //移動
    MoveInputVec(owner->GetGameObject());

    //入力がなかったら待機
    if (!owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //ジャンプ入力
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //シフトスキル
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma ジャンプ
void UenoCharacterState_JumpState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), false);

    //ジャンプベクトル
    JumpInput(owner->GetGameObject());
}

void UenoCharacterState_JumpState::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
        //ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP);
    }

    //シフトスキル
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma ジャンプループ
void UenoCharacterState_JumpLoopState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), true);
}

void UenoCharacterState_JumpLoopState::Execute(const float& elapsedTime)
{
    //地面についていれば
    if (moveCom.lock()->OnGround())
    {
        //入力がなかったら待機
        if (!owner->IsPushLeftStick())
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
        }

        //ステック入力があれば移動ステートに遷移
        if (owner->IsPushLeftStick())
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
        }
    }
}
#pragma endregion

#pragma シフトスキル(ジャンプパック)
void UenoCharacterState_ShiftSkillState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), false);

    //ジャンプベクトル
    moveCom.lock()->SetFriction(5.0f);
    DirectX::XMFLOAT3 front = { GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront() };

    //xとｚ軸課題です
    float speedRate = 1 - powf(-powf(rate, a) + 1, b);
    float moveSpeed = Mathf::Lerp(startbust, endbust * influence, speedRate);
    moveCom.lock()->SetNonMaxSpeedVelocity(front * moveSpeed);
}

void UenoCharacterState_ShiftSkillState::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP);
    }
}

void UenoCharacterState_ShiftSkillState::Exit()
{
    moveCom.lock()->SetFriction(12.620f);
}

//imgui
void UenoCharacterState_ShiftSkillState::ImGui()
{
    ImGui::DragFloat("a", &a);
    ImGui::DragFloat("b", &b);
    ImGui::DragFloat("rate", &rate);
    ImGui::DragFloat("startbust", &startbust);
    ImGui::DragFloat("endbust", &endbust);
    ImGui::DragFloat("influence", &influence);
}
#pragma endregion