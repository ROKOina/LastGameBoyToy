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
#include "Component\Phsix\RigidBodyCom.h"
#include "JankratBullet.h"

void BulletCom::Update(float elapsedTime)
{
    //std::shared_ptr<SphereColliderCom> collider = GetGameObject()->GetComponent<SphereColliderCom>();
    //if (collider->OnHitGameObject().size())
    //{
    //    //�l�b�g�ő��M
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

    //�q�b�g�v���Z�X������Ώ�������
    auto& hitProcessCom = GetGameObject()->GetComponent<HitProcessCom>();
    if (hitProcessCom)
    {
        std::shared_ptr<GameObject> nonCharaObj;
        if (GetGameObject()->GetComponent<HitProcessCom>()->IsHit())
        {
            //�q�b�g�G�t�F�N�g����
            std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();
            hiteffectobject->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());
            hiteffectobject->SetName("HitEffect");
            std::shared_ptr<GPUParticle>Chiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hanabi.gpuparticle", 1000);
            Chiteffct->Play();
            std::shared_ptr<CPUParticle>Ghiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 100);
            Ghiteffct->SetActive(true);

            GameObjectManager::Instance().Remove(this->GetGameObject());
            GameObjectManager::Instance().Remove(viewBullet.lock());
        }
        else if (GetGameObject()->GetComponent<HitProcessCom>()->IsHitNonChara(nonCharaObj))
        {
            //�q�b�g�G�t�F�N�g����
            std::shared_ptr<GameObject> hiteffectobject = GameObjectManager::Instance().Create();
            hiteffectobject->transform_->SetWorldPosition(GetGameObject()->transform_->GetWorldPosition());
            hiteffectobject->SetName("HitEffect");
            std::shared_ptr<GPUParticle>Chiteffct = hiteffectobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hanabi.gpuparticle", 1000);
            Chiteffct->Play();
            std::shared_ptr<CPUParticle>Ghiteffct = hiteffectobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitsmokeeffect.cpuparticle", 100);
            Ghiteffct->SetActive(true);

            GameObjectManager::Instance().Remove(this->GetGameObject());
            GameObjectManager::Instance().Remove(viewBullet.lock());
        }
    }

    //�e�e�ƓG�̍U���Ƃ̏Փˏ���
    BulletVSEnemyMissile();

    //�e����
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

//�e�e�ƓG�̍U���Ƃ̏Փˏ���
void BulletCom::BulletVSEnemyMissile()
{
    const auto& collision = GetGameObject()->GetComponent<SphereColliderCom>();
    if (!collision) return;

    std::vector<std::shared_ptr<GameObject>> objectsToRemove;

    for (const auto& hitobject : collision->OnHitGameObject())
    {
        const auto& gameObject = hitobject.gameObject.lock();
        // "BOSS" �łȂ��I�u�W�F�N�g�̂ݍ폜���X�g�ɒǉ�
        if (gameObject)
        {
            if (hitobject.gameObject.lock()->GetComponent<Collider>()->GetMyTag() == COLLIDER_TAG::EnemyBullet)
            {
                //���j�G�t�F�N�g�Đ�
                GameObj obj = GameObjectManager::Instance().Create();
                obj->SetName("explosion");
                obj->transform_->SetWorldPosition(gameObject->transform_->GetWorldPosition());
                obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/hitfire.cpuparticle", 300);
                const auto& gpuparticle = obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/hitexplosion.gpuparticle", 5000);
                gpuparticle->Play();

                objectsToRemove.push_back(gameObject);
            }
        }
    }

    // �ꊇ�폜
    for (const auto& gameObject : objectsToRemove)
    {
        GameObjectManager::Instance().Remove(gameObject);
        GameObjectManager::Instance().Remove(this->GetGameObject());
        GameObjectManager::Instance().Remove(viewBullet.lock());
    }
}

