#include "RegisterChara.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component/Animation/AimIKCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\NodeCollsionCom.h"
#include "CharaStatusCom.h"
#include "InazawaCharacterCom.h"
#include "HaveAllAttackCharacter.h"
#include "Picohard.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Collsion/PushBackCom.h"
#include "Component/System/HitProcessCom.h"
#include "Component\System\SpawnCom.h"
#include "Component\Renderer\TrailCom.h"

void RegisterChara::SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject>& obj)
{
    switch (list)
    {
    case RegisterChara::CHARA_LIST::INAZAWA:
        InazawaChara(obj);
        break;
    case RegisterChara::CHARA_LIST::PICOHARD:
        PicohardChara(obj);
        break;
    case RegisterChara::CHARA_LIST::HAVE_ALL_ATTACK:
        HaveAllAttackChara(obj);
        break;
    default:
        break;
    }
}

void RegisterChara::InazawaChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/player_True/player.mdl");
    obj->AddComponent<AimIKCom>("spine2", nullptr);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>(nullptr);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();
    //HPの初期設定
    status->SetMaxHitPoint(200);
    status->SetHitPoint(status->GetMaxHitpoint());
    status->SetInvincibleTime(0.4f);
    std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
    c->SetCharaID(int(CHARA_LIST::INAZAWA));

    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

    auto& pushBack = obj->AddComponent<PushBackCom>();
    pushBack->SetRadius(0.5f);
    pushBack->SetWeight(1);

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }
}

void RegisterChara::HaveAllAttackChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine", nullptr);
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/Model/pico/pico.nodecollsion");
    obj->AddComponent<CharaStatusCom>();
    obj->AddComponent<MovementCom>();
    std::shared_ptr<HaveAllAttackCharaCom> c = obj->AddComponent<HaveAllAttackCharaCom>();
    c->SetCharaID(int(CHARA_LIST::HAVE_ALL_ATTACK));

    //煙のエフェクト
    {
        std::shared_ptr<GameObject> smoke = obj->AddChildObject();
        smoke->SetName("smokeeffect");
        std::shared_ptr<CPUParticle> smokeeffct = smoke->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/smoke.cpuparticle", 100);
        smokeeffct->SetActive(false);
    }

    //std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    //box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    //box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    //if (std::strcmp(obj->GetName(), "player") == 0)
    //    box->SetMyTag(COLLIDER_TAG::Player);
    //else
    //    box->SetMyTag(COLLIDER_TAG::Enemy);

    std::shared_ptr<CapsuleColliderCom> ca = obj->AddComponent<CapsuleColliderCom>();

    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        ca->SetMyTag(COLLIDER_TAG::Player);
        ca->SetJudgeTag(COLLIDER_TAG::Enemy);
    }
    else
    {
        ca->SetMyTag(COLLIDER_TAG::Enemy);
        ca->SetJudgeTag(COLLIDER_TAG::Player);
    }

    //std::shared_ptr<SphereColliderCom> sphere= obj->AddComponent<SphereColliderCom>();
    //sphere->SetRadius(2);
    //sphere->SetMyTag(COLLIDER_TAG::Player);
    //sphere->SetJudgeTag(COLLIDER_TAG::Enemy);

    //攻撃レイキャスト
    {
        std::shared_ptr<GameObject> rayChild = obj->AddChildObject();
        rayChild->SetName("rayObj");

        rayChild->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });

        std::shared_ptr<RayColliderCom> rayCol = rayChild->AddComponent<RayColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            rayCol->SetMyTag(COLLIDER_TAG::Enemy);
            rayCol->SetJudgeTag(COLLIDER_TAG::Player);
        }
        rayCol->SetEnabled(false);

        //ダメージ処理用
        std::shared_ptr<HitProcessCom> hitDamage = rayChild->AddComponent<HitProcessCom>(obj);
        hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
    }

    //回復カプセル
    {
        std::shared_ptr<GameObject> cupsuleChild = obj->AddChildObject();
        cupsuleChild->SetName("capsuleObj");

        cupsuleChild->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });

        std::shared_ptr<CapsuleColliderCom> capsuleCol = cupsuleChild->AddComponent<CapsuleColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            capsuleCol->SetMyTag(COLLIDER_TAG::Player);
            capsuleCol->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            capsuleCol->SetMyTag(COLLIDER_TAG::Enemy);
            capsuleCol->SetJudgeTag(COLLIDER_TAG::Player);
        }
        capsuleCol->SetEnabled(false);

        //ヒール処理用
        std::shared_ptr<HitProcessCom> hitHeal = cupsuleChild->AddComponent<HitProcessCom>(obj);
        hitHeal->SetHitType(HitProcessCom::HIT_TYPE::HEAL);
        hitHeal->SetValue(2);
    }
}

