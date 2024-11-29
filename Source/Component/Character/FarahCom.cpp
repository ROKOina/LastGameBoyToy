#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"
#include "SystemStruct\TimeManager.h"

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
}

//右クリック単発押し処理
void FarahCom::SubAttackDown()
{
    // ステート変更
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    // 移動コンポーネント取得
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    // 現在の時間を取得
    float currentTime = TimeManager::Instance().GetElapsedTime();

    // 水平方向のダッシュ力と上昇力に対する減衰設定
    static constexpr float DASH_POWER_BASE = 75.0f;    // 初期ダッシュ力
    static constexpr float DASH_DURATION = 5.0f;      // ダッシュ力が持続する時間
    static constexpr float LIFT_FORCE_BASE = 3.5f;    // 初期上昇力
    static constexpr float LIFT_DURATION = 1.0f;      // 上昇力が持続する時間
    static constexpr float GRAVITY = 9.8f;            // 重力加速度

    // 時間経過に応じたダッシュ力の減衰計算
    float dashPower = DASH_POWER_BASE * (1.0f - (currentTime / DASH_DURATION));
    dashPower = (std::max)(dashPower, 0.0f); // 力が負にならないよう制限

    // 前方向ベクトル（入力方向またはワールド前方向）
    DirectX::XMVECTOR frontVec = IsPushLeftStick()
        ? DirectX::XMLoadFloat3(&SceneManager::Instance().InputVec(GetGameObject()))
        : DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldFront());
    frontVec = DirectX::XMVector3Normalize(frontVec); // 正規化

    // 減衰後のダッシュ力を計算
    DirectX::XMVECTOR dashForce = DirectX::XMVectorScale(frontVec, dashPower);
    DirectX::XMFLOAT3 dashForceFloat3;
    DirectX::XMStoreFloat3(&dashForceFloat3, dashForce);

    // 時間経過に応じた上昇力の減衰計算
    float verticalForce = LIFT_FORCE_BASE - (GRAVITY * currentTime);
    verticalForce = (std::max)(verticalForce, 0.0f); // 力が負にならないよう制限

    // 最終的な力を計算
    DirectX::XMFLOAT3 liftForce = {
        0.0f, // X方向は無し
        verticalForce, // 上昇力
        0.0f  // Z方向は無し
    };

    // 力を移動コンポーネントに加える
    moveCom->AddForce(liftForce); // 上昇力
    moveCom->AddNonMaxSpeedForce({ dashForceFloat3.x, 0.0f, dashForceFloat3.z }); // 水平移動
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
    if (std::string(GetGameObject()->GetName()) != "player")return;
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