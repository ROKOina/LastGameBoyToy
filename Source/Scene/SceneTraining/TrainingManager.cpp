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
#include "Component\UI\Font.h"

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


#pragma region �g���[�j���O����
void TrainingManager::TrainingManagerStart()
{
    TrainingSystem::Instance().TrainingSystemStart();
    TutorialSystem::Instance().TutorialSystemStart();
}

void TrainingManager::TrainingManagerUpdate(float elapsedTime)
{
    if (tutorialFlag)
    {
        //�`���[�g���A��
        TutorialSystem::Instance().TutorialSystemUpdate(elapsedTime);
    }
    else
    {
        //�g���[�j���O
        TrainingSystem::Instance().TrainingSystemUpdate(elapsedTime);
    }
}

void TrainingManager::TrainingManagerClear()
{
    TutorialSystem::Instance().TutorialSystemClear();
    TrainingSystem::Instance().TrainingSystemClear();
}

void TrainingManager::ChangeTutorialFlag()
{
    GameObjectManager::Instance().Find("player")->transform_->SetWorldPosition({ -0.115f,0.0f,3.489f });
    GameObjectManager::Instance().Find("player")->transform_->SetEulerRotation({0.0f,180.119f,0.0f});
    TrainingSystem::Instance().TrainingObjUnhide();
    TutorialSystem::Instance().TutorialUIUnhind();
    
    tutorialFlag = true;
}

void TrainingManager::ChangeTrainigFlag()
{
   
    TutorialSystem::Instance().TutorialFlagClear();
    TutorialSystem::Instance().TutorialUIDisplay();
    tutorialFlag = false;
}

void TrainingManager::Changeblackout()
{
    //�Ó]
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);
    
}

void TrainingManager::Changelightchange()
{
    //�Ó]
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(1.4f, 7.0f, parameters);
}

void TrainingManager::OnGUI()
{
    TutorialSystem::Instance().OnGui();
}

#pragma endregion





#pragma region �g���[�j���O���[�h
void TrainingSystem::TrainingSystemStart()
{
    //�_�����ĎR�q�N
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

    //�ړ��ĎR�q�N
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

    //�ˌ��ē��ĎR�q�N
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
    
    //ULT�̃A�C�e��
    {
        auto& scarecrow1 = GameObjectManager::Instance().Create();
        scarecrow1->SetName("ULTSKILLMAXITEM");
        std::shared_ptr<RendererCom> r = scarecrow1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Item/item.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow1->transform_->SetWorldPosition({ 0.0f,1.0f,-2.0f });
        scarecrow1->transform_->SetEulerRotation({ 0.0f,0.0f,30.0f });
        scarecrow1->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        scarecrow1->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/UltSkillMaxItem.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = scarecrow1->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        collider->SetJudgeTag(COLLIDER_TAG::Player);
        collider->SetEnabled(true);
        collider->SetRadius(1.0f);

        scarecrow1->AddComponent<NodeCollsionCom>(nullptr);
        scarecrow1->AddComponent<AnimationCom>();
        scarecrow1->AddComponent<UltSkillMaxItem>();
        scarecrow1->AddComponent<AudioCom>();

    }

}

//�X�V����
void TrainingSystem::TrainingSystemUpdate(float elapsedTime)
{
    //�ˌ��̊J�n�����ƏI������
    ShootingStartEndSystem();

    //�A�C�e���X�|�[���V�X�e��
    SpawnItemSystem(elapsedTime);

    if (shootingStartFlag)
    {
        ShootingSystem(elapsedTime);
    }
}

//�N���A����
void TrainingSystem::TrainingSystemClear()
{

}



