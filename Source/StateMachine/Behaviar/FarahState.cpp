#include "FarahState.h"
#include "Component\Bullet\BulletCom.h"

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
void Farah_MainAttackState::Enter()
{
}
void Farah_MainAttackState::Execute(const float& elapsedTime)
{
    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //腕アニメーションをする
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.3f);
        }

        //上半身アニメーション?
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);

        //弾減らさないとリロードしない
        charaCom.lock()->AddCurrentBulletNum(-1);

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
    moveCom.lock()->SetMoveAcceleration(5.0f);
    charaCom.lock()->SetDashGaugeMins(2.0f);
}
#pragma endregion

#pragma region Eskill
void Farah_ESkillState::Enter()
{
}
void Farah_ESkillState::Execute(const float& elapsedTime)
{
    //攻撃終了処理＆攻撃処理
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //攻撃処理
    charaCom.lock()->AddBullet(BulletCreate::FarahKnockBack(owner->GetGameObject(), 30.0f, 2.0f));

    //ステート変更
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion