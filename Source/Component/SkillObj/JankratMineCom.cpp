#include "JankratMineCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\ColliderCom.h"

void JankratMineCom::Update(float elapsedTime)
{
    MovementCom* moveCom = GetGameObject()->GetComponent<MovementCom>().get();

    //起爆
    if (explosionBegin)
    {
        //当たり判定の半径増やす
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);
        sphere->SetJudgeTag(COLLIDER_TAG::Enemy);

        //ノックバック判定をONにする
        SphereColliderCom* childCollder = GetGameObject()->GetChildren()[0].lock()->GetComponent<SphereColliderCom>().get();
        sphere->SetRadius(sphere->GetRadius() * 2.5f);
        childCollder->SetJudgeTag(COLLIDER_TAG::Player | COLLIDER_TAG::Enemy);

        //直撃よりダメージ減らす
        HitProcessCom* hit = GetGameObject()->GetComponent<HitProcessCom>().get();
        hit->SetValue(hit->GetValue() * 0.8f);

        explosionFlag = true;
        explosionBegin = false;
    }
    else if (explosionFlag)
    {
        //当たり判定OFF
        SphereColliderCom* sphere = GetGameObject()->GetComponent<SphereColliderCom>().get();
        sphere->SetMyTag(COLLIDER_TAG::NONE_COL);
        SphereColliderCom* childCollder = GetGameObject()->GetChildren()[0].lock()->GetComponent<SphereColliderCom>().get();
        childCollder->SetMyTag(COLLIDER_TAG::NONE_COL);

        //TODO ここで爆発エフェクト再生

        //爆発から一定時間で消去
        if (lifeTimer >= lifeTime)
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

void JankratMineCom::Fire()
{
    explosionBegin = true;
}