//�ˌ����̈ĎR�q�X�|�[��
void TrainingSystem::ShootingSpawnCrow()
{
    //�ˌ����̓|���ĎR�q
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

//�A�C�e���̃X�|�[��
void TrainingSystem::SpawnItem()
{
    //ULT�̃A�C�e��
    {
        auto& scarecrow1 = GameObjectManager::Instance().Create();
        scarecrow1->SetName("ULTSKILLMAXITEM");
        std::shared_ptr<RendererCom> r = scarecrow1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Item/item.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        scarecrow1->transform_->SetWorldPosition({ 0.0f,1.0f,-2.0f });
        scarecrow1->transform_->SetEulerRotation({ 0.0f,0.0f,30.0f });
        scarecrow1->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        
        std::shared_ptr<SphereColliderCom> collider = scarecrow1->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        collider->SetJudgeTag(COLLIDER_TAG::Player);
        collider->SetEnabled(true);
        collider->SetRadius(1.0f);

        scarecrow1->AddComponent<NodeCollsionCom>("Data/SerializeData/NodeCollsionData/UltSkillMaxItem.nodecollsion");
        scarecrow1->AddComponent<AnimationCom>();
        scarecrow1->AddComponent<UltSkillMaxItem>();
        scarecrow1->AddComponent<AudioCom>();

    }
}

//�ˌ��̊J�n�����ƏI������
void TrainingSystem::ShootingStartEndSystem()
{
    //�J�n����
    if (GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->IsDeath()&&!shootingStartFlag)
    {
       /* shootingStartFlag = true;
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(false);
        ShootingSpawnCrow();*/
        TrainingManager::Instance().ChangeTutorialFlag();
    }

    //�I������
    if (scarecrowCount == scarecrowMaxTotal)
    {
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(true);
        GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->ReSpawn(1);
        GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));
        shootingStartFlag = false;
        scarecrowCount = 0;
    }
}

//�ˌ��̃��W�b�N�H
void TrainingSystem::ShootingSystem(float elapsdTime)
{
       
    //���Ԑ؂�Ń��X�|�[��
    if (!shootingIntervalFlag)
    {
        scarecrowLifeTimer += elapsdTime;
        if (scarecrowLifeTime < scarecrowLifeTimer)
        {
            //�ĎR�q�j��
            GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));

            //�ĎR�q�X�|�[��
            ShootingSpawnCrow();

            //������
            scarecrowLifeTimer = 0.0f;
            scarecrowCount += 1;
        }
    }
    //�|���ă��X�|�[��
    else if (!shootingIntervalFlag)
    {
        if (GameObjectManager::Instance().Find("scarecrow")->GetComponent<CharaStatusCom>()->IsDeath())
        {
            //�ĎR�q�j��
            GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));

            //������
            shootingIntervalFlag = true;
            scarecrowLifeTimer = 0.0f;
            shootingScore += 1;
            scarecrowCount += 1;
        }
    }

    //���̈ĎR�q�X�|�[���܂ł̐M���҂�
    if (shootingIntervalFlag)
    {
        scarecrowSpawnIntervalTimer += elapsdTime;
        if (scarecrowSpawnIntervalTime < scarecrowSpawnIntervalTimer)
        {   
            //�ĎR�q�X�|�[��
            ShootingSpawnCrow();

            //������
            scarecrowSpawnIntervalTimer = 0.0f;
            shootingIntervalFlag = false;
        }
    }
    
}

//�A�C�e���X�|�[���V�X�e��
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

//�g���[�j���O���[�h�̃A�C�e����\��
void TrainingSystem::TrainingObjUnhide()
{
    GameObjectManager::Instance().Find("scarecrow1")->SetEnabled(false);
    GameObjectManager::Instance().Find("scarecrow2")->SetEnabled(false);
    GameObjectManager::Instance().Find("scarecrow3")->SetEnabled(false);
    GameObjectManager::Instance().Find("scarecrow4")->SetEnabled(false);
    GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(false);

    if (GameObjectManager::Instance().Find("ULTSKILLMAXITEM") != nullptr)
    {
        GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->SetEnabled(false);
    }
}


