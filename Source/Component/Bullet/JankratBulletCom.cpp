#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //爆発
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        //当たり判定の半径増やす
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //直撃よりダメージ減らす
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        RigidBodyCom* rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

        //TODO ここで爆発のエフェクト再生

        explosionFlag = true;
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
