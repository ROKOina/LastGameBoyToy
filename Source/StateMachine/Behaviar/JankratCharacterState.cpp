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
    DirectX::XMFLOAT3 pos = transCom.lock()->GetWorldPosition();
    pos.y += 2.0f;

    charaCom.lock()->SetHaveBullet(BulletCreate::JankratBulletFire(owner->GetGameObject(), pos, charaCom.lock()->GetCharaID()));
}

void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    if (charaCom.lock()->GetHaveBullet())
    {
        GameObj bullet = charaCom.lock()->GetHaveBullet();
        RigidBodyCom* rigid = bullet->GetComponent<RigidBodyCom>().get();
        JankratBulletCom* jankratBullet = bullet->GetComponent<JankratBulletCom>().get();

        //弾がセットされていたら発射
        rigid->SetMass(mass);           //質量
        rigid->SetRestitution(restitution);    //反発係数
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true); //速くても貫通しないような計算にするフラグ
        charaCom.lock()->ReleaseHaveBullet();

        jankratBullet->SetLifeTime(bulletLifeTimer);//弾の寿命
        jankratBullet->SetAddGravity(addGravity);

        //TODO 発射地点を銃の位置に変更
        DirectX::XMFLOAT3 vec = owner->GetFpsCameraDir();
        rigid->AddForce(Mathf::Normalize({vec.x, vec.y + fireVecY, vec. z}) * force);
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

void JankratCharacter_JumpLoop::Execute(const float& elapsedTime)
{
    //TODO 空気抵抗を計算した空中制御(できたらベースのジャンプループに置き換え

    //ホバリング
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        moveVec = SceneManager::Instance().InputVec(owner->GetGameObject());

        // 空中での速度制御
        const auto& moveComponent = moveCom.lock(); // moveComのロック
        if (moveComponent)
        {
            DirectX::XMFLOAT3 velocity = moveComponent->GetVelocity();

            // ホバリング中の上下方向の速度を管理
            if (velocity.y < 0.05f && HoveringTimer < HoveringTime)
            {
                // 重力の影響を軽減
                velocity.y = -GRAVITY_NORMAL * 0.5f * elapsedTime;

                // ホバリングタイマーを加算
                HoveringTimer += elapsedTime;

                // 微細な上下動を追加（浮遊感を演出）
                float hoverOscillation = sin(HoveringTimer * 3.0f) * 0.1f; // 振幅0.1、周波数3.0
                velocity.y += hoverOscillation;
            }
            else if (HoveringTimer >= HoveringTime)
            {
                // ホバリング終了後、通常の落下挙動
                velocity.y -= GRAVITY_NORMAL * elapsedTime; // 重力を適用
            }

            // 上下方向の速度を更新
            moveComponent->SetVelocity(velocity);

            // 移動力を計算
            DirectX::XMFLOAT3 force =
            {
                moveVec.x * moveComponent->GetMoveAcceleration(),
                0.0f, // 水平方向のみ力を加える
                moveVec.z * moveComponent->GetMoveAcceleration()
            };

            // 移動力を制限して滑らかさを保つ
            float maxForce = 10.0f; // 最大移動力
            force.x = Mathf::Clamp(force.x, -maxForce, maxForce);
            force.z = Mathf::Clamp(force.z, -maxForce, maxForce);

            // 移動力を適用
            moveComponent->AddNonMaxSpeedForce(force);
        }
    }

    if (moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::LANDING);
    }
}
