#include "BulletCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\System\GameObject.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Particle\CPUParticle.h"
#include <Component\MoveSystem\MovementCom.h>
#include "Component\Renderer\TrailCom.h"

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
        std::shared_ptr<GameObject> nonCharaObj;
        if (GetGameObject()->GetComponent<HitProcessCom>()->IsHit())
        {
            GameObjectManager::Instance().Remove(this->GetGameObject());
            GameObjectManager::Instance().Remove(viewBullet.lock());
        }
        else if (GetGameObject()->GetComponent<HitProcessCom>()->IsHitNonChara(nonCharaObj))
        {
            //ヒットエフェクト生成
            {
                std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();
                hiteffectobject->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());
                hiteffectobject->SetName("HitEffect");
                std::shared_ptr<GPUParticle>hiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hanabi.gpuparticle", 1000);
                hiteffct->Play();
            }
            {
                std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();
                hiteffectobject->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());
                hiteffectobject->SetName("HitSmokeEffect");
                std::shared_ptr<CPUParticle>hiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 1000);
                hiteffct->SetActive(true);
            }

            GameObjectManager::Instance().Remove(this->GetGameObject());
            GameObjectManager::Instance().Remove(viewBullet.lock());
        }
    }

    //銃弾と敵の攻撃との衝突処理
    BulletVSEnemyMissile();

    //弾消去
    EraseBullet(elapsedTime);
}

void BulletCom::EraseBullet(float elapsedTime)
{
    timer += elapsedTime;
    if (timer > aliveTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
        GameObjectManager::Instance().Remove(viewBullet.lock());
    }
}

//銃弾と敵の攻撃との衝突処理
void BulletCom::BulletVSEnemyMissile()
{
    const auto& collision = GetGameObject()->GetComponent<SphereColliderCom>();
    if (!collision) return;

    // "BOSS" オブジェクトへの参照を取得
    const auto& bossObject = GameObjectManager::Instance().Find("BOSS");
    std::vector<std::shared_ptr<GameObject>> objectsToRemove;

    for (const auto& hitobject : collision->OnHitGameObject())
    {
        const auto& gameObject = hitobject.gameObject.lock();
        // "BOSS" でないオブジェクトのみ削除リストに追加
        if (gameObject && gameObject != bossObject)
        {
            //爆破エフェクト再生
            GameObj obj = GameObjectManager::Instance().Create();
            obj->SetName("explosion");
            obj->transform_->SetWorldPosition(gameObject->transform_->GetWorldPosition());
            obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitfire.cpuparticle", 300);
            const auto& gpuparticle = obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hitexplosion.gpuparticle", 5000);
            gpuparticle->Play();

            objectsToRemove.push_back(gameObject);
        }
    }

    // 一括削除
    for (const auto& gameObject : objectsToRemove)
    {
        GameObjectManager::Instance().Remove(gameObject);
        GameObjectManager::Instance().Remove(this->GetGameObject());
        GameObjectManager::Instance().Remove(viewBullet.lock());
    }
}

//ダメージ弾生成
void BulletCreate::DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power, int damageValue)
{
    //弾丸オブジェクトを生成///////

    //発射位置算出用変数定義
    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();
    auto& cameraObj = GameObjectManager::Instance().Find("cameraPostPlayer");

    //見た目部分
    GameObj viewObj = GameObjectManager::Instance().Create();
    viewObj->SetName("damageballView");

    std::shared_ptr<Trail>trail = viewObj->AddComponent<Trail>("Data/SerializeData/TrailData/trajectory.trail");
    trail->SetTransform(viewObj->transform_->GetWorldTransform());

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    float ya = cameraObj->transform_->GetLocalPosition().y * objPoint->transform_->GetScale().y;
    firePos.y += ya;
    viewObj->transform_->SetWorldPosition(firePos);

    //弾発射
    std::shared_ptr<MovementCom> moveCom = viewObj->AddComponent<MovementCom>();
    float gravity = 0.98f - 0.95f * power;
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);

    //銃口から発射する
    auto& arm = cameraObj->GetChildFind("armChild");
    if (arm)
    {
        const auto& model= arm->GetComponent<RendererCom>()->GetModel();
        const auto& node = model->FindNode("gun2");

        DirectX::XMFLOAT3 gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };
        DirectX::XMFLOAT3 cameraPos = cameraObj->transform_->GetWorldPosition();

        //カメラの子供にする
        DirectX::XMFLOAT3 cameraFromGun = gunPos - cameraPos;
        DirectX::XMFLOAT3 fpsPos = fpsDir * 30;

        DirectX::XMFLOAT3 velo = fpsPos - cameraFromGun;
        moveCom->SetNonMaxSpeedVelocity(Mathf::Normalize(velo) * bulletSpeed);
        viewObj->transform_->SetWorldPosition(gunPos);
    }

    moveCom->SetIsRaycast(false);

    //パーティクル
    const auto& bulletgpuparticle = viewObj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerbullet.gpuparticle", 100);
    bulletgpuparticle->Play();
    std::shared_ptr<GameObject>bullettrajectory = viewObj->AddChildObject();
    std::shared_ptr<GPUParticle>bullettrajectoryparticle = bullettrajectory->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/trajectory.gpuparticle", 200);
    bullettrajectoryparticle->Play();


    //判定部分
    GameObj colObj = GameObjectManager::Instance().Create();
    colObj->SetName("damageball");

    colObj->transform_->SetWorldPosition(firePos);

    ///////////////////////////////

    //弾発射
    moveCom = colObj->AddComponent<MovementCom>();
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);
    moveCom->SetIsRaycast(false);

    std::shared_ptr<SphereColliderCom> coll = colObj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);
    coll->SetRadius(0.6f);

    //弾
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = colObj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);
    bulletCom->SetDamageValue(-damageValue);
    bulletCom->SetViewBullet(viewObj);

    //判定用
    std::shared_ptr<HitProcessCom> hit = colObj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    hit->SetValue(damageValue);
}

//スタン弾生成
void BulletCreate::StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power, int stanValue)
{
    //弾丸オブジェクトを生成///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });
    obj->SetName("stanball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS));
    renderCom->LoadModel("Data/Model/cube/cube.mdl");

    ///////////////////////////////

    //弾発射
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 0.98f - 0.95f * power;
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);
    moveCom->SetIsRaycast(false);

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
    bulletCom->SetDamageValue(0);

    //判定用
    std::shared_ptr<HitProcessCom> hit = obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::STAN);
    hit->SetValue(stanValue);
}

//ノックバック弾生成
void BulletCreate::KnockbackFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //弾丸オブジェクトを生成///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("knockbackball");
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS));
    renderCom->LoadModel("Data/Model/cube/cube.mdl");

    ///////////////////////////////

    //弾発射
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 0.98f - 0.95f * power;
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);
    moveCom->SetIsRaycast(false);

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
    bulletCom->SetDamageValue(0);

    //判定用
    std::shared_ptr<HitProcessCom> hit = obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::KNOCKBACK);
    DirectX::XMFLOAT3 startpos = { hit->GetGameObject()->transform_->GetWorldPosition() };
    DirectX::XMFLOAT3 knockVec = { 0,2,0 };
    hit->SetValue3(Mathf::Lerp(startpos, knockVec, 0.3f));
}