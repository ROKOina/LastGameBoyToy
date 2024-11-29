#include"Scene/SceneTraining/TrainingManager.h"

#include <Graphics\Graphics.h>
#include <Component\Camera\FreeCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Character\RegisterChara.h>
#include <Component\Collsion\ColliderCom.h>
#include <Component\Camera\EventCameraManager.h>
#include <Input\Input.h>
#include <Component\Character\CharacterCom.h>
#include <Component\Animation\AnimationCom.h>
#include <StateMachine\Behaviar\BossState.h>
#include <Component\System\SpawnCom.h>
#include <Component\Stage\StageEditorCom.h>
#include "Component\Phsix\RigidBodyCom.h"
#include <Component\Collsion\RayCollisionCom.h>
#include <Component\MoveSystem\EasingMoveCom.h>
#include "Component\Collsion\PushBackCom.h"
#include "Component\Enemy\BossCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"
#include <Component/System/HitProcessCom.h>
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include <Component/UI/UiSystem.h>
#include <Component/UI/PlayerUI.h>
#include <Component/UI/UiFlag.h>
#include <Component/UI/UiGauge.h>
#include "Component\Audio\AudioCom.h"
#include "Component/Enemy/ScarecrowCom.h"
#include "Component\Stage\GateGimmickCom.h"
#include <StateMachine\Behaviar\InazawaCharacterState.h>


TrainingManager::TrainingManager()
{
}
TrainingManager::~TrainingManager()
{
}
TrainingSystem::TrainingSystem()
{
}
TrainingSystem::~TrainingSystem()
{
}
TutorialSystem::TutorialSystem()
{
}
TutorialSystem::~TutorialSystem()
{
}



void TrainingManager::TrainingManagerStart()
{
    TrainingSystem::Instance().TrainingSystemStart();
    TutorialSystem::Instance().TutorialSystemStart();
}

void TrainingManager::TrainingManagerUpdate(float elapsedTime)
{
    if (tutorialFlag)
    {
        //チュートリアル
        TutorialSystem::Instance().TutorialSystemUpdate(elapsedTime);
    }
    else
    {
        //トレーニング
        TrainingSystem::Instance().TrainingSystemUpdate(elapsedTime);
    }
}

void TrainingManager::TrainingManagerClear()
{
    TutorialSystem::Instance().TutorialSystemClear();
    TrainingSystem::Instance().TrainingSystemClear();
}

void TrainingSystem::TrainingSystemStart()
{
    ////棒立ち案山子
    //{
    //    GameObjectManager::Instance().Find("scarecrow1")->GetComponent<AnimationCom>()->PlayAnimation(GameObjectManager::Instance().Find("scarecrow1")->GetComponent<AnimationCom>()->FindAnimation("Boss_idol"), true, false, 0.1f);
    //    GameObjectManager::Instance().Find("scarecrow2")->GetComponent<AnimationCom>()->PlayAnimation(GameObjectManager::Instance().Find("scarecrow2")->GetComponent<AnimationCom>()->FindAnimation("Boss_idol"), true, false, 0.1f);
    //}

        //棒立ち案山子君
    {
        auto& scarecrow1 = GameObjectManager::Instance().Create();
        scarecrow1->SetName("scarecrow1");
        std::shared_ptr<RendererCom> r = scarecrow1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow1->transform_->SetWorldPosition({ 21.004f,0.0f,-19.323 });
        scarecrow1->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow1->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow1->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        scarecrow1->AddComponent<AnimationCom>();
        auto& charaStatusCom = scarecrow1->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(100);
        charaStatusCom->SetMaxHitPoint(100);
        scarecrow1->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = scarecrow1->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);
    }

    {
        auto& scarecrow2 = GameObjectManager::Instance().Create();
        scarecrow2->SetName("scarecrow2");
        std::shared_ptr<RendererCom> r = scarecrow2->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow2->transform_->SetWorldPosition({ 9.915f,0.0f,-19.323f });
        scarecrow2->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow2->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow2->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        scarecrow2->AddComponent<AnimationCom>();
        auto& charaStatusCom = scarecrow2->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(100);
        charaStatusCom->SetMaxHitPoint(100);
        scarecrow2->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = scarecrow2->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);

    }


    {
        auto& scarecrow3 = GameObjectManager::Instance().Create();
        scarecrow3->SetName("scarecrow3");
        std::shared_ptr<RendererCom> r = scarecrow3->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow3->transform_->SetWorldPosition({ -10.0f,0.0f,-19.323f });
        scarecrow3->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow3->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow3->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        scarecrow3->AddComponent<AnimationCom>();
        scarecrow3->AddComponent<MovementCom>();
        scarecrow3->AddComponent<ScarecrowCom>();

        auto& charaStatusCom = scarecrow3->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(100);
        charaStatusCom->SetMaxHitPoint(100);
        scarecrow3->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = scarecrow3->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);

    }
}

void TrainingSystem::TrainingSystemUpdate(float elapsedTime)
{

}

void TrainingSystem::TrainingSystemClear()
{

}

void TutorialSystem::TutorialSystemStart()
{
    
   
}

void TutorialSystem::TutorialSystemUpdate(float elapsedTime)
{

}

void TutorialSystem::TutorialSystemClear()
{

}