void TrainingSystem::TrainingObjDisplay()
{
    GameObjectManager::Instance().Find("scarecrow1")->SetEnabled(true);
    GameObjectManager::Instance().Find("scarecrow2")->SetEnabled(true);
    GameObjectManager::Instance().Find("scarecrow3")->SetEnabled(true);
    GameObjectManager::Instance().Find("scarecrow4")->SetEnabled(true);
    GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(true);
    GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->ReSpawn(1);

    if (GameObjectManager::Instance().Find("ULTSKILLMAXITEM") != nullptr)
    {
        GameObjectManager::Instance().Find("ULTSKILLMAXITEM")->SetEnabled(true);
    }
}
#pragma endregion





#pragma region �`���[�g���A�����[�h
void TutorialSystem::TutorialSystemStart()
{
    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testFont");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 0,0 };
        font->str = L"";  //L�t���Ă�
        font->scale = 1.0f;
        font->color.w = 0.0f;
        font->SetEnabled(false);
    }

#pragma region �ړ�

    moveSubTitle[0].str= L"�悤�����@�`���[�g���A����";
    moveSubTitle[0].pos = {650.0f,80.0f};
    moveSubTitle[0].UIID = 0;

    moveSubTitle[1].str = L"�������{�I�ȑ�����@��������̂�";
    moveSubTitle[1].pos = {553.0f,80.0f};
    moveSubTitle[1].UIID = 1;

    moveSubTitle[2].str = L"�܂��́A�ړ�����Ȃ̂�";
    moveSubTitle[2].pos = { 701.0f,80.0f };
    moveSubTitle[2].UIID = 2;

    moveSubTitle[3].str = L"�L�[�{�[�h��AD�L�[�ō��E�ړ��Ȃ̂�";
    moveSubTitle[3].pos = { 539.0f,80.0f };
    moveSubTitle[3].UIID = 3;

    moveSubTitle[4].str = L"�悭�ł����̂�";
    moveSubTitle[4].pos = { 806.0f,80.0f };
    moveSubTitle[4].UIID = 4;

    moveSubTitle[5].str = L"���͏c�ړ��Ȃ̂�";
    moveSubTitle[5].pos = { 777.0f,80.0f };
    moveSubTitle[5].UIID = 5;

    moveSubTitle[6].str = L"�L�[�{�[�h��WS�L�[�ŏc�ړ��Ȃ̂�";
    moveSubTitle[6].pos = { 556.0f,80.0f };
    moveSubTitle[6].UIID = 6;

    moveSubTitle[7].str = L"�f���炵���̂�";
    moveSubTitle[7].pos = { 809.0f,80.0f };
    moveSubTitle[7].UIID = 7;

    moveSubTitle[8].str = L"���̓W�����v�Ȃ̂�";
    moveSubTitle[8].pos = { 740.0f,80.0f };
    moveSubTitle[8].UIID = 8;

    moveSubTitle[9].str = L"�L�[�{�[�h�̃X�y�[�X�L�[�ŃW�����v�Ȃ̂�";
    moveSubTitle[9].pos = { 493.0f,80.0f };
    moveSubTitle[9].UIID = 9;

    moveSubTitle[10].str = L"�V�˂Ȃ̂�";
    moveSubTitle[10].pos = { 834.0f,80.0f };
    moveSubTitle[10].UIID = 10;

    moveSubTitle[11].str = L"���͏e�̐����Ȃ̂�";
    moveSubTitle[11].pos = { 715.0f,80.0f };
    moveSubTitle[11].UIID = 11;


#pragma endregion
#pragma region �e�Z���t

    gunSubTitle[0].str = L"�e�̊�{������������̂�";
    gunSubTitle[0].pos = {674.0f,80.0f};
    gunSubTitle[0].UIID = 0;

    gunSubTitle[1].str = L"�}�E�X�̍��N���b�N�ŁA���˂Ȃ̂�";
    gunSubTitle[1].pos = { 571.0f,80.0f };
    gunSubTitle[1].UIID=1;

    gunSubTitle[2].str = L"�悭�ł����̂�";
    gunSubTitle[2].pos = { 765.0f,80.0f };
    gunSubTitle[2].UIID =2;

    gunSubTitle[3].str = L"���̓X�L���̐����Ȃ̂�";
    gunSubTitle[3].pos = { 693.0f,80.0f};
    gunSubTitle[3].UIID=3;

