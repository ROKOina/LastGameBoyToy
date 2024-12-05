#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //����
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        //�����蔻��̔��a���₷
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //�������_���[�W���炷
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        RigidBodyCom* rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

        //TODO �����Ŕ����̃G�t�F�N�g�Đ�

        explosionFlag = true;
    }
    //����
    if (lifeTimer >= lifeTime + explosionTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }

    //��C��R��d�͂����߂ɂ�����
    GetGameObject()->GetComponent<RigidBodyCom>()->AddForce({ 0, addGravity, 0 });

    lifeTimer += elapsedTime;
}