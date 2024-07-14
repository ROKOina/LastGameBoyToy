#include "TestCharacterState.h"
#include "Input\Input.h"

#include "Components\CameraCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\BulletCom.h"

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(TestCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
    renderCom = GetComp(RendererCom);
}

void TestCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true, false, 0.2f);

    //�_�b�V���p�̑��x�ݒ�
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele = 10.0f;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele = 1.0f;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}

void TestCharacter_MoveState::Execute(const float& elapsedTime)
{
    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //����
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    QuaternionStruct q = transCom.lock()->GetRotation();

    //���񏈗�
    transCom.lock()->Turn(moveVec, 0.1f);

    //�ҋ@
    if (moveVec == 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //�W�����v
    if (CharacterInput::JumpButton_SPACE & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void TestCharacter_AttackState::Enter()
{
    //�V���b�g�A�j���[�V�����Đ�
    animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Shot_Enter"), true, 0.8f);

    //�����蔻��L����
    std::shared_ptr<CapsuleColliderCom> collision = charaCom.lock()->GetGunFireCollision()->GetComponent<CapsuleColliderCom>();
    collision->SetEnabled(true);
}

void TestCharacter_AttackState::Execute(const float& elapsedTime)
{
    //�e�̔���̌����ݒ�
    std::shared_ptr<CapsuleColliderCom> collision = charaCom.lock()->GetGunFireCollision()->GetComponent<CapsuleColliderCom>();
    Model::Node* hand = renderCom.lock()->GetModel()->FindNode("Rb_Hand_R");
    DirectX::XMFLOAT4X4 fireTrans = hand->worldTransform;
    DirectX::XMFLOAT3 firePos = Mathf::TransformSamplePosition(fireTrans);
    collision->SetPosition1(firePos);
    collision->SetPosition2(owner->GetGameObject()->transform_->GetWorldFront() * firePower);

    //�e����
    if (fireTimer >= fireTime)
    {
        Fire();
        fireTimer = 0.0f;
    }
    else
    {
        fireTimer += elapsedTime;
    }

  //�{�^���𗣂�����U�����
  if (CharacterInput::MainAttackButton & owner->GetButtonUp())
  {
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
  }
}

void TestCharacter_AttackState::Exit()
{
    fireTimer = 0.0f;
}

void TestCharacter_AttackState::ImGui()
{
    ImGui::DragFloat(u8"���ˊԊu", &fireTime, 0.001f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"���ˈЗ�", &firePower, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"��C��R", &friction1, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"�n�ʖ��C", &friction2, 0.01f, 0.0f, 100.0f);

    ImGui::DragFloat(u8"���C���U�� �U����", &attackPower, 0.01f, 0.0f, 100.0f);
}

#include "Components/ProjectileCom.h"
void TestCharacter_AttackState::Fire()
{
    std::shared_ptr<CapsuleColliderCom> collision = charaCom.lock()->GetGunFireCollision()->GetComponent<CapsuleColliderCom>();
    
    for (HitObj& obj : collision->OnHitGameObject())
    {
        obj.gameObject.lock()->GetComponent<CharacterCom>()->AddHitPoint(-attackPower);
    }
}


void TestCharacter_DashState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Dash_Forward"), true);

    //�_�b�V���p�̑��x�ݒ�
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele += maxDashAcceleration;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele += dashAcceleration;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}

void TestCharacter_DashState::Execute(const float& elapsedTime)
{
    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //�ړ�
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);

    //���񏈗�
    transCom.lock()->Turn(moveVec, 0.1f);

    //�_�b�V�����
    if (owner->GetLeftStick().y <= 0.0f)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void TestCharacter_DashState::Exit()
{
    //�_�b�V���p�̑��x�ݒ�
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele -= maxDashAcceleration;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele -= dashAcceleration;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}
