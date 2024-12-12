#include "JankratBulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\System\SpawnCom.h"

//�X�V����
void JankratBulletCom::Update(float elapsedTime)
{
    // ��������
    if (!explosionFlag && lifeTimer >= lifeTime)
    {
        TriggerExplosion();
    }

    // �����_���[�W����
    const auto& collider = GetGameObject()->GetComponent<SphereColliderCom>().get();
    if (!collider->OnHitGameObject().empty())
    {
        ApplyDirectHitDamage();
    }

    // �I�u�W�F�N�g�̍폜
    if (lifeTimer >= lifeTime + explosionTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
        return;
    }

    // ��C��R��d�͂̓K�p
    const auto& rigidBody = GetGameObject()->GetComponent<RigidBodyCom>();
    if (rigidBody)
    {
        rigidBody->AddForce({ 0, addGravity, 0 });
    }

    //�E���g�X�V
    UltUpdate(elapsedTime);

    // �^�C�}�[�̍X�V
    lifeTimer += elapsedTime;
}

//ult�X�V
void JankratBulletCom::UltUpdate(float elapsedTime)
{
    //���j���Ԃ�spawn�^�C�~���O�ł��
    if (lifeTimer >= 1.0f)
    {
        const auto& spawn = GetGameObject()->GetComponent<SpawnCom>();

        //�����t���O��ON
        if (spawn)
        {
            spawn->SetOnTrigger(true);
        }
    }
}

//����������
void JankratBulletCom::TriggerExplosion()
{
    // �����蔻��̔��a�𑝂₷
    const auto& sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
    sphere->SetRadius(sphere->GetRadius() * 2.5f);

    // �_���[�W������������
    damageValue *= 0.8f;

    // ���̂��L�l�}�e�B�b�N�ɕύX
    const auto& rigid = GetGameObject()->GetComponent<RigidBodyCom>().get();
    rigid->SetRigidFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

    // �����G�t�F�N�g�̍Đ�
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

//�����_���[�W��^����
void JankratBulletCom::ApplyDirectHitDamage()
{
    const auto& collider = GetGameObject()->GetComponent<SphereColliderCom>().get();
    for (const auto& obj : collider->OnHitGameObject())
    {
        const auto& chara = obj.gameObject.lock()->GetComponent<CharaStatusCom>().get();
        if (chara)
        {
            chara->AddDamagePoint(-damageValue);

            //��������
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
    lifeTimer = lifeTime;
    TriggerExplosion();
}