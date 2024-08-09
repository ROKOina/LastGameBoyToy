#include "BulletCom.h"
#include "Components\ColliderCom.h"
#include "Components\TransformCom.h"
#include "Components\RendererCom.h"
#include "Components\Character\CharacterCom.h"
#include "Components\System\GameObject.h"
#include "Netwark/Photon/StaticSendDataManager.h"

void BulletCom::Update(float elapsedTime)
{
    std::shared_ptr<SphereColliderCom> collider = GetGameObject()->GetComponent<SphereColliderCom>();
    if (collider->OnHitGameObject().size())
    {
        //�l�b�g�ő��M
        for (auto& c : collider->OnHitGameObject())
        {
            int sendID = c.gameObject.lock()->GetComponent<CharacterCom>()->GetNetID();
            switch (hitType)
            {
            case HitProcessCom::HIT_TYPE::DAMAGE:
                StaticSendDataManager::Instance().SetSendDamage(ownerID, sendID, 1);
                break;
            case HitProcessCom::HIT_TYPE::HEAL:
                StaticSendDataManager::Instance().SetSendHeal(ownerID, sendID, 1);
                break;
            case HitProcessCom::HIT_TYPE::STAN:
                StaticSendDataManager::Instance().SetSendStan(ownerID, sendID, 2);
                break;
            default:
                break;
            }
        }
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
    //�e����
    EraseBullet(elapsedTime);
}

void BulletCom::EraseBullet(float elapsedTime)
{
    timer += elapsedTime;
    if (timer > aliveTime)
    {
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
}




//�_���[�W�e����
void DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("damageball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + bulletSpeed * power));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetBulletType(HitProcessCom::HIT_TYPE::DAMAGE);
    bulletCom->SetAliveTime(2.0f);
}


//�X�^���e����
void BulletCreate::StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("stanball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::BLACK), (BLENDSTATE::ALPHA));
    renderCom->LoadModel("Data/Ball/t.mdl");

    ///////////////////////////////


    //�e����
    std::shared_ptr<MovementCom> moveCom = obj->AddComponent<MovementCom>();
    float gravity = 10 - power * 9;
    moveCom->SetGravity(-gravity);
    moveCom->SetFriction(0.0f);
    moveCom->AddNonMaxSpeedForce(objPoint->transform_->GetWorldFront() * (20.0f + bulletSpeed * power));

    std::shared_ptr<SphereColliderCom> coll = obj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetBulletType(HitProcessCom::HIT_TYPE::STAN);
    bulletCom->SetAliveTime(2.0f);
}

