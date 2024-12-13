#include "SolderState.h"

//基底クラス
Solder_BaseState::Solder_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(SoldierCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region メイン攻撃
void Solder_MainAttackState::Enter()
{
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
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

        //弾減らさないとリロードしない
        charaCom.lock()->AddCurrentBulletNum(-1);

        //射撃間隔タイマー起動
        charaCom.lock()->ResetShootTimer();

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_MainAttackState::Exit()
{
}
void Solder_MainAttackState::ImGui()
{
}
#pragma endregion

#pragma region ult攻撃
void Solder_UltState::Enter()
{
}
void Solder_UltState::Execute(const float& elapsedTime)
{
}
void Solder_UltState::Exit()
{
}
void Solder_UltState::ImGui()
{
}
#pragma endregion

#pragma region Eskill
void Solder_ESkillState::Enter()
{
}
void Solder_ESkillState::Execute(const float& elapsedTime)
{
    //攻撃終了処理＆攻撃処理
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //ステート変更
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void Solder_ESkillState::Exit()
{
}
void Solder_ESkillState::ImGui()
{
}
#pragma endregion