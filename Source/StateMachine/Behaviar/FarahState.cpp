#include "FarahState.h"
#include "Component\Bullet\BulletCom.h"
#include <Component\Collsion\ColliderCom.h>
#include "Component\Particle\GPUParticle.h"

//基底クラスです
Farah_BaseState::Farah_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(FarahCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region メイン攻撃
void Farah_MainAttackState::Execute(const float& elapsedTime)
{
    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //弾減らさないとリロードしない
        if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
        {
            charaCom.lock()->AddCurrentBulletNum(-1);
        }

        //攻撃処理
        charaCom.lock()->AddBullet(BulletCreate::FarahDamageFire(owner->GetGameObject(), 40.0f));

        //射撃間隔タイマー起動
        charaCom.lock()->ResetShootTimer();

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
#pragma endregion

#pragma region ult攻撃
void Farah_UltState::Enter()
{
    owner->GetGameObject()->GetChildFind("UltObject")->GetComponent<GPUParticle>()->SetLoop(true);

    //設定
    charaCom.lock()->SetCurrentBulletNum(100);
    charaCom.lock()->SetMaxBulletNum(100);
}
void Farah_UltState::Execute(const float& elapsedTime)
{
    owner->GetUltTimer() += elapsedTime;

    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //攻撃処理
        charaCom.lock()->AddBullet(BulletCreate::FarahDamageFire(owner->GetGameObject(), 40.0f));
    }

    if (owner->GetUltTimer() > owner->GetMaxUltTime())
    {
        owner->GetGameObject()->GetChildFind("UltObject")->GetComponent<GPUParticle>()->SetLoop(false);

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Farah_UltState::Exit()
{
    //弾数を元に戻す
    charaCom.lock()->SetCurrentBulletNum(10);
    charaCom.lock()->SetMaxBulletNum(10);

    //時間初期化
    owner->SetUltTimer(0.0f);

    //ult終了
    charaCom.lock()->FinishUlt();
}
#pragma endregion

#pragma region Eskill
void Farah_ESkillState::Execute(const float& elapsedTime)
{
    //腕アニメーション再生
    charaCom.lock()->HandleArmAnimation();

    //攻撃処理
    charaCom.lock()->AddBullet(BulletCreate::FarahKnockBack(owner->GetGameObject(), 30.0f, 2.0f));

    //ステート変更
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion