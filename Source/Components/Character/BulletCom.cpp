#include "BulletCom.h"
#include "Components\ColliderCom.h"
#include "Components\System\GameObject.h"

void BulletCom::Update(float elapsedTime)
{
    std::shared_ptr<SphereColliderCom> collider = GetGameObject()->GetComponent<SphereColliderCom>();
    if(collider->OnHitGameObject().size()) 
    {



        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
    //’eÁ‹Ž
    EraseBullet(elapsedTime);
}

void BulletCom::EraseBullet(float elapsedTime)
{
    timer += elapsedTime;
    if (timer > aliveTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
}