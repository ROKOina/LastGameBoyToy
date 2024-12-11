#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Character\CharaStatusCom.h"

void JankratBulletCom::Update(float elapsedTime)
{
    //����
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        //�����蔻��̔��a���₷
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //�������_���[�W���炷
        damageValue *= 0.8f;

        RigidBodyCom* rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

        // �����Ŕ����̃G�t�F�N�g�Đ�
        GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion")->GetComponent<CPUParticle>()->SetActive(true);

        explosionFlag = true;
    }

    //�����_���[�W����
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

        // �����Ŕ����̃G�t�F�N�g�Đ�
        GetGameObject()->GetChildFind("bomber")->GetChildFind("explosion")->GetComponent<CPUParticle>()->SetActive(true);

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