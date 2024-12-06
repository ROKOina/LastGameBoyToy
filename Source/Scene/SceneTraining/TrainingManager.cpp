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
#include "Component/Item/UltSkillMaxItem.h"


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


#pragma region トレーニング統括
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
#pragma endregion





#pragma region トレーニングモード
void TrainingSystem::TrainingSystemStart()
{
    //棒立ち案山子君
    {
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
            scarecrow1->AddComponent<MovementCom>();

            auto& scareCom = scarecrow1->AddComponent<ScarecrowCom>();
            scareCom->SetCrowMode(0);

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
            scarecrow2->AddComponent<MovementCom>();
            auto& scareCom = scarecrow2->AddComponent<ScarecrowCom>();
            scareCom->SetCrowMode(0);

            auto& charaStatusCom = scarecrow2->AddComponent<CharaStatusCom>();
            charaStatusCom->SetInvincibleTime(0.1f);
            charaStatusCom->SetHitPoint(100);
            charaStatusCom->SetMaxHitPoint(100);
            scarecrow2->AddComponent<AudioCom>();
            std::shared_ptr<PushBackCom>pushBack = scarecrow2->AddComponent<PushBackCom>();
            pushBack->SetRadius(1.5f);
            pushBack->SetWeight(600.0f);

        }
    }

    //移動案山子君
    {
        {
            auto& scarecrow3 = GameObjectManager::Instance().Create();
            scarecrow3->SetName("scarecrow3");
            std::shared_ptr<RendererCom> r = scarecrow3->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
            r->SetOutlineColor({ 1,0,0 });
            r->SetOutlineIntensity(10.0f);
            scarecrow3->transform_->SetWorldPosition({ -18.0f,0.0f,-19.323f });
            scarecrow3->transform_->SetScale({ 0.08f, 0.08f, 0.08f });
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

        {
            auto& scarecrow4 = GameObjectManager::Instance().Create();
            scarecrow4->SetName("scarecrow4");
            std::shared_ptr<RendererCom> r = scarecrow4->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
            r->SetOutlineColor({ 1,0,0 });
            r->SetOutlineIntensity(10.0f);
            scarecrow4->transform_->SetWorldPosition({ -18.0f,0.0f,-20.0f });
            scarecrow4->transform_->SetScale({ 0.08f, 0.08f, 0.08f });
            scarecrow4->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
            std::shared_ptr<SphereColliderCom> collider = scarecrow4->AddComponent<SphereColliderCom>();
            collider->SetMyTag(COLLIDER_TAG::Enemy);
            scarecrow4->AddComponent<AnimationCom>();
            scarecrow4->AddComponent<MovementCom>();
            scarecrow4->AddComponent<ScarecrowCom>();

            auto& charaStatusCom = scarecrow4->AddComponent<CharaStatusCom>();
            charaStatusCom->SetInvincibleTime(0.1f);
            charaStatusCom->SetHitPoint(100);
            charaStatusCom->SetMaxHitPoint(100);
            scarecrow4->AddComponent<AudioCom>();
            std::shared_ptr<PushBackCom>pushBack = scarecrow4->AddComponent<PushBackCom>();
            pushBack->SetRadius(1.5f);
            pushBack->SetWeight(600.0f);
        }
    }

    //射撃案内案山子君
    {
        auto& scarecrow5 = GameObjectManager::Instance().Create();
        scarecrow5->SetName("scarecrow5");
        std::shared_ptr<RendererCom> r = scarecrow5->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow5->transform_->SetWorldPosition({-2.656f, 0.0f, -46.567f});
        scarecrow5->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow5->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow5->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        scarecrow5->AddComponent<AnimationCom>();
        scarecrow5->AddComponent<MovementCom>();

        auto& scareCom = scarecrow5->AddComponent<ScarecrowCom>();
        scareCom->SetCrowMode(2);

        auto& charaStatusCom = scarecrow5->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(1);
        charaStatusCom->SetMaxHitPoint(1);
        scarecrow5->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = scarecrow5->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);
    }
    
    //ULTのアイテム
    {
        auto& scarecrow1 = GameObjectManager::Instance().Create();
        scarecrow1->SetName("ULTSKILLMAXITEM");
        std::shared_ptr<RendererCom> r = scarecrow1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Enemy/Enemy.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow1->transform_->SetWorldPosition({ 0.0f,0.0f,-2.0f });
        scarecrow1->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow1->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow1->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        collider->SetJudgeTag(COLLIDER_TAG::Player);
        collider->SetEnabled(true);
        collider->SetRadius(1.0f);
        scarecrow1->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/noobenemy.nodecollsion");
        scarecrow1->AddComponent<AnimationCom>();
        scarecrow1->AddComponent<MovementCom>();
        scarecrow1->AddComponent<UltSkillMaxItem>();



        scarecrow1->AddComponent<AudioCom>();

    }

}

//更新処理
void TrainingSystem::TrainingSystemUpdate(float elapsedTime)
{
    //射撃の開始処理と終了処理
    ShootingStartEndSystem();

    //アイテムスポーンシステム
    SpawnItemSystem(elapsedTime);

    if (shootingStartFlag)
    {
        ShootingSystem(elapsedTime);
    }
}

//クリア処理
void TrainingSystem::TrainingSystemClear()
{

}



