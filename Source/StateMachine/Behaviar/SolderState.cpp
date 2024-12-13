#include "SolderState.h"
#include <Component\Collsion\ColliderCom.h>
#include "Component\Renderer\RendererCom.h"
#include "Component\Particle\GPUParticle.h"

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
    auto& ray = rayobj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = rayobj->transform_->GetWorldPosition();

    //カメラ取得
    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //銃の位置を取得
    DirectX::XMFLOAT3 gunpos = {};
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        const auto& arm = camera->GetChildFind("armChild");
        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& gunNode = model->FindNode("gun2"); // 銃の先端ボーン名（仮名）

        gunpos =
        {
            gunNode->worldTransform._41,
            gunNode->worldTransform._42,
            gunNode->worldTransform._43
        };
    }

    ////パーティクル生成
    //std::shared_ptr<GameObject> beem = GameObjectManager::Instance().Create();
    //beem->transform_->SetWorldPosition(gunpos);
    ////beem->transform_->SetRotation()
    //beem->SetName("beem");
    //std::shared_ptr<GPUParticle>beemeffect = beem->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/soldier_mainattack.gpuparticle", 300);
    //beemeffect->Play();

    //レイキャスト有効化
    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
{
    auto& ray = rayobj->GetComponent<RayColliderCom>();

    //レイキャストOFF
    if (CharacterInput::MainAttackButton & owner->GetButton())
    {
        ray->SetEnabled(false);
    }

    //攻撃終了処理＆攻撃処理
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //腕アニメーションをする
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.5f);
        }

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