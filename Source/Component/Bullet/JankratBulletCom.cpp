#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //����
    if (!explosionFlag && lifeTimer >= explosionTime)
    {
        //�����蔻��̔��a���₷
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //�������_���[�W���炷
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        explosionFlag = true;
    }
    //����
    if (lifeTimer >= lifeTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }

    lifeTimer += elapsedTime;
}
