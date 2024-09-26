#include "RegisterChara.h"
#include "Components\System\GameObject.h"

#include "Components\TransformCom.h"
#include "Components\RendererCom.h"
#include "Components\AnimationCom.h"
#include "Components/AimIKCom.h"
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\NodeCollsionCom.h"

#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\HaveAllAttackCharacter.h"
#include "Components/Character/UenoCharacterCom.h"
#include "Components/EventCom.h"
#include "Components/Character/Picohard.h"
#include "HitProcess/HitProcessCom.h"

void RegisterChara::SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject> obj)
{
    switch (list)
    {
    case RegisterChara::CHARA_LIST::INAZAWA:
        InazawaChara(obj);
        break;
    case RegisterChara::CHARA_LIST::UENO:
        UenoChara(obj);
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

void RegisterChara::InazawaChara(std::shared_ptr<GameObject> obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine");
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>(nullptr);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
    c->SetCharaID(int(CHARA_LIST::INAZAWA));

    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);
}

void RegisterChara::HaveAllAttackChara(std::shared_ptr<GameObject> obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine");
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>(nullptr);
    a->PlayAnimation(0, true, false, 0.001f);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<HaveAllAttackCharaCom> c = obj->AddComponent<HaveAllAttackCharaCom>();
    c->SetCharaID(int(CHARA_LIST::HAVE_ALL_ATTACK));

    //std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    //box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    //box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    //if (std::strcmp(obj->GetName(), "player") == 0)
    //    box->SetMyTag(COLLIDER_TAG::Player);
    //else
    //    box->SetMyTag(COLLIDER_TAG::Enemy);

    std::shared_ptr<CapsuleColliderCom> ca = obj->AddComponent<CapsuleColliderCom>();
    ca->SetMyTag(COLLIDER_TAG::Player);
    ca->SetJudgeTag(COLLIDER_TAG::Enemy);

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

//上野キャラ
void RegisterChara::UenoChara(std::shared_ptr<GameObject> obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.015f, 0.015f, 0.015f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/PicoLabo/picolabo.mdl");
    std::shared_ptr<UenoCharacterCom> c = obj->AddComponent<UenoCharacterCom>();
    c->SetCharaID(int(CHARA_LIST::UENO));
    obj->AddComponent<AimIKCom>("Spine");
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<MovementCom>();
    obj->AddComponent<EventCom>(nullptr);
    obj->AddComponent<NodeCollsionCom>(nullptr);
    std::shared_ptr<BoxColliderCom> box = obj->AddComponent<BoxColliderCom>();
    box->SetSize(DirectX::XMFLOAT3(0.5f, 1.4f, 0.5f));
    box->SetOffsetPosition(DirectX::XMFLOAT3(0, 1.5f, 0));
    if (std::strcmp(obj->GetName(), "player") == 0)
        box->SetMyTag(COLLIDER_TAG::Player);
    else
        box->SetMyTag(COLLIDER_TAG::Enemy);

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
void RegisterChara::PicohardChara(std::shared_ptr<GameObject> obj)
{
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/pico/pico.mdl");
    obj->AddComponent<AimIKCom>("Spine");
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    obj->AddComponent<NodeCollsionCom>(nullptr);
    a->PlayAnimation(0, true, false, 0.001f);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<PicohardCharaCom> c = obj->AddComponent<PicohardCharaCom>();
    c->SetCharaID(int(CHARA_LIST::HAVE_ALL_ATTACK));

    std::shared_ptr<CapsuleColliderCom> ca = obj->AddComponent<CapsuleColliderCom>();
    ca->SetMyTag(COLLIDER_TAG::Player);
    ca->SetJudgeTag(COLLIDER_TAG::Enemy);

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
        s->SetMyTag(COLLIDER_TAG::Player);
        s->SetJudgeTag(COLLIDER_TAG::Enemy);

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
        tateRen->LoadModel("Data/cube/cube.mdl");

        tate->AddComponent<NodeCollsionCom>("Data/cube/sield.nodecollsion");

        std::shared_ptr<SphereColliderCom> s = tate->AddComponent<SphereColliderCom>();
        s->SetMyTag(COLLIDER_TAG::PlayerSield);
        s->SetJudgeTag(COLLIDER_TAG::Enemy);
    }
}
