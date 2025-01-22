#include "SolderState.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\System\SpawnCom.h"

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
    rayobj = owner->GetGameObject()->GetChildFind("mainattack");
    if (!rayobj)return;

    //レイ設定
    ray = rayobj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = rayobj->transform_->GetWorldPosition();

    //カメラ取得
    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //エフェクト再生
    //owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();

    //レイキャスト有効化
    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);

    //腕アニメーション再生
    charaCom.lock()->HandleArmAnimation();
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
{
    //レイキャストOFF
    if (CharacterInput::MainAttackButton & owner->GetButton())
    {
        ray->SetEnabled(false);

        //弾減らさないとリロードしない
        if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
        {
            charaCom.lock()->AddCurrentBulletNum(-1);
        }
    }

    //時間を経過させて打つ時間を調整
    if (CharacterInput::MainAttackButton & owner->GetButton())
    {
        shottimer += elapsedTime;
    }

    //ここで時間を0にする
    if (shottimer > 0.4f)
    {
        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //レイキャストOFF
        ray->SetEnabled(true);

        //弾減らさないとリロードしない
        if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
        {
            charaCom.lock()->AddCurrentBulletNum(-1);
        }

        //時間を初期化
        shottimer = 0.0f;
    }

    //右クリックを上げたら時間は0に初期化する
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_MainAttackState::Exit()
{
    //レイキャストOFF
    ray->SetEnabled(false);

    //諸々初期化
    shottimer = 0.0f;
    rayobj.reset();
}
void Solder_MainAttackState::ImGui()
{
    ImGui::DragFloat("shottimer", &shottimer);
    ImGui::Checkbox("rayenabled", &rayenabled);
}
#pragma endregion

#pragma region ult攻撃
void Solder_UltState::Enter()
{
    //レイキャストのリセット
    //rayobj = owner->GetGameObject()->GetChildFind("mainattack");
    //rayobj.reset();

    //ウルトオブジェクトを更新
    auto& ultobj = owner->GetGameObject()->GetChildFind("UltObject");
    ultobj->GetComponent<GPUParticle>()->SetLoop(true);
    ultobj->GetComponent<SpawnCom>()->SetOnTrigger(true);
}
void Solder_UltState::Execute(const float& elapsedTime)
{
    owner->GetUltTimer() += elapsedTime;

    //時間になれば終了
    if (owner->GetUltTimer() > owner->GetMaxUltTime())
    {
        //ウルトオブジェクトを更新しない
        auto& ultobj = owner->GetGameObject()->GetChildFind("UltObject");
        ultobj->GetComponent<GPUParticle>()->SetLoop(false);
        ultobj->GetComponent<SpawnCom>()->SetOnTrigger(false);

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_UltState::Exit()
{
    //時間初期化
    owner->SetUltTimer(0.0f);

    //ult終了
    charaCom.lock()->FinishUlt();
}
#pragma endregion

#pragma region Eskill
void Solder_ESkillState::Enter()
{
    //エフェクト再生
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();
}
void Solder_ESkillState::Execute(const float& elapsedTime)
{
    //腕アニメーション再生
    charaCom.lock()->HandleArmAnimation();

    //攻撃処理(スタンボール)
    BulletCreate::SoldierStanBall(owner->GetGameObject(), 50.0f, 10.0f, 2.0f);

    //ステート変更
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion

#pragma region 右クリックスキル
void Solder_RightClickSkillState::Enter()
{
    //エフェクト再生
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();
}
void Solder_RightClickSkillState::Execute(const float& elapsedTime)
{
    //腕アニメーション再生
    charaCom.lock()->HandleArmAnimation();

    //攻撃処理
    BulletCreate::SoldierEskillBullet(owner->GetGameObject(), 50.0f, 20.0f);

    //ステート変更
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion