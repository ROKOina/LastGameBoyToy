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

    //当たり判定有効化
    std::shared_ptr<CapsuleColliderCom> collision = charaCom.lock()->GetGunFireCollision()->GetComponent<CapsuleColliderCom>();
    collision->SetEnabled(true);
}

void TestCharacter_AttackState::Execute(const float& elapsedTime)
{
    //銃の判定の向き設定
    std::shared_ptr<CapsuleColliderCom> collision = charaCom.lock()->GetGunFireCollision()->GetComponent<CapsuleColliderCom>();
    Model::Node* hand = renderCom.lock()->GetModel()->FindNode("Rb_Hand_R");
    DirectX::XMFLOAT4X4 fireTrans = hand->worldTransform;
    DirectX::XMFLOAT3 firePos = Mathf::TransformSamplePosition(fireTrans);
    collision->SetPosition1(firePos);
    collision->SetPosition2(owner->GetGameObject()->transform_->GetWorldFront() * firePower);

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
    ImGui::DragFloat(u8"発射威力", &firePower, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"空気抵抗", &friction1, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"地面摩擦", &friction2, 0.01f, 0.0f, 100.0f);

    ImGui::DragFloat(u8"メイン攻撃 攻撃力", &attackPower, 0.01f, 0.0f, 100.0f);
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