//�_���[�W�e����
void BulletCreate::DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power, int damageValue)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////

    //���ˈʒu�Z�o�p�ϐ���`
    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();
    auto& cameraObj = objPoint->GetChildFind("cameraPostPlayer");

    //�����ڕ���
    GameObj viewObj = GameObjectManager::Instance().Create();
    viewObj->SetName("damageballView");

    std::shared_ptr<Trail>trail = viewObj->AddComponent<Trail>("Data/SerializeData/TrailData/trajectory.trail");
    trail->SetTransform(viewObj->transform_->GetWorldTransform());

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    float ya;
    if (cameraObj)
        ya = cameraObj->transform_->GetLocalPosition().y * objPoint->transform_->GetScale().y;
    else
        ya = 1.5f;

    firePos.y += ya;
    viewObj->transform_->SetWorldPosition(firePos);

    //�e����
    std::shared_ptr<MovementCom> moveCom = viewObj->AddComponent<MovementCom>();
    float g = 5.98f;
    float gravity = g - (g - 0.3) * power;
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);

    //�e�����甭�˂���
    if (cameraObj)
    {
        auto& arm = cameraObj->GetChildFind("armChild");
        if (arm)
        {
            const auto& model = arm->GetComponent<RendererCom>()->GetModel();
            const auto& node = model->FindNode("gun2");

            DirectX::XMFLOAT3 gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };
            DirectX::XMFLOAT3 cameraPos = cameraObj->transform_->GetWorldPosition();

            //�J�����̎q���ɂ���
            DirectX::XMFLOAT3 cameraFromGun = gunPos - cameraPos;
            DirectX::XMFLOAT3 fpsPos = fpsDir * 60;

            DirectX::XMFLOAT3 velo = fpsPos - cameraFromGun;
            moveCom->SetNonMaxSpeedVelocity(Mathf::Normalize(velo) * bulletSpeed);
            viewObj->transform_->SetWorldPosition(gunPos);
        }
    }

    moveCom->SetIsRaycast(false);

    //�p�[�e�B�N��
    const auto& bulletgpuparticle = viewObj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerbullet.gpuparticle", 100);
    bulletgpuparticle->Play();
    std::shared_ptr<GameObject>bullettrajectory = viewObj->AddChildObject();
    std::shared_ptr<GPUParticle>bullettrajectoryparticle = bullettrajectory->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/trajectory.gpuparticle", 200);
    bullettrajectoryparticle->Play();

    //���蕔��
    GameObj colObj = GameObjectManager::Instance().Create();
    colObj->SetName("damageball");

    colObj->transform_->SetWorldPosition(firePos);

    ///////////////////////////////

    //�e����
    moveCom = colObj->AddComponent<MovementCom>();
    moveCom->SetGravity(gravity);
    moveCom->SetFriction(0.0f);

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);
    moveCom->SetIsRaycast(false);

    std::shared_ptr<SphereColliderCom> coll = colObj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyBullet);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);
    coll->SetRadius(0.6f);

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = colObj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(5.0f);
    bulletCom->SetDamageValue(-damageValue);
    bulletCom->SetViewBullet(viewObj);

    //����p
    std::shared_ptr<HitProcessCom> hit = colObj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    hit->SetValue(damageValue);
}

//�X�^���e����
void BulletCreate::StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power, int stanValue)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });
    obj->SetName("stanball");

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS));
    renderCom->LoadModel("Data/Model/cube/cube.mdl");

    ///////////////////////////////

    //�e����
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

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);
    bulletCom->SetDamageValue(0);

    //����p
    std::shared_ptr<HitProcessCom> hit = obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::STAN);
    hit->SetValue(stanValue);
}

//�m�b�N�o�b�N�e����
void BulletCreate::KnockbackFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power)
{
    //�e�ۃI�u�W�F�N�g�𐶐�///////
    GameObj obj = GameObjectManager::Instance().Create();
    obj->SetName("knockbackball");
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    firePos.y += 1.0f;
    obj->transform_->SetWorldPosition(firePos);

    std::shared_ptr<RendererCom> renderCom = obj->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS));
    renderCom->LoadModel("Data/Model/cube/cube.mdl");

    ///////////////////////////////

    //�e����
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

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = obj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(2.0f);
    bulletCom->SetDamageValue(0);

    //����p
    std::shared_ptr<HitProcessCom> hit = obj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::KNOCKBACK);
    DirectX::XMFLOAT3 startpos = { hit->GetGameObject()->transform_->GetWorldPosition() };
    DirectX::XMFLOAT3 knockVec = { 0,2,0 };
    hit->SetValue3(Mathf::Lerp(startpos, knockVec, 0.3f));
}

