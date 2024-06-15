#include "TestCharacterState.h"
#include "Input\Input.h"

#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
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
}

void TestCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
}

void TestCharacter_MoveState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

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
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
    }
}

void TestCharacter_AttackState::Execute(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

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


    if (CharacterInput::MainAttackButton & gamePad.GetButtonUp())
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

    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");

    DirectX::XMFLOAT3 firePos = owner->GetGameObject()->transform_->GetWorldPosition();
    firePos.y = 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADERMODE::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(owner->GetGameObject()->transform_->GetWorldFront() * 30.0f);

    //�e
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>();
    bulletCom->SetAliveTime(2.0f);
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
