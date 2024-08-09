#include "BulletCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\CharacterCom.h"
#include "Components\System\GameObject.h"
#include "Netwark/Photon/StaticSendDataManager.h"

void BulletCom::Update(float elapsedTime)
{
    std::shared_ptr<SphereColliderCom> collider = GetGameObject()->GetComponent<SphereColliderCom>();
    if (collider->OnHitGameObject().size())
    {
        //�l�b�g�ő��M
        for (auto& c : collider->OnHitGameObject())
        {
            int sendID = c.gameObject.lock()->GetComponent<CharacterCom>()->GetNetID();
            StaticSendDataManager::Instance().SetSendDamage(ownerID, sendID, 1);
        }
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
    //�e����
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