#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"

//初期化
void FarahCom::Start()
{
    //ステート登録(移動関係)
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    //ステート登録(攻撃関係)
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    //初期ステート
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//更新処理
void FarahCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    //fps用の腕アニメーション
    FPSArmAnimation();
}

//gui
void FarahCom::OnGUI()
{
    CharacterCom::OnGUI();
    ImGui::DragFloat("frontvecpower", &frontvecpower);
}

//右クリック単発押し処理
void FarahCom::SubAttackDown()
{
    // ステート変更
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    // movecomp取得
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    // 前方向ベクトルを取得（左スティック入力による方向か、ワールド前方向か）
    DirectX::XMVECTOR frontVec = IsPushLeftStick()
        ? DirectX::XMLoadFloat3(&SceneManager::Instance().InputVec(GetGameObject()))
        : DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldFront());
    frontVec = DirectX::XMVector3Normalize(frontVec); // 正規化

    // ダッシュ力を計算
    DirectX::XMVECTOR dashForce = DirectX::XMVectorScale(frontVec, dashProgress * frontvecpower);
    DirectX::XMFLOAT3 dashForceFloat3;
    DirectX::XMStoreFloat3(&dashForceFloat3, dashForce);

    DirectX::XMFLOAT3 liftForce =
    {
        GetGameObject()->transform_->GetWorldPosition().x,
        dashProgress, // 上昇力を更新
        GetGameObject()->transform_->GetWorldPosition().z
    };

    // 力を移動コンポーネントに加える
    moveCom->AddForce(liftForce); // 空中の挙動
    moveCom->AddNonMaxSpeedForce({ dashForceFloat3.x,0.0f,dashForceFloat3.z }); // 前方向の移動
}

//スペーススキル長押し
void FarahCom::SpaceSkillPushing(float elapsedTime)
{
    //上昇
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::JUMPLOOP)
    {
        GetGameObject()->GetComponent<MovementCom>()->Rising(elapsedTime);
    }
}

//Eスキル
void FarahCom::SubSkill()
{
    //ステート変更
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    //一瞬の飛び
    DirectX::XMFLOAT3 power = {
        0.0f,
        Mathf::Lerp(0.0f,10.0f,0.8f),
        0.0f
    };

    // 力を移動コンポーネントに加える
    moveCom->AddForce(power);
}

static float AH = 0;
//fps用の腕アニメーション
void FarahCom::FPSArmAnimation()
{
    auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();

    //待機
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::IDLE)
    {
        if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_idol"))return;

        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_shoot"))
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_idol"), true);
    }

    //移動
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::MOVE)
    {
        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_walk"))
        {
            if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
            {
                if (armAnim->IsEventCalling("attackEnd"))
                    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
            }
            else
                armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
        }
    }

    //アニメーションスピード変更
    float fmax = GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
    float max = GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed();

    float v = max - fmax;
    AH = max;
    if (v < 0)v = 0;

    arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[armAnim->FindAnimation("FPS_walk")].animationspeed = 1 + v * 0.1f;
}