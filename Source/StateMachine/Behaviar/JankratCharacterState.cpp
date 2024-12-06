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

            charaCom.lock()->SetHaveBullet(BulletCreate::JankratBulletFire(owner->GetGameObject(), gunPos, charaCom.lock()->GetCharaID()));
        }
    }
}

void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    if (charaCom.lock()->GetHaveBullet())
    {
        GameObj bullet = charaCom.lock()->GetHaveBullet();
        RigidBodyCom* rigid = bullet->GetComponent<RigidBodyCom>().get();
        JankratBulletCom* jankratBullet = bullet->GetComponent<JankratBulletCom>().get();

        // 弾がセットされていたら発射
        rigid->SetMass(mass);           // 質量
        rigid->SetRestitution(restitution);    // 反発係数
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true); // 貫通防止フラグ
        charaCom.lock()->ReleaseHaveBullet();

        jankratBullet->SetLifeTime(bulletLifeTimer); // 弾の寿命
        jankratBullet->SetAddGravity(addGravity);

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

                // カメラの方向ベクトルを取得
                DirectX::XMFLOAT3 cameraDir = owner->GetFpsCameraDir();

                // 発射方向ベクトルの計算 (正規化 + Y軸補正)
                DirectX::XMFLOAT3 fireDir = Mathf::Normalize({
                    cameraDir.x,
                    cameraDir.y + fireVecY,
                    cameraDir.z
                    });

                // 弾の初期位置を設定
                bullet->transform_->SetWorldPosition(gunPos);

                // 弾の初速度を設定 (方向ベクトルに速度を乗算)
                rigid->AddForce(fireDir * force);
            }
        }
    }

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
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
    //TODO 発射地点を銃の位置に変更
    DirectX::XMFLOAT3 firePos = owner->GetGameObject()->transform_->GetWorldPosition();
    firePos.y += 2;

    charaCom.lock()->AddHaveMine(BulletCreate::JankratMineFire(owner->GetGameObject(), firePos, 100.0f, 20, owner->GetCharaID()));
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
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