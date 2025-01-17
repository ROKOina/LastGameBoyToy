#include "SolderState.h"
#include <Component\Collsion\ColliderCom.h>
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
    if (!rayobj.lock())return;

    //レイ設定
    auto& ray = rayobj.lock()->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = rayobj.lock()->transform_->GetWorldPosition();

    //カメラ取得
    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //エフェクト再生
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();

    //レイキャスト有効化
    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
{
    auto& ray = rayobj.lock()->GetComponent<RayColliderCom>();

    //レイキャストOFF
    if (CharacterInput::MainAttackButton & owner->GetButton())
    {
        ray->SetEnabled(false);
    }

    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //腕アニメーション再生
        charaCom.lock()->HandleArmAnimation();

        //弾減らさないとリロードしない
        charaCom.lock()->AddCurrentBulletNum(-1);

        //射撃間隔タイマー起動
        charaCom.lock()->ResetShootTimer();

        //レイキャストOFF
        ray->SetEnabled(false);

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_MainAttackState::Exit()
{
    rayobj.reset();
}
#pragma endregion

#pragma region ult攻撃
void Solder_UltState::Enter()
{
    //ウルトオブジェクトを更新
    auto& ultobj = owner->GetGameObject()->GetChildFind("UltObject");
    ultobj->GetComponent<GPUParticle>()->SetLoop(true);
    ultobj->GetComponent<SpawnCom>()->SetOnTrigger(true);
}
void Solder_UltState::Execute(const float& elapsedTime)
{
    time += elapsedTime;

    //時間になれば終了
    if (time > 8.0f)
    {
        //ウルトオブジェクトを更新しない
        auto& ultobj = owner->GetGameObject()->GetChildFind("UltObject");
        ultobj->GetComponent<GPUParticle>()->SetLoop(false);
        ultobj->GetComponent<SpawnCom>()->SetOnTrigger(false);

        //時間を初期化
        time = 0.0f;

        //ステート変更
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_UltState::Exit()
{
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