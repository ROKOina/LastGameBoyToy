#include "OnGroundDeleteCom.h"
#include <Component\MoveSystem\MovementCom.h>
#include "BulletCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Renderer\DecalCom.h"

//�X�V����
void OnGroundDeleteCom::Update(float elapsedTime)
{
    //�X�e�[�W�ɓ�����΍폜����֐�
    IsGroundDelete();

    //�q�b�g�����ʒu�Ɩ@�����擾���Đݒ�
    IsRayCast();
}

//�X�e�[�W�ɓ�����΍폜����֐�
void OnGroundDeleteCom::IsGroundDelete()
{
    const auto& movecom = GetGameObject()->GetComponent<MovementCom>();
    const auto& bullet = GetGameObject()->GetComponent<BulletCom>();

    //�n�ʂɐڐG
    if (movecom->OnGround() || movecom->GetOnWall())
    {
        //���C�L���X�g��؂�
        movecom->SetIsRaycast(false);

        //�폜
        GameObjectManager::Instance().Remove(bullet->GetViewBullet().lock());
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}

//�q�b�g�����ʒu�Ɩ@�����擾���Đݒ�
void OnGroundDeleteCom::IsRayCast()
{
    const auto& movecom = GetGameObject()->GetComponent<MovementCom>();

    //�q�b�g�G�t�F�N�g����
    if (movecom->OnGround() || movecom->GetOnWall())
    {
        std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();

        //�f�J�[������
        std::shared_ptr<GameObject>decal = GameObjectManager::Instance().Create();
        decal->SetName("decal");
        std::shared_ptr<Decal>d = decal->AddComponent<Decal>("Data/Texture/bullethole.png");

        //�����Ńq�b�g��ނ𕪕ʂ���
        if (movecom->OnGround())
        {
            hiteffectobject->transform_->SetWorldPosition(movecom->GetHitPosition());
            d->Add(movecom->GetHitPosition(), movecom->GetNormal(), 1.0f);
        }

        if (movecom->GetOnWall())
        {
            hiteffectobject->transform_->SetWorldPosition(movecom->GetWallHitPosition());
            d->Add(movecom->GetWallHitPosition(), movecom->GetWallNormal(), 1.0f);
        }

        //�q�b�g�G�t�F�N�g�Đ�
        hiteffectobject->SetName("HitEffect");
        std::shared_ptr<GPUParticle>Chiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/groundhiteffect.gpuparticle", 1000);
        Chiteffct->Play();
        std::shared_ptr<CPUParticle>Ghiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 100);
        Ghiteffct->SetActive(true);
    }
}