#include "UenoCharacterState.h"
#include "Input\Input.h"
#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "BaseCharacterState.h"
#include "Components/ColliderCom.h"

//���N���X�̃R���|�[�l���g
UenoCharacterState_BaseState::UenoCharacterState_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(UenoCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma �ҋ@
void UenoCharacterState_IdleState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void UenoCharacterState_IdleState::Execute(const float& elapsedTime)
{
    //�X�e�b�N���͂�����Έړ��X�e�[�g�ɑJ��
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }

    //�W�����v����
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //�V�t�g�X�L��
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma �ړ�
void UenoCharacterState_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    //4�����̕����A�j���[�V����
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
    //�ړ�
    MoveInputVec(owner->GetGameObject());

    //���͂��Ȃ�������ҋ@
    if (!owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //�W�����v����
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //�V�t�g�X�L��
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma �W�����v
void UenoCharacterState_JumpState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), false);

    //�W�����v�x�N�g��
    JumpInput(owner->GetGameObject());
}

void UenoCharacterState_JumpState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
        //ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP);
    }

    //�V�t�g�X�L��
    if (GamePad::BTN_LEFT_SHOULDER & owner->GetButtonDown())
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_SKILL);
    }
}
#pragma endregion

#pragma �W�����v���[�v
void UenoCharacterState_JumpLoopState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), true);
}

void UenoCharacterState_JumpLoopState::Execute(const float& elapsedTime)
{
    //�n�ʂɂ��Ă����
    if (moveCom.lock()->OnGround())
    {
        //���͂��Ȃ�������ҋ@
        if (!owner->IsPushLeftStick())
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
        }

        //�X�e�b�N���͂�����Έړ��X�e�[�g�ɑJ��
        if (owner->IsPushLeftStick())
        {
            ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
        }
    }
}
#pragma endregion

#pragma �V�t�g�X�L��(�W�����v�p�b�N)
void UenoCharacterState_ShiftSkillState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("jump"), false);

    //�W�����v�x�N�g��
    moveCom.lock()->SetFriction(5.0f);
    DirectX::XMFLOAT3 front = { GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront() };

    //x�Ƃ����ۑ�ł�
    float speedRate = 1 - powf(-powf(rate, a) + 1, b);
    float moveSpeed = Mathf::Lerp(startbust, endbust * influence, speedRate);
    moveCom.lock()->SetNonMaxSpeedVelocity(front * moveSpeed);
}

void UenoCharacterState_ShiftSkillState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
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