//射撃時の案山子スポーン
void TrainingSystem::ShootingSpawnCrow()
{
    //射撃時の倒す案山子
    {
        DirectX::XMFLOAT3 randomPos;
        randomPos.x = Mathf::RandomRange(-23.737f, 19.528f);
        randomPos.y = 0.0f;
        randomPos.z = Mathf::RandomRange(-65.607f, -46.164f);

        auto& scarecrow5 = GameObjectManager::Instance().Create();
        scarecrow5->SetName("scarecrow");
        std::shared_ptr<RendererCom> r = scarecrow5->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow5->transform_->SetWorldPosition(randomPos);
        scarecrow5->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow5->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow5->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        scarecrow5->AddComponent<AnimationCom>();
        scarecrow5->AddComponent<MovementCom>();

        auto& scareCom = scarecrow5->AddComponent<ScarecrowCom>();
        scareCom->SetCrowMode(0);

        auto& charaStatusCom = scarecrow5->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(1);
        charaStatusCom->SetMaxHitPoint(1);
        scarecrow5->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = scarecrow5->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);
    }
}

//アイテムのスポーン
void TrainingSystem::SpawnItem()
{
    //ULTのアイテム
    {
        auto& scarecrow1 = GameObjectManager::Instance().Create();
        scarecrow1->SetName("ULTSKILLMAXITEM");
        std::shared_ptr<RendererCom> r = scarecrow1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Enemy/Enemy.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow1->transform_->SetWorldPosition({ 0.0f,0.0f,-2.0f });
        scarecrow1->transform_->SetScale({ 0.12f, 0.12f, 0.12f });
        scarecrow1->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow1->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        collider->SetJudgeTag(COLLIDER_TAG::Player);
        collider->SetEnabled(true);
        collider->SetRadius(1.0f);
        scarecrow1->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/noobenemy.nodecollsion");
        scarecrow1->AddComponent<AnimationCom>();
        scarecrow1->AddComponent<MovementCom>();
        scarecrow1->AddComponent<UltSkillMaxItem>();



        scarecrow1->AddComponent<AudioCom>();

    }
}

//射撃の開始処理と終了処理
void TrainingSystem::ShootingStartEndSystem()
{
    //開始処理
    if (GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->IsDeath()&&!shootingStartFlag)
    {
        shootingStartFlag = true;
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(false);
        ShootingSpawnCrow();
    }

    //終了処理
    if (scarecrowCount == scarecrowMaxTotal)
    {
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(true);
        GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->ReSpawn(1);
        GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));
        shootingStartFlag = false;
        scarecrowCount = 0;
    }
}

//射撃のロジック？
void TrainingSystem::ShootingSystem(float elapsdTime)
{
       
    //時間切れでリスポーン
    if (!shootingIntervalFlag)
    {
        scarecrowLifeTimer += elapsdTime;
        if (scarecrowLifeTime < scarecrowLifeTimer)
        {
            //案山子破棄
            GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));

            //案山子スポーン
            ShootingSpawnCrow();

            //初期化
            scarecrowLifeTimer = 0.0f;
            scarecrowCount += 1;
        }
    }
    //倒してリスポーン
    else if (!shootingIntervalFlag)
    {
        if (GameObjectManager::Instance().Find("scarecrow")->GetComponent<CharaStatusCom>()->IsDeath())
        {
            //案山子破棄
            GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));

            //初期化
            shootingIntervalFlag = true;
            scarecrowLifeTimer = 0.0f;
            shootingScore += 1;
            scarecrowCount += 1;
        }
    }

    //次の案山子スポーンまでの信号待ち
    if (shootingIntervalFlag)
    {
        scarecrowSpawnIntervalTimer += elapsdTime;
        if (scarecrowSpawnIntervalTime < scarecrowSpawnIntervalTimer)
        {   
            //案山子スポーン
            ShootingSpawnCrow();

            //初期化
            scarecrowSpawnIntervalTimer = 0.0f;
            shootingIntervalFlag = false;
        }
    }
    
}

//アイテムスポーンシステム
void TrainingSystem::SpawnItemSystem(float elapsdTime)
{
    if (!itemFlag)
    {
        spawnItemIntervalTimer += elapsdTime;
        if (spawnItemIntervalTimer > spawnItemIntervalTime)
        {
            spawnItemIntervalTimer = 0.0f;
            itemFlag = true;
            SpawnItem();
        }
    }
}
#pragma endregion





#pragma region チュートリアルモード
void TutorialSystem::TutorialSystemStart()
{
    
   
}

void TutorialSystem::TutorialSystemUpdate(float elapsedTime)
{
    TutorialManagerSystem(elapsedTime);
}

void TutorialSystem::TutorialSystemClear()
{

}

void TutorialSystem::TutorialManagerSystem(float elapsedTime)
{
    switch (tutorialID)
    {
    case TutorialID::MOVE:
        MoveTutorialManager(elapsedTime);
        break;
    case TutorialID::GUN:
        GunTutorialManager(elapsedTime);
        break;
    case TutorialID::SKILL:
        SkillTutorialManager(elapsedTime);
        break;
    case TutorialID::ULT:
        UltTutorialManager(elapsedTime);
        break;
    }
}

void TutorialSystem::NextTutorial(TutorialID id)
{
    tutorialID = id;
}

void TutorialSystem::MoveTutorialManager(float elapsedTime)
{
    if (moveInspectionFlag)
    {
        NextTutorial(TutorialID::GUN);
    }
}

void TutorialSystem::GunTutorialManager(float elapsedTime)
{
    if (gunInspectionFlag)
    {
        NextTutorial(TutorialID::SKILL);
    }
}

void TutorialSystem::SkillTutorialManager(float elapsedTime)
{
    if (skillInspectionFlag)
    {
        NextTutorial(TutorialID::ULT);
    }
}

void TutorialSystem::UltTutorialManager(float elapsedTime)
{
    if (ultInspectionFlag)
    {
        NextTutorial(TutorialID::END);
    }
}

#pragma endregion

