#include "FarahCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Component\Renderer\RendererCom.h"
#include "Math\Mathf.h"
#include "SystemStruct\TimeManager.h"
#include "StateMachine\Behaviar\FarahState.h"

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
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<Farah_MainAttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<Farah_ESkillState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Farah_UltState>(this));

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

    // クールタイム更新
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer = (std::max)(0.0f, cooldownTimer - elapsedTime);
    }

    //ブースト制御
    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    if (moveCom->OnGround())
    {
        boostflag = false;
    }
    else
    {
        boostflag = true;
    }

    //ult更新
    UltUpdate(elapsedTime);
}

//gui
void FarahCom::OnGUI()
{
    ImGui::DragFloat("cooldownTimer", &cooldownTimer);
    CharacterCom::OnGUI();
}

//右クリック単発押し処理
void FarahCom::SubAttackDown()
{
    //初期化
    GetGameObject()->GetComponent<MovementCom>()->SetAirForce(12.620f);

    //ステート変更
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::JUMP);

    const auto& moveCom = GetGameObject()->GetComponent<MovementCom>();

    //初期化
    moveCom->SetVelocity({ moveCom->GetVelocity().x,0.0f,moveCom->GetVelocity().z });

    //一瞬の飛び
    DirectX::XMFLOAT3 power = {
        0.0f,
        Mathf::Lerp(0.0f,13.0f,0.8f),
        0.0f
    };

    // 力を移動コンポーネントに加える
    moveCom->AddForce(power);

    // クールタイム設定（0.5秒）
    cooldownTimer = 0.5f;

    //ダッシュゲージ増加
    dashGauge += 5.0f;
}

//スペーススキル長押し
void FarahCom::SpaceSkillPushing(float elapsedTime)
{
    // クールタイム中は処理を無効化
    if (cooldownTimer > 0.0f)
    {
        return; // クールダウン中なので何もしない
    }

    //上昇
    if (dashGauge > 0.01f)
    {
        if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::JUMPLOOP)
        {
            //ゲージ減らす
            dashGauge -= dashgaugemin * elapsedTime;
            GetGameObject()->GetComponent<MovementCom>()->Rising(elapsedTime);
        }
    }
}

//Eスキル
void FarahCom::SubSkill()
{
    if (!UseUlt())
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    else
        ResetSkillCoolTimer(SkillCoolID::E);
}

//メインの攻撃
void FarahCom::MainAttackDown()
{
    //弾丸発射
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

//ULT
void FarahCom::UltSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);
}

//ウルト更新
void FarahCom::UltUpdate(float elapsedTime)
{
    //ult使用中
    if (UseUlt())
    {
        ulttimer += elapsedTime;

        const auto& move = GetGameObject()->GetComponent<MovementCom>();

        //時間でult解除ステータスを元に戻す
        if (ulttimer > 15.0f)
        {
            dashgaugemin = 4.0f;
            move->SetMoveAcceleration(3.0f);
            FinishUlt();
            ulttimer = 0.0f;
        }
    }
}