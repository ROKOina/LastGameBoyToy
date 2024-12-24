#include "OnGroundDeleteCom.h"
#include <Component\MoveSystem\MovementCom.h>
#include "BulletCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Renderer\DecalCom.h"

//更新処理
void OnGroundDeleteCom::Update(float elapsedTime)
{
    //ステージに当たれば削除する関数
    IsGroundDelete();

    //ヒットした位置と法線を取得して設定
    IsRayCast();
}

//ステージに当たれば削除する関数
void OnGroundDeleteCom::IsGroundDelete()
{
    const auto& movecom = GetGameObject()->GetComponent<MovementCom>();
    const auto& bullet = GetGameObject()->GetComponent<BulletCom>();

    //地面に接触
    if (movecom->OnGround() || movecom->GetOnWall())
    {
        //レイキャストを切る
        movecom->SetIsRaycast(false);

        //削除
        GameObjectManager::Instance().Remove(bullet->GetViewBullet().lock());
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}

//ヒットした位置と法線を取得して設定
void OnGroundDeleteCom::IsRayCast()
{
    const auto& movecom = GetGameObject()->GetComponent<MovementCom>();

    //ヒットエフェクト生成
    if (movecom->OnGround() || movecom->GetOnWall())
    {
        std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();

        //デカール生成
        std::shared_ptr<GameObject>decal = GameObjectManager::Instance().Create();
        decal->SetName("decal");
        std::shared_ptr<Decal>d = decal->AddComponent<Decal>("Data/Texture/bullethole.png");

        //ここでヒット種類を分別する
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

        //ヒットエフェクト再生
        hiteffectobject->SetName("HitEffect");
        std::shared_ptr<GPUParticle>Chiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/groundhiteffect.gpuparticle", 1000);
        Chiteffct->Play();
        std::shared_ptr<CPUParticle>Ghiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 100);
        Ghiteffct->SetActive(true);
    }
}