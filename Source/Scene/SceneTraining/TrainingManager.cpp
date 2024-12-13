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
    tutorialFlag = true;
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
        font->str = L"ab";  //L�t���Ă�
        font->scale = 1.0f;
    }

#pragma region �ړ��Z���t

    movestr[0] = L"�悤�����@�`���[�g���A����";
    movestr[1] = L"�������{�I�ȑ�����@��������̂�";
    movestr[2] = L"�܂��́A�ړ�����Ȃ̂�";
    movestr[3] = L"�L�[�{�[�h��AD�L�[�ō��E�ړ��Ȃ̂�";
    movestr[4] = L"�悭�ł����̂�";
    movestr[5] = L"���͏c�ړ��Ȃ̂�";
    movestr[6] = L"�L�[�{�[�h��WS�L�[�ŏc�ړ��Ȃ̂�";
    movestr[7] = L"�f���炵���̂�";
    movestr[8] = L"���̓W�����v�Ȃ̂�";
    movestr[9] = L"�L�[�{�[�h�̃X�y�[�X�L�[�ŃW�����v�Ȃ̂�";
    movestr[10] = L"�V�˂Ȃ̂�";
    movestr[11] = L"���͏e�̐����Ȃ̂�";

#pragma endregion
#pragma region �e�Z���t

    gunstr[0] = L"�e�̊�{������������̂�";
    gunstr[1] = L"�}�E�X�̍��N���b�N�ŁA���˂Ȃ̂�";
    gunstr[2] = L"�悭�ł����̂�";
    gunstr[3] = L"���͐����Ȃ̂�";

#pragma endregion
#pragma region �X�L���Z���t

    skillstr[0] = L"�X�L���̊�{�����������̂�";
    skillstr[1] = L"�L�[�{�[�h��E�L�[�Ŕ����Ȃ̂�";
    skillstr[2] = L"�f���炵���̂�";
    skillstr[3] = L"�E���g�̊�{�����������̂�";

#pragma endregion
#pragma region �E���g�Z���t

    ultstr[0] = L"�E���g�̊�{�����������̂�";
    ultstr[1] = L"�L�[�{�[�h��R�L�[�Ŕ����Ȃ̂�";
    ultstr[2] = L"�V�˂Ȃ̂�";
    ultstr[3] = L"����Ń`���[�g���A���͏I���Ȃ̂�";
    ultstr[4] = L"�L�����N�^�[�ɂ���ăX�L����E���g�̓��e���ς�邩��A�悭������ǂ�ŗ~�����̂�";
    ultstr[5] = L"Les�ft enjoy�Ȃ̂�";

#pragma endregion


    int indexM = 0;
    for (auto& sub : moveSubTitle)
    {
        sub.UIID = indexM;
        sub.str = movestr[indexM];
        indexM++;
    }

    int indexG = 0;
    for (auto& sub : gunSubTitle)
    {
        sub.UIID = indexG;
        sub.str = gunstr[indexG];
        indexG++;
    }

    int indexS = 0;
    for (auto& sub : skillSubTitle)
    {
        sub.UIID = indexS;
        sub.str = skillstr[indexS];
        indexS++;
    }

    int indexU = 0;
    for (auto& sub : ultSubTitle)
    {
        sub.UIID = indexU;
        sub.str = ultstr[indexU];
        indexU++;
    }

   
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

//�ړ��̃��W�b�N
void TutorialSystem::MoveTutorialManager(float elapsedTime)
{
    //�^�C�}�[�X�V
    moveSubTitle[moveSubTitleIndex].subtitleTimer += elapsedTime;


    InputVec = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetLeftStick();

    if (moveSubTitleIndex != 3&&moveSubTitleIndex!=6&&moveSubTitleIndex!=9)
    {
        if (moveSubTitle[moveSubTitleIndex].subtitleTimer > moveSubTitle[moveSubTitleIndex].subtitleTime)
        {
            if (moveSubTitleIndex == 11)
            {
                moveInspectionFlag = true;
            }
            else
            {
                moveSubTitleIndex += 1;
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

        if (moveAFlag && moveDFlag)
        {
            moveSubTitleIndex += 1;
        }

    }
    //�c�ړ�
    else if (moveSubTitleIndex == 6)
    {
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
            moveSubTitleIndex += 1;
        }
    }
    //�W�����v
    else if (moveSubTitleIndex == 9)
    {
        if (CharacterInput::JumpButton_SPACE & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonDown())
        {
            moveSubTitleIndex += 1;
        }
    }


    if (moveInspectionFlag)
    {
        NextTutorial(TutorialID::GUN);
    }
}

void TutorialSystem::GunTutorialManager(float elapsedTime)
{
    //�^�C�}�[�X�V
    gunSubTitle[gunSubTitleIndex].subtitleTimer += elapsedTime;


    if (gunSubTitleIndex != 1)
    {
        if (gunSubTitle[gunSubTitleIndex].subtitleTimer > gunSubTitle[gunSubTitleIndex].subtitleTime)
        {
            if (gunSubTitleIndex == 3)
            {
                gunInspectionFlag = true;
            }
            else
            {
                gunSubTitleIndex += 1;
            }
        }
    }
    else if (gunSubTitleIndex==1)
    {
        //�U���I���������U������
        if (CharacterInput::MainAttackButton & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            gunSubTitleIndex += 1;
        }
    }



    if (gunInspectionFlag)
    {
        NextTutorial(TutorialID::SKILL);
    }
}

void TutorialSystem::SkillTutorialManager(float elapsedTime)
{
    //�^�C�}�[�X�V
    skillSubTitle[skillSubTitleIndex].subtitleTimer += elapsedTime;


    if (skillSubTitleIndex != 1)
    {
        if (skillSubTitle[skillSubTitleIndex].subtitleTimer > skillSubTitle[skillSubTitleIndex].subtitleTime)
        {
            if (skillSubTitleIndex == 3)
            {
                skillInspectionFlag = true;
            }
            else
            {
                skillSubTitleIndex += 1;
            }
        }
    }
    else if (skillSubTitleIndex == 1)
    {
        //�U���I���������U������
        if (CharacterInput::MainSkillButton_E & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            skillSubTitleIndex += 1;
        }
    }




    if (skillInspectionFlag)
    {
        NextTutorial(TutorialID::ULT);
    }
}

void TutorialSystem::UltTutorialManager(float elapsedTime)
{

    //�^�C�}�[�X�V
    ultSubTitle[ultSubTitleIndex].subtitleTimer += elapsedTime;


    if (ultSubTitleIndex != 1)
    {
        if (ultSubTitle[ultSubTitleIndex].subtitleTimer > ultSubTitle[ultSubTitleIndex].subtitleTime)
        {
            if (ultSubTitleIndex == 3)
            {
                ultInspectionFlag = true;
            }
            else
            {
                ultSubTitleIndex += 1;
            }
        }
    }
    else if (ultSubTitleIndex == 1)
    {
        //�U���I���������U������
        if (CharacterInput::UltimetButton & GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetButtonUp())
        {
            ultSubTitleIndex += 1;
        }
    }

    if (ultInspectionFlag)
    {
        NextTutorial(TutorialID::END);
    }
}

void TutorialSystem::OnGui()
{
    if (ImGui::Begin("TutorialSystem", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::DragInt("moveIndex", &moveSubTitleIndex);
        ImGui::DragInt("gunIndex", &gunSubTitleIndex);
        ImGui::DragInt("skillIndex", &skillSubTitleIndex);
        ImGui::DragInt("ultIndex", &ultSubTitleIndex);

    }
    ImGui::End();
}

#pragma endregion

