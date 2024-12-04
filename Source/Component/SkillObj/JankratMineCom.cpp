#include "JankratMineCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratMineCom::Update(float elapsedTime)
{
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //�N��
    if (explosionFlag)
    {
        //�����蔻��̔��a���₷
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //�������_���[�W���炷
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        //TODO �G�t�F�N�g�Đ�

        if (lifeTime >= lifeTimer)
        {
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
        lifeTimer += elapsedTime;
    }



    //�ǂɓ�����ΐݒu
    if (!isPlant && (moveCom->GetJustHitWall() || moveCom->JustLanded()))
    {
        moveCom->SetIsRaycast(false);
        moveCom->ZeroVelocity();
        moveCom->ZeroNonMaxSpeedVelocity();
        isPlant = true;

        //�ݒu��ԂłԂ����Ă��_���[�W��^���Ȃ��悤�ɂ���
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetJudgeTag(COLLIDER_TAG::NONE_COL);
    }
}
