#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Character\CharaStatusCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //爆発
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        //当たり判定の半径増やす
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //直撃よりダメージ減らす
        damageValue *= 0.8f;

        RigidBodyCom* rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

        // ここで爆発のエフェクト再生
        GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion")->GetComponent<CPUParticle>()->SetActive(true);

        explosionFlag = true;
    }

    //直撃ダメージ処理
    SphereColliderCom* collider = GetGameObject()->GetComponent<SphereColliderCom>().get();   
    if (collider->OnHitGameObject().size())
    {
        for (auto& obj : collider->OnHitGameObject())
        {
            CharaStatusCom* chara = obj.gameObject.lock()->GetComponent<CharaStatusCom>().get();
            if (chara) chara->AddDamagePoint(-damageValue);
        }
        lifeTimer = lifeTime;
        explosionFlag = true;

        // ここで爆発のエフェクト再生
        GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion")->GetComponent<CPUParticle>()->SetActive(true);

    }

    //消去
    if (lifeTimer >= lifeTime + explosionTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }

    //空気抵抗や重力を強めにかける
    GetGameObject()->GetComponent<RigidBodyCom>()->AddForce({ 0, addGravity, 0 });

    lifeTimer += elapsedTime;
}