#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\System\SpawnCom.h"

//更新処理
void JankratBulletCom::Update(float elapsedTime)
{
    // 爆発処理
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        TriggerExplosion();
    }

    // 直撃ダメージ処理
    const auto& collider = GetGameObject()->GetComponent<SphereColliderCom>().get();
    if (!collider->OnHitGameObject().empty())
    {
        ApplyDirectHitDamage();
    }

    // オブジェクトの削除
    if (lifeTimer >= lifeTime + explosionTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
        return;
    }

    // 空気抵抗や重力の適用
    const auto& rigidBody = GetGameObject()->GetComponent<RigidBodyCom>();
    if (rigidBody)
    {
        rigidBody->AddForce({ 0, addGravity, 0 });
    }

    //ウルト更新
    UltUpdate(elapsedTime);

    // タイマーの更新
    lifeTimer += elapsedTime;
}

//ult更新
void JankratBulletCom::UltUpdate(float elapsedTime)
{
    //爆破時間でspawnタイミングでやる
    if (lifeTimer >= 1.0f)
    {
        const auto& spawn = GetGameObject()->GetComponent<SpawnCom>();

        //生成フラグをON
        if (spawn)
        {
            spawn->SetOnTrigger(true);
        }
    }
}

//爆発させる
void JankratBulletCom::TriggerExplosion()
{
    // 当たり判定の半径を増やす
    const auto& sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
    sphere->SetRadius(sphere->GetRadius() * 2.5f);

    // ダメージを減少させる
    damageValue *= 0.8f;

    // 剛体をキネマティックに変更
    const auto& rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
    rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

    // 爆発エフェクトの再生
    if (GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion") != nullptr)
    {
        auto& explosionEffect = GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion")->GetComponent<CPUParticle>();
        if (explosionEffect)
        {
            explosionEffect->SetActive(true);
        }
    }

    explosionFlag = true;
}

//直撃ダメージを与える
void JankratBulletCom::ApplyDirectHitDamage()
{
    const auto& collider = GetGameObject()->GetComponent<SphereColliderCom>().get();
    for (const auto& obj : collider->OnHitGameObject())
    {
        const auto& chara = obj.gameObject.lock()->GetComponent<CharaStatusCom>().get();
        if (chara)
        {
            chara->AddDamagePoint(-damageValue);

            //消す処理
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
    lifeTimer = lifeTime;
    TriggerExplosion();
}