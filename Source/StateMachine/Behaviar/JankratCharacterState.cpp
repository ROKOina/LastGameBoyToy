#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Bullet\JankratBulletCom.h"

//基底クラス君
JankratCharacter_BaseState::JankratCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(JankratCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

// 銃の先端位置を取得
bool JankratCharacter_BaseState::GetGunTipPosition(DirectX::XMFLOAT3& outGunPos) const
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        const auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
        const auto& arm = cameraObj->GetChildFind("armChild");

        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& gunNode = model->FindNode("gun2"); // 銃の先端ボーン名（仮名）
        if (!gunNode) return false;

        outGunPos =
        {
            gunNode->worldTransform._41,
            gunNode->worldTransform._42,
            gunNode->worldTransform._43
        };
    }

    return true;
}

// 腕アニメーション処理
void JankratCharacter_BaseState::HandleArmAnimation() const
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        const auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        const auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }
}

// 弾丸を発射する処理
void JankratCharacter_BaseState::FireBullet(const GameObj& bullet)
{
    const auto& rigid = bullet->GetComponent<RigidBodyCom>().get();
    const auto& jankratBullet = bullet->GetComponent<JankratBulletCom>().get();
    if (!rigid || !jankratBullet) return;

    // 弾丸の物理プロパティ設定
    rigid->SetMass(mass);
    rigid->SetRestitution(restitution);
    rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);

    // 弾丸の寿命と重力設定
    jankratBullet->SetLifeTime(bulletLifeTimer);
    jankratBullet->SetAddGravity(addGravity);
    jankratBullet->SetExplosionTime(explosiontime);

    // 銃の先端位置とカメラ方向を使用して弾丸を発射
    DirectX::XMFLOAT3 gunPos = {}, fireDir;
    if (GetGunTipPosition(gunPos))
    {
        // 発射方向を計算
        fireDir = Mathf::Normalize({
            owner->GetFpsCameraDir().x,
            owner->GetFpsCameraDir().y + fireVecY,
            owner->GetFpsCameraDir().z
            });

        // 弾丸の初期位置と初速度を設定
        bullet->transform_->SetWorldPosition(gunPos);
        rigid->AddForce(fireDir * force);
    }
}

#pragma region 通常弾
void JankratCharacter_MainAtkState::Enter()
{
    const auto& charaComponent = charaCom.lock();

    if (!charaComponent)
    {
        return;
    }

    // 銃の先端位置を取得
    DirectX::XMFLOAT3 gunPos;
    if (GetGunTipPosition(gunPos))
    {
        // 弾丸を作成しセット
        const auto& bullet = BulletCreate::JankratBulletFire(owner->GetGameObject(), gunPos, charaCom.lock()->GetNetCharaData().GetCharaID());
        charaComponent->SetHaveBullet(bullet);
    }
}
void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    const auto& charaComponent = charaCom.lock();
    if (!charaComponent)
    {
        return;
    }

    //腕アニメーション再生
    HandleArmAnimation();

    if (const auto& bullet = charaComponent->GetHaveBullet())
    {
        FireBullet(bullet);
        charaComponent->ReleaseHaveBullet();

        //弾減らさないとリロードしない
        charaComponent->AddCurrentBulletNum(-1);

        //初期化
        charaComponent->ResetShootTimer();

        // 状態遷移
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void JankratCharacter_MainAtkState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
    ImGui::DragFloat("BulletLifeTimer", &bulletLifeTimer);
    ImGui::DragFloat("AddGravity", &addGravity);
    ImGui::DragFloat("VecY", &fireVecY);
}
#pragma endregion

#pragma region 地雷設置
void JankratCharacter_MainSkillState::Execute(const float& elapsedTime)
{
    //腕アニメーションをする
    HandleArmAnimation();

    //銃の位置から発射
    DirectX::XMFLOAT3 gunPos = {};
    if (GetGunTipPosition(gunPos))
    {
        // 弾丸を作成しセット
        charaCom.lock()->AddHaveMine(BulletCreate::JankratMineFire(owner->GetGameObject(), gunPos, 100.0f, 20, charaCom.lock()->GetNetCharaData().GetCharaID()));
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
#pragma endregion

#pragma region 地雷起爆
void JankratCharacter_SubAttackState::Execute(const float& elapsedTime)
{
    for (auto& mine : charaCom.lock()->GetHaveMine())
    {
        //全ての設置中の地雷を起爆
        mine->GetComponent<JankratMineCom>()->Fire();
    }

    //遷移
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion

#pragma region ウルト
void JankratCharacter_UltState::Enter()
{
    const auto& charaComponent = charaCom.lock();

    if (!charaComponent)
    {
        return;
    }

    //初期設定
    force = 60.0f;
    fireVecY = 1.350f;
    bulletLifeTimer = 1.0f;
    explosiontime = 0.1f;

    // 銃の先端位置を取得
    DirectX::XMFLOAT3 gunPos;
    if (GetGunTipPosition(gunPos))
    {
        // 弾丸を作成しセット
        const auto& bullet = BulletCreate::JankratUlt(owner->GetGameObject(), gunPos, 5.0f);
        charaComponent->SetHaveBullet(bullet);
    }
}
void JankratCharacter_UltState::Execute(const float& elapsedTime)
{
    if (!charaCom.lock()->UseUlt()) return;

    const auto& charaComponent = charaCom.lock();
    if (!charaComponent)
    {
        return;
    }

    //腕アニメーション再生
    HandleArmAnimation();

    if (const auto& bullet = charaComponent->GetHaveBullet())
    {
        FireBullet(bullet);

        charaComponent->ReleaseHaveBullet();

        //初期化
        charaComponent->ResetShootTimer();

        // 状態遷移
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void JankratCharacter_UltState::Exit()
{
    //設定を戻しておく
    force = 50.0f;
    fireVecY = 0.350f;
    bulletLifeTimer = 3.0f;
    explosiontime = 1.5f;

    //ult終わり
    charaCom.lock()->FinishUlt();
}
void JankratCharacter_UltState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
    ImGui::DragFloat("BulletLifeTimer", &bulletLifeTimer);
    ImGui::DragFloat("AddGravity", &addGravity);
    ImGui::DragFloat("VecY", &fireVecY);
}
#pragma endregion