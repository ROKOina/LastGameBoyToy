#include "JankratMineCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratMineCom::Update(float elapsedTime)
{
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //�N��
    if (explosionBegin)
    {
        //�����蔻��̔��a���₷
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);
        sphere->SetJudgeTag(COLLIDER_TAG::Enemy);

        //�m�b�N�o�b�N�����ON�ɂ���
        SphereColliderCom* childCollder = GetGameObject()->GetChildren()[0].lock()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);
        childCollder->SetJudgeTag(COLLIDER_TAG::Player | COLLIDER_TAG::Enemy);

        //�������_���[�W���炷
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        explosionFlag = true;
        explosionBegin = false;
    }
    else if (explosionFlag)
    {
        //�����蔻��OFF
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetMyTag(COLLIDER_TAG::NONE_COL);
        SphereColliderCom* childCollder = GetGameObject()->GetChildren()[0].lock()->GetComponent<SphereColliderCom>().get();
        childCollder->SetMyTag(COLLIDER_TAG::NONE_COL);

        //TODO �����Ŕ����G�t�F�N�g�Đ�

        //���������莞�Ԃŏ���
        if (lifeTimer >= lifeTime)
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

void JankratMineCom::Fire()
{
    explosionBegin = true;
}