//ラインハルト
void RegisterChara::PicohardChara(std::shared_ptr<GameObject>& obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/Model/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine", nullptr);
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>("Data/Model/pico/pico.nodecollsion");
    a->PlayAnimation(0, true, false, 0.001f);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<PicohardCharaCom> c = obj->AddComponent<PicohardCharaCom>();
    c->SetCharaID(int(CHARA_LIST::PICOHARD));
    std::shared_ptr<CharaStatusCom> status = obj->AddComponent<CharaStatusCom>();

    std::shared_ptr<CapsuleColliderCom> ca = obj->AddComponent<CapsuleColliderCom>();
    if (std::strcmp(obj->GetName(), "player") == 0)
    {
        ca->SetMyTag(COLLIDER_TAG::Player);
        ca->SetJudgeTag(COLLIDER_TAG::Enemy);
    }
    else
    {
        ca->SetMyTag(COLLIDER_TAG::Enemy);
        ca->SetJudgeTag(COLLIDER_TAG::Player);
    }

    //hanma-
    {
        //ハンマー軸
        //X
        std::shared_ptr<GameObject> zikuX = obj->AddChildObject();
        zikuX->SetName("hammerZikuX");
        zikuX->transform_->SetLocalPosition({ 0,88.0f,0 });
        //Y
        std::shared_ptr<GameObject> zikuY = zikuX->AddChildObject();
        zikuY->SetName("hammerZikuY");

        std::shared_ptr<GameObject> hanma = zikuY->AddChildObject();
        hanma->SetName("hammer");
        hanma->transform_->SetLocalPosition({ 0,0,71.0f });

        auto& s = hanma->AddComponent<SphereColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            s->SetMyTag(COLLIDER_TAG::Player);
            s->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            s->SetMyTag(COLLIDER_TAG::Enemy);
            s->SetJudgeTag(COLLIDER_TAG::Player);
        }
    }

    //たて
    {
        //軸
        std::shared_ptr<GameObject> zikuX = obj->AddChildObject();
        zikuX->SetName("sieldZikuX");
        zikuX->transform_->SetWorldPosition({ 0,80,33 });

        std::shared_ptr<GameObject> tate = zikuX->AddChildObject();
        tate->SetName("sield");
        tate->transform_->SetLocalPosition({ 0,0,30 });
        tate->transform_->SetScale({ 3,2,0.1f });

        std::shared_ptr<RendererCom> tateRen = tate->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::ALPHA);
        tateRen->LoadModel("Data/Model/cube/cube.mdl");

        tate->AddComponent<NodeCollsionCom>("Data/Model/cube/sield.nodecollsion");

        std::shared_ptr<SphereColliderCom> s = tate->AddComponent<SphereColliderCom>();
        s->SetMyTag(COLLIDER_TAG::PlayerSield);
        s->SetJudgeTag(COLLIDER_TAG::Enemy);
    }

    //チャージ当たり判定
    {
        std::shared_ptr<GameObject> chargeCol = obj->AddChildObject();
        chargeCol->SetName("chargeCol");
        chargeCol->transform_->SetWorldPosition({ 0,80,33 });

        auto& sph = chargeCol->AddComponent<SphereColliderCom>();
        if (std::strcmp(obj->GetName(), "player") == 0)
        {
            sph->SetMyTag(COLLIDER_TAG::Player);
            sph->SetJudgeTag(COLLIDER_TAG::Enemy);
        }
        else
        {
            sph->SetMyTag(COLLIDER_TAG::Enemy);
            sph->SetJudgeTag(COLLIDER_TAG::Player);
        }

        sph->SetRadius(1);
        sph->SetEnabled(false);
    }
}