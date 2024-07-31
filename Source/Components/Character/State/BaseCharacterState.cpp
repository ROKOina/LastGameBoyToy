#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "BaseCharacterState.h"

BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void BaseCharacter_IdleState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    AnimationCom::PlayLowBodyAnimParam param =
    {
        param.lowerAnimaOneId= animationCom.lock()->FindAnimation("Idle"),
        param.loop=true,
        param.animeChangeRate=0.1f
    };

    animationCom.lock()->PlayLowerBodyOnlyAnimation(param);
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //�ړ�
    if(owner->IsPushLeftStick())
    //if (moveVec != 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //�W�����v
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    MoveInputVec(owner->GetGameObject());

    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //�ҋ@
    if(!owner->IsPushLeftStick())
    //if (moveVec == 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
    //�W�����v
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_JumpState::Enter()
{
    //�W�����v
    if (!moveCom.lock()->OnGround())
        return;

    JumpInput(owner->GetGameObject());
    moveVec = SceneManager::Instance().InputVec();
    moveCom.lock()->SetOnGround(false);
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //�󒆐���
    DirectX::XMFLOAT3 inputVec = SceneManager::Instance().InputVec();
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

void BaseCharacter_NoneAttack::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Idle"), true, 0.1f);
}