//�t�@���ʏ�e
void BulletCreate::FarahDamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed, float power, int damageValue)
{
    //���ˈʒu�Z�o�p�ϐ���`
    DirectX::XMFLOAT3 fpsDir = objPoint->GetComponent<CharacterCom>()->GetFpsCameraDir();
    auto& cameraObj = objPoint->GetChildFind("cameraPostPlayer");

    //�����ڕ���
    GameObj viewObj = GameObjectManager::Instance().Create();
    viewObj->SetName("damageballView");

    std::shared_ptr<Trail>trail = viewObj->AddComponent<Trail>("Data/SerializeData/TrailData/trajectory.trail");
    trail->SetTransform(viewObj->transform_->GetWorldTransform());

    DirectX::XMFLOAT3 firePos = objPoint->transform_->GetWorldPosition();
    float ya;
    if (cameraObj)
        ya = cameraObj->transform_->GetLocalPosition().y * objPoint->transform_->GetScale().y;
    else
        ya = 1.5f;

    firePos.y += ya;
    viewObj->transform_->SetWorldPosition(firePos);

    //�e����
    std::shared_ptr<MovementCom> moveCom = viewObj->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);
    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);

    //�e�����甭�˂���
    if (cameraObj)
    {
        auto& arm = cameraObj->GetChildFind("armChild");
        if (arm)
        {
            const auto& model = arm->GetComponent<RendererCom>()->GetModel();
            const auto& node = model->FindNode("gun2");

            DirectX::XMFLOAT3 gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };
            DirectX::XMFLOAT3 cameraPos = cameraObj->transform_->GetWorldPosition();

            //�J�����̎q���ɂ���
            DirectX::XMFLOAT3 cameraFromGun = gunPos - cameraPos;
            DirectX::XMFLOAT3 fpsPos = fpsDir * 60;

            DirectX::XMFLOAT3 velo = fpsPos - cameraFromGun;
            moveCom->SetNonMaxSpeedVelocity(Mathf::Normalize(velo) * bulletSpeed);
            viewObj->transform_->SetWorldPosition(gunPos);
        }
    }

    moveCom->SetIsRaycast(false);

    //�p�[�e�B�N��
    const auto& bulletgpuparticle = viewObj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerbullet.gpuparticle", 100);
    bulletgpuparticle->Play();
    std::shared_ptr<GameObject>bullettrajectory = viewObj->AddChildObject();
    std::shared_ptr<GPUParticle>bullettrajectoryparticle = bullettrajectory->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/trajectory.gpuparticle", 200);
    bullettrajectoryparticle->Play();

    //���蕔��
    GameObj colObj = GameObjectManager::Instance().Create();
    colObj->SetName("damageball");

    colObj->transform_->SetWorldPosition(firePos);

    ///////////////////////////////

    //�e����
    moveCom = colObj->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);

    moveCom->SetNonMaxSpeedVelocity(fpsDir * bulletSpeed);
    moveCom->SetIsRaycast(false);

    std::shared_ptr<SphereColliderCom> coll = colObj->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    if (std::strcmp(objPoint->GetName(), "player") == 0)
        coll->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyBullet);
    else
        coll->SetJudgeTag(COLLIDER_TAG::Player);
    coll->SetRadius(0.6f);

    //�e
    int netID = objPoint->GetComponent<CharacterCom>()->GetNetID();
    std::shared_ptr<BulletCom> bulletCom = colObj->AddComponent<BulletCom>(netID);
    bulletCom->SetAliveTime(5.0f);
    bulletCom->SetDamageValue(-damageValue);
    bulletCom->SetViewBullet(viewObj);

    //����p
    std::shared_ptr<HitProcessCom> hit = colObj->AddComponent<HitProcessCom>(objPoint);
    hit->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    hit->SetValue(damageValue);
}

GameObj BulletCreate::JankratBombFire(std::shared_ptr<GameObject> parent, DirectX::XMFLOAT3 pos, int id)
{
    GameObj bullet = GameObjectManager::Instance().Create();
    bullet->SetName("damageball");
    bullet->transform_->SetScale({ 0.01f,0.01f,0.01f });
    bullet->transform_->SetWorldPosition({ pos.x,pos.y,pos.z });

    //����
    RigidBodyCom* rigid = bullet->AddComponent<RigidBodyCom>(false, PhysXLib::ShapeType::Sphere).get();
    RendererCom* r = bullet->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS)).get();
    r->LoadModel("Data/Model/Jankrat/mine.mdl");

    //�R���C�_�[
    std::shared_ptr<SphereColliderCom> coll = bullet->AddComponent<SphereColliderCom>();
    coll->SetMyTag(COLLIDER_TAG::Bullet);
    coll->SetJudgeTag(COLLIDER_TAG::Enemy | COLLIDER_TAG::EnemyBullet);
    coll->SetRadius(1.0f);

    //�e
    std::shared_ptr<JankratBulletCom> bulletCom = bullet->AddComponent<JankratBulletCom>();

    //����p
    std::shared_ptr<HitProcessCom> hit = bullet->AddComponent<HitProcessCom>(parent);
    hit->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    hit->SetValue(1);

    //RigidBody��AddForce���������Ɏg���Ȃ��̂ł����ŕԂ�
    return bullet;
}

GameObj BulletCreate::JankratMineFire(std::shared_ptr<GameObject> parent, DirectX::XMFLOAT3 pos, float force, float damage, int id)
{
    //���ˈʒu�Z�o�p�ϐ���`
    DirectX::XMFLOAT3 fpsDir = parent->GetComponent<CharacterCom>()->GetFpsCameraDir();

    //�e�ۃI�u�W�F�N�g����
    GameObj bullet = GameObjectManager::Instance().Create();
    bullet->SetName("damageball");
    bullet->transform_->SetScale({ 0.01f,0.01f,0.01f });
    bullet->transform_->SetWorldPosition({ pos.x,pos.y,pos.z });

    //���x�ݒ�
    std::shared_ptr<MovementCom> moveCom = bullet->AddComponent<MovementCom>();
    moveCom->SetGravity(0.0f);
    moveCom->SetFriction(0.0f);
    moveCom->SetNonMaxSpeedVelocity(fpsDir * force);

    //Render
    std::shared_ptr<RendererCom> renderCom = bullet->AddComponent<RendererCom>((SHADER_ID_MODEL::DEFERRED), (BLENDSTATE::MULTIPLERENDERTARGETS));
    renderCom->LoadModel("Data/Model/Jankrat/mine.mdl");

    //�n���̃R���|�[�l���g����ĕt����



    return bullet;
}
