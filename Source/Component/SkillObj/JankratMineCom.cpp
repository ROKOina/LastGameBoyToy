#include "JankratMineCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratMineCom::Update(float elapsedTime)
{
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //起爆
    if (explosionFlag)
    {
        //当たり判定の半径増やす
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);

        //直撃よりダメージ減らす
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        //TODO エフェクト再生

        if (lifeTime >= lifeTimer)
        {
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
        lifeTimer += elapsedTime;
    }



    //壁に当たれば設置
    if (!isPlant && (moveCom->GetJustHitWall() || moveCom->JustLanded()))
    {
        moveCom->SetIsRaycast(false);
        moveCom->ZeroVelocity();
        moveCom->ZeroNonMaxSpeedVelocity();
        isPlant = true;

        //設置状態でぶつかってもダメージを与えないようにする
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetJudgeTag(COLLIDER_TAG::NONE_COL);
    }
}
