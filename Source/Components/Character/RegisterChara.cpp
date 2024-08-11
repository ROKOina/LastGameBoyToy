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

#include "HitProcess/HitProcessCom.h"

void RegisterChara::SetCharaComponet(CHARA_LIST list, std::shared_ptr<GameObject> obj)
{
	switch (list)
	{
	case RegisterChara::CHARA_LIST::INAZAWA:
		InazawaChara(obj);
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
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>("Spine");
    obj->AddComponent<NodeCollsionCom>("Data//pico//pico.nodecollsion");
    a->PlayAnimation(0, true, false, 0.001f);
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
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>("Spine");
    obj->AddComponent<NodeCollsionCom>("Data//pico//pico.nodecollsion");
    a->PlayAnimation(0, true, false, 0.001f);
    std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
    std::shared_ptr<HaveAllAttackCharaCom> c = obj->AddComponent<HaveAllAttackCharaCom>();
    c->SetCharaID(int(CHARA_LIST::HAVE_ALL_ATTACK));

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
