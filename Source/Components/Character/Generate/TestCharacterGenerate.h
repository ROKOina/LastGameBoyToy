#pragma once
#include "Components\System\GameObject.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\TransformCom.h"
#include "Components\AnimationCom.h"
#include "Components\MovementCom.h"
#include "Components\RendererCom.h"
#include "Components\ColliderCom.h"

static GameObj GenerateTestCharacter(int teamId, DirectX::XMFLOAT3 pos)
{
    GameObj obj = GameObjectManager::Instance().Create();
    
    std::string name = "player" + std::to_string(teamId);
    obj->SetName(name.c_str());
    obj->transform_->SetWorldPosition(pos);
    obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });

    std::shared_ptr<RendererCom> renderComp = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    renderComp->LoadModel("Data/OneCoin/robot.mdl");

    std::shared_ptr<AnimationCom> animeComp = obj->AddComponent<AnimationCom>();
    animeComp->PlayAnimation(0, true, false, 0.001f);

    std::shared_ptr<MovementCom> moveComp = obj->AddComponent<MovementCom>();
    std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();
    c->SetCharaID(teamId);

    std::shared_ptr<SphereColliderCom> sphere = obj->AddComponent<SphereColliderCom>();
    //ヒットスキャン用オブジェクト
    GameObj collision = GameObjectManager::Instance().Create();
    std::shared_ptr<CapsuleColliderCom> capsule = collision->AddComponent<CapsuleColliderCom>();
    
    //とりあえずIDによってコリジョンのチーム分け
    if (teamId % 2 == 0)
    {
        sphere->SetMyTag(COLLIDER_TAG::Player);
        sphere->SetJudgeTag(COLLIDER_TAG::EnemyAttack);
        capsule->SetMyTag(COLLIDER_TAG::PlayerAttack);
        capsule->SetJudgeTag(COLLIDER_TAG::Enemy);
    }
    else
    {
        sphere->SetMyTag(COLLIDER_TAG::Enemy);
        sphere->SetJudgeTag(COLLIDER_TAG::PlayerAttack);
        capsule->SetMyTag(COLLIDER_TAG::EnemyAttack);
        capsule->SetJudgeTag(COLLIDER_TAG::Player);
    }
    c->SetGunFireCollision(collision);

    return obj;
}