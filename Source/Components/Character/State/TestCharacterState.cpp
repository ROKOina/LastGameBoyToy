#include "TestCharacterState.h"
#include "Input\Input.h"

#include "Components\CameraCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\BulletCom.h"

// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
  //�����ݒ�
  testCharaCom = GetComp(CharacterCom);
  moveCom = GetComp(MovementCom);
  transCom = GetComp(TransformCom);
  animationCom = GetComp(AnimationCom);
  renderCom = GetComp(RendererCom);
}

void TestCharacter_MoveState::Enter()
{
  //�����A�j���[�V�����Đ��J�n
  animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
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
    ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
  }

  //�W�����v
  if (CharacterInput::JumpButton_SPACE & owner->GetButtonDown())
  {
    ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
  }
}

void TestCharacter_AttackState::Enter()
{
    //�V���b�g�A�j���[�V�����Đ�
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Shot_Enter"), false, false, 0.8f);
}

void TestCharacter_AttackState::Execute(const float& elapsedTime)
{
  //���͒l�擾
  DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

  //����
  DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
  moveCom.lock()->AddForce(v);
  QuaternionStruct q = transCom.lock()->GetRotation();

  //�J�����������Ă�������֐���
  DirectX::XMFLOAT3 cameraForward = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>()->GetFront();
  transCom.lock()->Turn(cameraForward, 0.1f);

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


  if (CharacterInput::MainAttackButton & owner->GetButtonUp())
  {
    ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
  }
}

void TestCharacter_AttackState::Exit()
{
  fireTimer = 0.0f;
}

void TestCharacter_AttackState::Fire()
{
  //�e�ۃI�u�W�F�N�g�𐶐�///////

  GameObj bullet = GameObjectManager::Instance().Create();
  bullet->SetName("blackball");

  Model::Node* hand = renderCom.lock()->GetModel()->FindNode("Rb_Hand_R");
  DirectX::XMFLOAT4X4 fireTrans = hand->worldTransform;
  DirectX::XMFLOAT3 firePos = Mathf::TransformSamplePosition(fireTrans);//owner->GetGameObject()->transform_->GetWorldPosition();
  //firePos.y = 1.0f;
  bullet->transform_->SetWorldPosition(firePos);

  std::shared_ptr<RendererCom> bullet_renderCom = bullet->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
  bullet_renderCom->LoadModel("Data/Ball/t.mdl");

  std::shared_ptr<SphereColliderCom> sphereCollider = bullet->AddComponent<SphereColliderCom>();
  sphereCollider->SetPushBack(false);
  sphereCollider->SetMyTag(COLLIDER_TAG::PlayerAttack);
  sphereCollider->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyAttack);

  std::shared_ptr<MovementCom> bullet_moveCom = bullet->AddComponent<MovementCom>();
  std::shared_ptr<BulletCom> bullet_bulletCom = bullet->AddComponent<BulletCom>();

  ///////////////////////////////


  //�e����
  bullet_moveCom->SetGravity(0.0f);
  bullet_moveCom->SetFriction(0.0f);
  bullet_moveCom->AddNonMaxSpeedForce(owner->GetGameObject()->transform_->GetWorldFront() * 30.0f);

  //�e
  bullet_bulletCom->SetAliveTime(2.0f);
}


void TestCharacter_DashState::Enter()
{
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
    ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
  }
  if (GamePad::BTN_A & owner->GetButtonDown())
  {
    ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
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
