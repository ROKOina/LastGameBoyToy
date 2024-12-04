#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //”š”­
    if (!explosionFlag && lifeTimer >= explosionTime)
    {
        //“–‚½‚è”»’è‚Ì”¼Œa‘‚â‚·
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //’¼Œ‚‚æ‚èƒ_ƒ[ƒWŒ¸‚ç‚·
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        explosionFlag = true;
    }
    //Á‹Ž
    if (lifeTimer >= lifeTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }

    lifeTimer += elapsedTime;
}
