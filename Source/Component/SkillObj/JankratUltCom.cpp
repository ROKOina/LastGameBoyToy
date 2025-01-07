#include "JankratUltCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Renderer\DecalCom.h"

//更新処理
void JankratUltCom::Update(float elapsedTime)
{
    //起爆
    Fire(elapsedTime);
}

//起爆
void JankratUltCom::Fire(float elapsedTime)
{
    // MovementCom を取得
    const auto& move = GetGameObject()->GetComponent<MovementCom>();
    const auto& explosion = GetGameObject()->GetChildFind("explosion");

    //着地した瞬間の処理
    if (move->JustLanded() || move->GetJustHitWall())
    {
        //地面判定
        plustime = true;

        move->SetIsRaycast(false);
        move->ZeroNonMaxSpeedVelocity();
        move->ZeroVelocity();
        move->SetGravity(0.0f);

        //ここでパーティクルを停止
        GetGameObject()->GetComponent<CPUParticle>()->SetActive(false);

        //火災エフェクト再生
        explosion->GetComponent<CPUParticle>()->SetActive(true);
    }

    //時間を更新する
    if (plustime)
    {
        //時間経過
        time += elapsedTime;
    }

    //火災エフェクト停止
    if (time > 7.6f)
    {
        GetGameObject()->GetComponent<Collider>()->SetEnabled(false);
        explosion->GetComponent<CPUParticle>()->SetActive(false);
    }

    //時間になれば削除
    if (time > 9.0f)
    {
        //デカール生成
        std::shared_ptr<GameObject>decal = GameObjectManager::Instance().Create();
        decal->SetName("decal");
        std::shared_ptr<Decal>d = decal->AddComponent<Decal>("Data/Texture/susu.png");

        //ここでヒット種類を分別する
        if (move->GetOnceGround())
        {
            d->Add(move->GetHitPosition(), move->GetNormal(), 1.0f);
        }

        if (move->GetOnWall())
        {
            d->Add(move->GetWallHitPosition(), move->GetWallNormal(), 1.0f);
        }

        GameObjectManager::Instance().Remove(GetGameObject());
    }
}