#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Bullet\JankratBulletCom.h"

JankratCharacter_BaseState::JankratCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(JankratCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void JankratCharacter_MainAtkState::Enter()
{
    const auto& charaComponent = charaCom.lock();

    if (!charaComponent)
    {
        return;
    }
    charaCom.lock()->SetHaveBullet(BulletCreate::JankratBulletFire(owner->GetGameObject(), pos, charaCom.lock()->GetNetCharaData().GetCharaID()));
}

    // 銃の先端位置を取得
    DirectX::XMFLOAT3 gunPos;
    if (GetGunTipPosition(gunPos))
    {
        // 弾丸を作成しセット
        const auto& bullet = BulletCreate::JankratBulletFire(owner->GetGameObject(), gunPos, charaComponent->GetCharaID());
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

// 銃の先端位置を取得
bool JankratCharacter_MainAtkState::GetGunTipPosition(DirectX::XMFLOAT3& outGunPos) const
{
    const auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
    const auto& arm = cameraObj->GetChildFind("armChild");
    if (!arm) return false;

    const auto& model = arm->GetComponent<RendererCom>()->GetModel();
    const auto& gunNode = model->FindNode("gun2"); // 銃の先端ボーン名（仮名）
    if (!gunNode) return false;

    outGunPos =
    {
        gunNode->worldTransform._41,
        gunNode->worldTransform._42,
        gunNode->worldTransform._43
    };

    return true;
}

// 腕アニメーション処理
void JankratCharacter_MainAtkState::HandleArmAnimation() const
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
void JankratCharacter_MainAtkState::FireBullet(const GameObj& bullet)
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

    // 銃の先端位置とカメラ方向を使用して弾丸を発射
    DirectX::XMFLOAT3 gunPos, fireDir;
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

void JankratCharacter_MainAtkState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
    ImGui::DragFloat("BulletLifeTimer", &bulletLifeTimer);
    ImGui::DragFloat("AddGravity", &addGravity);
    ImGui::DragFloat("VecY", &fireVecY);
}

void JankratCharacter_MainSkillState::Enter()
{
}

void JankratCharacter_MainSkillState::Execute(const float& elapsedTime)
{
    //腕アニメーションをする
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //銃の位置から発射
    auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
    auto& arm = cameraObj->GetChildFind("armChild");
    if (arm)
    {
        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& gunNode = model->FindNode("gun2"); // 銃の先端ボーン名（仮名）

        if (gunNode)
        {
            // 銃先端ボーンのワールド位置を取得
            DirectX::XMFLOAT3 gunPos =
            {
                gunNode->worldTransform._41,
                gunNode->worldTransform._42,
                gunNode->worldTransform._43
            };

            charaCom.lock()->AddHaveMine(BulletCreate::JankratMineFire(owner->GetGameObject(), gunPos, 100.0f, 20, owner->GetCharaID()));
            ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        }
    }
}

void JankratCharacter_SubAttackState::Execute(const float& elapsedTime)
{
    for (auto& mine : charaCom.lock()->GetHaveMine())
    {
        //全ての設置中の地雷を起爆
        mine->GetComponent<JankratMineCom>()->Fire();
    }

    //TODO アニメーション終わってから遷移
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}