#pragma endregion
#pragma region �X�L���Z���t

    skillSubTitle[0].str = L"�X�L���̊�{�����������̂�";
    skillSubTitle[0].pos = { 624.0f,80.0f };
    skillSubTitle[0].UIID = 0;

    skillSubTitle[1].str = L"�L�[�{�[�h��E�L�[�Ŕ����Ȃ̂�";
    skillSubTitle[1].pos = { 621.0f,80.0f };
    skillSubTitle[1].UIID =1;

    skillSubTitle[2].str = L"�f���炵���̂�";
    skillSubTitle[2].pos = { 780.0f,80.0f };
    skillSubTitle[2].UIID = 2;

    skillSubTitle[3].str = L"���̓E���g�̐����Ȃ̂�";
    skillSubTitle[3].pos = { 714.0f,80.0f };
    skillSubTitle[3].UIID=3;

#pragma endregion
#pragma region �E���g�Z���t

    ultSubTitle[0].str = L"�E���g�̊�{�����������̂�";
    ultSubTitle[0].pos = { 628.0f,80.0f};
    ultSubTitle[0].UIID = 0;

    ultSubTitle[1].str = L"�L�[�{�[�h��R�L�[�Ŕ����Ȃ̂�";
    ultSubTitle[1].pos = { 628.0f,80.0f};
    ultSubTitle[1].UIID = 1;

    ultSubTitle[2].str = L"�V�˂Ȃ̂�";
    ultSubTitle[2].pos = { 827.0f,80.0f};
    ultSubTitle[2].UIID = 2;

    ultSubTitle[3].str = L"����Ń`���[�g���A���͏I���Ȃ̂�";
    ultSubTitle[3].pos = { 543.0f,80.0f};
    ultSubTitle[3].UIID = 3;

    ultSubTitle[4].str = L"Les�ft enjoy�Ȃ̂�";
    ultSubTitle[4].pos = { 742.0f,80.0f};
    ultSubTitle[4].UIID=5;

#pragma endregion

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
    case TutorialID::BLACK:
        BlackOutManager(elapsedTime);
        break;
    case TutorialID::LIGHT:
        LightChangeManger(elapsedTime);
        break;
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
    case TutorialID::ENDBLACK:
        EndBlackTutorialManager(elapsedTime);
        break;
    case TutorialID::END:
        EndTutorialManager(elapsedTime);
        break;
    }
}

void TutorialSystem::NextTutorial(TutorialID id)
{
    tutorialID = id;
}


//�Ó]
void TutorialSystem::BlackOutManager(float elapsedTime)
{
    if (!flag)
    {
        TrainingManager::Instance().Changeblackout();
        flag = true;
    }

    if (flag)
    {
        blackTimer += elapsedTime;
    }

    if (blackTime<blackTimer)
    {
        NextTutorial(TutorialID::LIGHT);
        blackTimer = 0.0f;
        flag = false;
    }
}

//���]
void TutorialSystem::LightChangeManger(float elapsedTime)
{
    if (!flag)
    {
        TrainingManager::Instance().Changelightchange();
        GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->SetEnabled(true);
        flag = true;
    }
    
    if (flag)
    {
        GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;
    }

    if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w<1.0f)
    {
        NextTutorial(TutorialID::MOVE);
        GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
        upFlag = true;
        flag = false;
    }

}

