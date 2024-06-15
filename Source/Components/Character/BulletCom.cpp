#include "BulletCom.h"
#include "Components\System\GameObject.h"
void BulletCom::Update(float elapsedTime)
{
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