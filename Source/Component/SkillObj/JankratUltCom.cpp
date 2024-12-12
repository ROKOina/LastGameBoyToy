#include "JankratUltCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Particle\CPUParticle.h"

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

    //時間経過
    time += elapsedTime;

    //地面判定
    if (move->OnGround())
    {
        //着地した瞬間の処理
        if (move->JustLanded())
        {
            move->SetIsRaycast(false);
            move->ZeroNonMaxSpeedVelocity();
            move->ZeroVelocity();
            move->SetGravity(0.0f);

            //ここでパーティクルを停止
            GetGameObject()->GetComponent<CPUParticle>()->SetActive(false);

            //火災エフェクト再生
            const auto& explosion = GetGameObject()->GetChildFind("explosion");
            explosion->GetComponent<CPUParticle>()->SetActive(true);
        }
    }

    //時間になれば削除
    if (time > 7.0f)
    {
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}