//�ړ��̃��W�b�N
void TutorialSystem::MoveTutorialManager(float elapsedTime)
{
    


    InputVec = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetLeftStick();

    if (moveSubTitleIndex != 3 && moveSubTitleIndex != 6 && moveSubTitleIndex != 9)
    {
        if (upFlag)
        {
            //�^�C�}�[�X�V
            moveSubTitle[moveSubTitleIndex].subtitleTimer += elapsedTime;
            if (moveSubTitle[moveSubTitleIndex].subtitleTimer > moveSubTitle[moveSubTitleIndex].subtitleTime)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    if (moveSubTitleIndex == 11)
                    {
                        moveInspectionFlag = true;
                        upFlag = false;
                        downFlag = true;
                    }
                    else
                    {
                        moveSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                    }
                }
            }
        }
        else if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }


    }
    //���ړ�
    else if(moveSubTitleIndex==3)
    {
        if (InputVec.x == 1.0f)
        {
            moveDFlag = true;
        }

        if (InputVec.x == -1.0f)
        {
            moveAFlag = true;
        }



        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }

        if (moveAFlag && moveDFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    moveSubTitleIndex += 1;
                }

            }
        }

    }
    //�c�ړ�
    else if (moveSubTitleIndex == 6)
    {
        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }

        if (InputVec.y == 1.0f)
        {
            moveWFlag = true;
        }

        if (InputVec.y == -1.0f)
        {
            moveSFlag = true;
        }



        if (moveWFlag && moveSFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    moveSubTitleIndex += 1;
                }

            }
        }
    }
    //�W�����v
    else if (moveSubTitleIndex == 9)
    {
        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }


        if (CharacterInput::JumpButton_SPACE & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonDown())
        {
            jumpFlag = true;
        }

        if (jumpFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    moveSubTitleIndex += 1;
                }

            }
        }
    }


    if (moveInspectionFlag)
    {
        NextTutorial(TutorialID::GUN);
    }

    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->str = moveSubTitle[moveSubTitleIndex].str;
    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->position = moveSubTitle[moveSubTitleIndex].pos;
}

void TutorialSystem::GunTutorialManager(float elapsedTime)
{
    


    if (gunSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //�^�C�}�[�X�V
            gunSubTitle[gunSubTitleIndex].subtitleTimer += elapsedTime;
            if (gunSubTitle[gunSubTitleIndex].subtitleTimer > gunSubTitle[gunSubTitleIndex].subtitleTime)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    if (gunSubTitleIndex == 3)
                    {
                        gunInspectionFlag = true;
                        upFlag = false;
                        downFlag = true;
                    }
                    else
                    {
                        gunSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                    }
                }
            }
        }
        else if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }
    }
    else if (gunSubTitleIndex==1)
    {
        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }

        //�U���I���������U������
        if (CharacterInput::MainAttackButton & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            gunFlag = true;
        }

        if (gunFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    gunSubTitleIndex += 1;
                }

            }
        }

           
    }



    if (gunInspectionFlag)
    {
        NextTutorial(TutorialID::SKILL);
    }

    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->str = gunSubTitle[gunSubTitleIndex].str;
    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->position = gunSubTitle[gunSubTitleIndex].pos;

}

void TutorialSystem::SkillTutorialManager(float elapsedTime)
{


    if (skillSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //�^�C�}�[�X�V
            skillSubTitle[skillSubTitleIndex].subtitleTimer += elapsedTime;
            if (skillSubTitle[skillSubTitleIndex].subtitleTimer > skillSubTitle[skillSubTitleIndex].subtitleTime)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    if (skillSubTitleIndex == 3)
                    {
                        skillInspectionFlag = true;
                        upFlag = false;
                        downFlag = true;
                    }
                    else
                    {
                        skillSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                    }
                }
            }
        }
        else if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }
    }
    else if (skillSubTitleIndex == 1)
    {
        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }

        //�U���I���������U������
        if (CharacterInput::MainSkillButton_E & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            skillFlag = true;
        }

        if (skillFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    skillSubTitleIndex += 1;
                }

            }
        }
          
    }




    if (skillInspectionFlag)
    {
        NextTutorial(TutorialID::ULT);
    }

    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->str = skillSubTitle[skillSubTitleIndex].str;
    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->position = skillSubTitle[skillSubTitleIndex].pos;

}

