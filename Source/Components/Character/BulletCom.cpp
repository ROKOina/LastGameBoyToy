#include "BulletCom.h"
#include "Components\ColliderCom.h"
#include "Components\TransformCom.h"
#include "Components\RendererCom.h"
#include "Components\Character\CharacterCom.h"
#include "Components\System\GameObject.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "HitProcess/HitProcessCom.h"

void BulletCom::Update(float elapsedTime)
{
    //std::shared_ptr<SphereColliderCom> collider = GetGameObject()->GetComponent<SphereColliderCom>();
    //if (collider->OnHitGameObject().size())
    //{
    //    //ネットで送信
    //    for (auto& c : collider->OnHitGameObject())
    //    {
    //        int sendID = c.gameObject.lock()->GetComponent<CharacterCom>()->GetNetID();
    //        switch (hitType)
    //        {
    //        case HitProcessCom::HIT_TYPE::DAMAGE:
    //            StaticSendDataManager::Instance().SetSendDamage(ownerID, sendID, 1);
    //            break;
    //        case HitProcessCom::HIT_TYPE::HEAL:
    //            StaticSendDataManager::Instance().SetSendHeal(ownerID, sendID, 1);
    //            break;
    //        case HitProcessCom::HIT_TYPE::STAN:
    //            StaticSendDataManager::Instance().SetSendStan(ownerID, sendID, 2);
    //            break;
    //        default:
    //            break;
    //        }
    //    }
    //    GameObjectManager::Instance().Remove(this->GetGameObject());
    //}
    
    //ヒットプロセスがあれば処理する
    auto& hitProcessCom = GetGameObject()->GetComponent<HitProcessCom>();
    if (hitProcessCom)
    {
        if (GetGameObject()->GetComponent<HitProcessCom>()->IsHit())
        {
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }

    //弾消去
    EraseBullet(elapsedTime);
}

void BulletCom::EraseBullet(float elapsedTime)
{
    timer += elapsedTime;
    if (timer > aliveTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }

    //地面に着いたら消す
    if (GetGameObject()->GetComponent<MovementCom>()->OnGround())
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
}




//ダメージ弾生成
void BulletCreate::DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //弾丸オブジェクトを生成///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("damageball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //弾発射
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = -0.1 * power;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + bulletSpeed));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //弾
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);

    //判定用
    std::shared_ptr<HitProcessCom> hit = obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    hit->SetValue(5);
}


//スタン弾生成
void BulletCreate::StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //弾丸オブジェクトを生成///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("stanball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //弾発射
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = -0.1 * power;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + bulletSpeed));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //弾
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);

    //判定用
    std::shared_ptr<HitProcessCom> hit= obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::STAN);
    hit->SetValue(5);
}

