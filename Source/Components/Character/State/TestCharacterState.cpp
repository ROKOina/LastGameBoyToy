#include "TestCharacterState.h"
#include "Input\Input.h"

#include "Components\CameraCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\BulletCom.h"

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(TestCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
    renderCom = GetComp(RendererCom);
}

void TestCharacter_MoveState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true, false, 0.2f);

    //ダッシュ用の速度設定
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele = 10.0f;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele = 1.0f;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}

void TestCharacter_MoveState::Execute(const float& elapsedTime)
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //歩く
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    QuaternionStruct q = transCom.lock()->GetRotation();

    //旋回処理
    transCom.lock()->Turn(moveVec, 0.1f);

    //待機
    if (moveVec == 0)
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //ジャンプ
    if (CharacterInput::JumpButton_SPACE & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void TestCharacter_AttackState::Enter()
{
    //ショットアニメーション再生
    animationCom.lock()->PlayUpperBodyOnlyAnimation(animationCom.lock()->FindAnimation("Shot_Enter"), true, 0.8f);
}

void TestCharacter_AttackState::Execute(const float& elapsedTime)
{
    //弾発射
    if (fireTimer >= fireTime)
    {
        Fire();
        fireTimer = 0.0f;
    }
    else
    {
        fireTimer += elapsedTime;
    }

  //ボタンを離したら攻撃やめ
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
    ImGui::DragFloat(u8"発射間隔", &fireTime, 0.001f, 0.0f, 100.0f);
    ImGui::DragFloat("friction1", &friction1, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat("friction2", &friction2, 0.01f, 0.0f, 100.0f);
}

#include "Components/ProjectileCom.h"
void TestCharacter_AttackState::Fire()
{
    //弾丸オブジェクトを生成///////

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

    bullet->transform_->SetScale({ 0.1f,0.1f,0.1f });

    ProjectileContext context;
    context.velocity = owner->GetGameObject()->transform_->GetWorldFront() * 30.0f;
    context.frictionAir = friction1;
    context.frictionGround = friction2;

    std::shared_ptr<ProjectileCom> bullet_moveCom = bullet->AddComponent<ProjectileCom>(context);
    std::shared_ptr<BulletCom> bullet_bulletCom = bullet->AddComponent<BulletCom>();

    ///////////////////////////////

    //弾
    bullet_bulletCom->SetAliveTime(20.0f);
}


void TestCharacter_DashState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Dash_Forward"), true);

    //ダッシュ用の速度設定
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele += maxDashAcceleration;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele += dashAcceleration;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}

void TestCharacter_DashState::Execute(const float& elapsedTime)
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //移動
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);

    //旋回処理
    transCom.lock()->Turn(moveVec, 0.1f);

    //ダッシュやめ
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
    //ダッシュ用の速度設定
    float maxDashAccele = moveCom.lock()->GetMoveMaxSpeed();
    maxDashAccele -= maxDashAcceleration;
    float dashAccele = moveCom.lock()->GetMoveAcceleration();
    dashAccele -= dashAcceleration;
    moveCom.lock()->SetMoveMaxSpeed(maxDashAccele);
    moveCom.lock()->SetMoveAcceleration(dashAccele);
}