void TutorialSystem::UltTutorialManager(float elapsedTime)
{



    if (ultSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //�^�C�}�[�X�V
            ultSubTitle[ultSubTitleIndex].subtitleTimer += elapsedTime;
            if (ultSubTitle[ultSubTitleIndex].subtitleTimer > ultSubTitle[ultSubTitleIndex].subtitleTime)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    if (ultSubTitleIndex == 4)
                    {
                        ultInspectionFlag = true;
                        upFlag = false;
                        downFlag = true;
                    }
                    else
                    {
                        ultSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                    }
                }
            }
        }
        else if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }
    }
    else if (ultSubTitleIndex == 1)
    {

        if (downFlag)
        {
            GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w += elapsedTime;

            if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w > 1.0f)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 1.0f;
                downFlag = false;
                upFlag = true;
            }
        }

        //�U���I���������U������
        if (CharacterInput::UltimetButton & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            ultFlag = true;
           
        }

        if (ultFlag)
        {
            if (upFlag)
            {
                GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w -= elapsedTime;

                if (GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w < 0.0)
                {
                    upFlag = false;
                    downFlag = true;
                    ultSubTitleIndex += 1;
                }

            }
        }

        
    }

    if (ultInspectionFlag)
    {
        NextTutorial(TutorialID::ENDBLACK);
    }

    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->str = ultSubTitle[ultSubTitleIndex].str;
    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->position = ultSubTitle[ultSubTitleIndex].pos;


}

void TutorialSystem::EndBlackTutorialManager(float elapsedTime)
{
    if (!flag)
    {
        TrainingManager::Instance().Changeblackout();
        flag = true;
    }

    if (flag)
    {
        blackTimer += elapsedTime;
    }

    if (blackTime < blackTimer)
    {
        TrainingSystem::Instance().TrainingObjDisplay();
        blackTimer = 0.0f;
        flag = false;
        NextTutorial(TutorialID::END);
    }
}

void TutorialSystem::EndTutorialManager(float elapsedTime)
{
    if (!flag)
    {
        TrainingManager::Instance().Changelightchange();

        flag = true;
    }

    if (flag)
    {
        TrainingManager::Instance().ChangeTrainigFlag();
    }
}

void TutorialSystem::TutorialFlagClear()
{
    tutorialID = 0;

    moveSubTitleIndex = 0;
    gunSubTitleIndex = 0;
    skillSubTitleIndex = 0;
    ultSubTitleIndex = 0;

    lightTimer = 0.0f;

    blackTimer = 0.0f;

    moveInspectionFlag = false;         //�����̃`���[�g���A������
    gunInspectionFlag = false;          //�e�̃`���[�g���A������
    skillInspectionFlag = false;        //�X�L���̃`���[�g���A������
    ultInspectionFlag = false;          //�E���g�̃`���[�g���A������

    flag = false;

    upFlag = false;
    downFlag = false;
    jumpFlag = false;
    gunFlag = false;
    skillFlag = false;
    ultFlag = false;

    moveAFlag = false;
    moveDFlag = false;
    moveWFlag = false;
    moveSFlag = false;
    moveJumpFlag = false;


    // moveSubTitle �̃��Z�b�g
    for (auto& subtitle : moveSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // gunSubTitle �̃��Z�b�g
    for (auto& subtitle : gunSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // skillSubTitle �̃��Z�b�g
    for (auto& subtitle : skillSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // ultSubTitle �̃��Z�b�g
    for (auto& subtitle : ultSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

}

void TutorialSystem::TutorialUIUnhind()
{
    GameObjectManager::Instance().Find("reticle")->SetEnabled(false);
    GameObjectManager::Instance().Find("HpFrame")->SetEnabled(false);
    GameObjectManager::Instance().Find("HpGauge")->SetEnabled(false);
    GameObjectManager::Instance().Find("UltFrame")->SetEnabled(false);
    GameObjectManager::Instance().Find("UltHideGauge")->SetEnabled(false);
    GameObjectManager::Instance().Find("UltGauge")->SetEnabled(false);
    GameObjectManager::Instance().Find("ultCore")->SetEnabled(false);

    GameObjectManager::Instance().Find("SkillFrame")->SetEnabled(false);
    GameObjectManager::Instance().Find("Skill_Frame2")->SetEnabled(false);
    GameObjectManager::Instance().Find("SkillGaugeHide")->SetEnabled(false);
    GameObjectManager::Instance().Find("SkillGauge")->SetEnabled(false);
    GameObjectManager::Instance().Find("Skill_E")->SetEnabled(false);
    GameObjectManager::Instance().Find("SkillCore")->SetEnabled(false);


    GameObjectManager::Instance().Find("SkillFrame2")->SetEnabled(false);
    GameObjectManager::Instance().Find("Skill_Frame2")->SetEnabled(false);
    GameObjectManager::Instance().Find("SkillGaugeHide")->SetEnabled(false);
    GameObjectManager::Instance().Find("SkillGauge")->SetEnabled(false);
    GameObjectManager::Instance().Find("Skill_SPACE")->SetEnabled(false);
    GameObjectManager::Instance().Find("boostGauge2")->SetEnabled(false);
    GameObjectManager::Instance().Find("Decoration")->SetEnabled(false);
    GameObjectManager::Instance().Find("HitEffect")->SetEnabled(false);
}

void TutorialSystem::TutorialUIDisplay()
{
    GameObjectManager::Instance().Find("reticle")->SetEnabled(true);
    GameObjectManager::Instance().Find("HpFrame")->SetEnabled(true);
    GameObjectManager::Instance().Find("HpGauge")->SetEnabled(true);
    GameObjectManager::Instance().Find("UltFrame")->SetEnabled(true);
    GameObjectManager::Instance().Find("UltHideGauge")->SetEnabled(true);
    GameObjectManager::Instance().Find("UltGauge")->SetEnabled(true);
    GameObjectManager::Instance().Find("ultCore")->SetEnabled(true);

    GameObjectManager::Instance().Find("SkillFrame")->SetEnabled(true);
    GameObjectManager::Instance().Find("Skill_Frame2")->SetEnabled(true);
    GameObjectManager::Instance().Find("SkillGaugeHide")->SetEnabled(true);
    GameObjectManager::Instance().Find("SkillGauge")->SetEnabled(true);
    GameObjectManager::Instance().Find("Skill_E")->SetEnabled(true);
    GameObjectManager::Instance().Find("SkillCore")->SetEnabled(true);

    GameObjectManager::Instance().Find("SkillFrame2")->SetEnabled(true);
    GameObjectManager::Instance().Find("Skill_Frame2")->SetEnabled(true);
    GameObjectManager::Instance().Find("SkillGaugeHide")->SetEnabled(true);
    GameObjectManager::Instance().Find("SkillGauge")->SetEnabled(true);
    GameObjectManager::Instance().Find("Skill_SPACE")->SetEnabled(true);
    GameObjectManager::Instance().Find("boostGauge2")->SetEnabled(true);
    GameObjectManager::Instance().Find("Decoration")->SetEnabled(true);
    GameObjectManager::Instance().Find("HitEffect")->SetEnabled(true);

}

void TutorialSystem::OnGui()
{
    if (ImGui::Begin("TutorialSystem", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::DragInt("moveIndex", &moveSubTitleIndex);
        ImGui::DragInt("gunIndex", &gunSubTitleIndex);
        ImGui::DragInt("skillIndex", &skillSubTitleIndex);
        ImGui::DragFloat2("float", &skillSubTitle[skillSubTitleIndex].pos.x, 0.1);
        ImGui::DragInt("ultIndex", &ultSubTitleIndex);

    }
    ImGui::End();
}

#pragma endregion

