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
#include "Component/Enemy/ScarecrowCom.h"
#include "Component\Stage\GateGimmickCom.h"
#include <StateMachine\Behaviar\InazawaCharacterState.h>
#include "Component/Item/UltSkillMaxItem.h"
#include "Component\UI\Font.h"
#include "Setting/Setting.h"
#include "Scene\SceneTitle\SceneTitle.h"


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
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("traCanvas");

    TrainingSystem::Instance().TrainingSystemStart();
    TutorialSystem::Instance().TutorialSystemStart();

    // INAZAWA
    {
        // タイトルへ
        {
            auto& name  = obj->AddChildObject();
            name->SetName("title");
            auto& spr=name->AddComponent<Sprite>("Data/SerializeData/UIData/setting/trainingGotoTitle.ui", Sprite::SpriteShader::DEFALT, true);
            spr->SetOrderinLayer(100);
            name->SetEnabled(false);
        }

        // チュートリアルへ
        {
            auto& skill = obj->AddChildObject();
            skill->SetName("tuto");
            auto& spr = skill->AddComponent<Sprite>("Data/SerializeData/UIData/setting/trainingGotoTutorial.ui", Sprite::SpriteShader::DEFALT, true);
            spr->SetOrderinLayer(100);
            skill->SetEnabled(false);
        }

        // トレーニングモードへ
        {
            auto& traing = obj->AddChildObject();
            traing->SetName("training");
            auto& spr = traing->AddComponent<Sprite>("Data/SerializeData/UIData/setting/trainingGotoTraining.ui", Sprite::SpriteShader::DEFALT, true);
            spr->SetOrderinLayer(100);
            traing->SetEnabled(false);
        }
    }
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

    Setting();
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

void TrainingManager::ChangeTrainigFlag()
{
   
    TutorialSystem::Instance().TutorialFlagClear();
    tutorialFlag = false;
}

void TrainingManager::Changeblackout()
{
    //暗転
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);
    
}

void TrainingManager::Changelightchange()
{
    //暗転
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(1.4f, 7.0f, parameters);
}

void TrainingManager::Setting()
{

    auto& tuto = GameObjectManager::Instance().Find("tuto");
    auto& title = GameObjectManager::Instance().Find("title");
    auto& traing = GameObjectManager::Instance().Find("training");
    GamePad& gamePad = Input::Instance().GetGamePad();

    DirectX::XMFLOAT4 selectColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

    //UI表示
    if (SceneManager::Instance().GetSettingScreen()->IsViewSetting())
    {
        if (tutorialFlag)
        {
            traing->SetEnabled(true);
            title->SetEnabled(true);
        }
        else
        {
            tuto->SetEnabled(true);
            title->SetEnabled(true);
        }
    }
    else
    {
        tuto->SetEnabled(false);
        title->SetEnabled(false);
        traing->SetEnabled(false);
        
    }


    //チュートリアルへ
    if (SceneManager::Instance().GetSettingScreen()->IsViewSetting()&&tuto->GetComponent<Sprite>()->GetHitSprite()&&!tutorialFlag)
    {
        tuto->GetComponent<Sprite>()->spc.color = selectColor;

        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
        {
            TrainingManager::Instance().ChangeTutorialFlag();
            SceneManager::Instance().GetSettingScreen()->SetViewSetting(false);
           
        }
    }
    else
    {
        tuto->GetComponent<Sprite>()->spc.color = Color;
    }


    //タイトルへ
    if (SceneManager::Instance().GetSettingScreen()->IsViewSetting() && title->GetComponent<Sprite>()->GetHitSprite())
    {
        title->GetComponent<Sprite>()->spc.color= selectColor;

        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
        {
            SceneManager::Instance().ChangeScene(new SceneTitle);
            TrainingManager::Instance().ChangeTrainigFlag();
        }
    }
    else
    {
        title->GetComponent<Sprite>()->spc.color = Color;
    }


    //トレーニングモードへ
    if (SceneManager::Instance().GetSettingScreen()->IsViewSetting() && traing->GetComponent<Sprite>()->GetHitSprite())
    {
        traing->GetComponent<Sprite>()->spc.color = selectColor;

        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
        {
            TrainingManager::Instance().ChangeTrainigFlag();
            TrainingSystem::Instance().TrainingObjDisplay();
            SceneManager::Instance().GetSettingScreen()->SetViewSetting(false);
        }
    }
    else
    {
        traing->GetComponent<Sprite>()->spc.color = Color;
    }
}

void TrainingManager::OnGUI()
{
    TutorialSystem::Instance().OnGui();
}

#pragma endregion





#pragma region トレーニングモード
void TrainingSystem::TrainingSystemStart()
{
    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("trainingScoreF");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 768.0f,30.0f };
        font->str = L"スコア";  //L付けてね
        font->scale = 0.6f;
        font->color.w = 1.0f;
        font->SetEnabled(false);
    }

    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("trainingScoreI");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 784.0f,66.0f };
        font->str = L"00";  //L付けてね
        font->scale = 1.0f;
        font->color.w = 1.0f;
        font->SetEnabled(false);
    }

    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("trainingRestF");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 1048.0f,30.0f };
        font->str = L"残り";  //L付けてね
        font->scale = 0.6f;
        font->color.w = 1.0f;
        font->SetEnabled(false);
    }

    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("trainingRestI");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 1050.0f,66.0f };
        font->str = L"00";  //L付けてね
        font->scale = 1.0f;
        font->color.w = 1.0f;
        font->SetEnabled(false);
    }



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

//射撃の開始処理と終了処理
void TrainingSystem::ShootingStartEndSystem()
{
    //開始処理
    if (GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->IsDeath()&&!shootingStartFlag)
    {
        shootingStartFlag = true;
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(false);
        GameObjectManager::Instance().Find("trainingScoreF")->GetComponent<Font>()->SetEnabled(true);
        GameObjectManager::Instance().Find("trainingScoreI")->GetComponent<Font>()->SetEnabled(true);
        GameObjectManager::Instance().Find("trainingRestF")->GetComponent<Font>()->SetEnabled(true);
        GameObjectManager::Instance().Find("trainingRestI")->GetComponent<Font>()->SetEnabled(true);
        ShootingSpawnCrow();
        
    }

    //終了処理
    if (scarecrowCount == scarecrowMaxTotal)
    {
        GameObjectManager::Instance().Find("scarecrow5")->SetEnabled(true);
        GameObjectManager::Instance().Find("scarecrow5")->GetComponent<CharaStatusCom>()->ReSpawn(1);
        GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));
        GameObjectManager::Instance().Find("trainingScoreF")->GetComponent<Font>()->SetEnabled(false);
        GameObjectManager::Instance().Find("trainingScoreI")->GetComponent<Font>()->SetEnabled(false);
        GameObjectManager::Instance().Find("trainingRestF")->GetComponent<Font>()->SetEnabled(false);
        GameObjectManager::Instance().Find("trainingRestI")->GetComponent<Font>()->SetEnabled(false);

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

    if (!shootingIntervalFlag&& GameObjectManager::Instance().Find("scarecrow")->GetComponent<CharaStatusCom>()->IsDeath())
    {
        //案山子破棄
        GameObjectManager::Instance().Remove(GameObjectManager::Instance().Find("scarecrow"));

        //初期化
        shootingIntervalFlag = true;
        scarecrowLifeTimer = 0.0f;
        shootingScore += 1;
        scarecrowCount += 1;
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

    GameObjectManager::Instance().Find("trainingScoreI")->GetComponent<Font>()->str = std::to_wstring(shootingScore);
    GameObjectManager::Instance().Find("trainingRestI")->GetComponent<Font>()->str = std::to_wstring(scarecrowMaxTotal - scarecrowCount);


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

//トレーニングモードのアイテム非表示
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





#pragma region チュートリアルモード
void TutorialSystem::TutorialSystemStart()
{
    //font
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testFont");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->position = { 0,0 };
        font->str = L"";  //L付けてね
        font->scale = 1.0f;
        font->color.w = 0.0f;
        font->SetEnabled(false);
    }

    {
        GameObj audio = GameObjectManager::Instance().Create();
        audio->SetName("Audio");
        audioObj = audio->AddComponent<AudioCom>().get();
        audioObj->RegisterSource(AUDIOID::TUTOLINES_01, "01");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_02, "02");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_03, "03");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_04, "04");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_05, "05");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_06, "06");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_07, "07");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_08, "08");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_09, "09");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_10, "10");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_11, "11");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_12, "12");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_13, "13");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_14, "14");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_15, "15");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_16, "16");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_17, "17");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_18, "18");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_19, "19");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_20, "20");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_21, "21");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_22, "22");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_23, "23");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_24, "24");
        audioObj->RegisterSource(AUDIOID::TUTOLINES_25, "25");

        
    }

#pragma region 移動

    moveSubTitle[0].str= L"ようこそ　チュートリアルへ";
    moveSubTitle[0].Lines = "01";
    moveSubTitle[0].pos = {650.0f,80.0f};
    moveSubTitle[0].UIID = 0;

    moveSubTitle[1].str = L"今から基本的な操作方法を教えるのだ";
    moveSubTitle[1].Lines = "02";
    moveSubTitle[1].pos = {553.0f,80.0f};
    moveSubTitle[1].UIID = 1;

    moveSubTitle[2].str = L"まずは、移動からなのだ";
    moveSubTitle[2].Lines = "03";
    moveSubTitle[2].pos = { 701.0f,80.0f };
    moveSubTitle[2].UIID = 2;

    moveSubTitle[3].str = L"キーボードのADキーで左右移動なのだ";
    moveSubTitle[3].Lines = "04";
    moveSubTitle[3].pos = { 539.0f,80.0f };
    moveSubTitle[3].UIID = 3;

    moveSubTitle[4].str = L"よくできたのだ";
    moveSubTitle[4].Lines = "05";
    moveSubTitle[4].pos = { 806.0f,80.0f };
    moveSubTitle[4].UIID = 4;

    moveSubTitle[5].str = L"次は縦移動なのだ";
    moveSubTitle[5].Lines = "06";
    moveSubTitle[5].pos = { 777.0f,80.0f };
    moveSubTitle[5].UIID = 5;

    moveSubTitle[6].str = L"キーボードのWSキーで縦移動なのだ";
    moveSubTitle[6].Lines = "07";
    moveSubTitle[6].pos = { 556.0f,80.0f };
    moveSubTitle[6].UIID = 6;

    moveSubTitle[7].str = L"素晴らしいのだ";
    moveSubTitle[7].Lines = "08";
    moveSubTitle[7].pos = { 809.0f,80.0f };
    moveSubTitle[7].UIID = 7;

    moveSubTitle[8].str = L"次はジャンプなのだ";
    moveSubTitle[8].Lines = "09";
    moveSubTitle[8].pos = { 740.0f,80.0f };
    moveSubTitle[8].UIID = 8;

    moveSubTitle[9].str = L"キーボードのスペースキーでジャンプなのだ";
    moveSubTitle[9].Lines = "10";
    moveSubTitle[9].pos = { 493.0f,80.0f };
    moveSubTitle[9].UIID = 9;

    moveSubTitle[10].str = L"天才なのだ";
    moveSubTitle[10].Lines = "11";
    moveSubTitle[10].pos = { 834.0f,80.0f };
    moveSubTitle[10].UIID = 10;

    moveSubTitle[11].str = L"次は銃の説明なのだ";
    moveSubTitle[11].Lines = "12";
    moveSubTitle[11].pos = { 715.0f,80.0f };
    moveSubTitle[11].UIID = 11;


#pragma endregion
#pragma region 銃セリフ

    gunSubTitle[0].str = L"銃の基本説明を教えるのだ";
    gunSubTitle[0].Lines = "13";
    gunSubTitle[0].pos = {674.0f,80.0f};
    gunSubTitle[0].UIID = 0;

    gunSubTitle[1].str = L"マウスの左クリックで、発射なのだ";
    gunSubTitle[1].Lines = "14";
    gunSubTitle[1].pos = { 571.0f,80.0f };
    gunSubTitle[1].UIID=1;

    gunSubTitle[2].str = L"よくできたのだ";
    gunSubTitle[2].Lines = "15";
    gunSubTitle[2].pos = { 765.0f,80.0f };
    gunSubTitle[2].UIID =2;

    gunSubTitle[3].str = L"次はスキルの説明なのだ";
    gunSubTitle[3].Lines = "16";
    gunSubTitle[3].pos = { 693.0f,80.0f};
    gunSubTitle[3].UIID=3;

#pragma endregion
#pragma region スキルセリフ

    skillSubTitle[0].str = L"スキルの基本操作を教えるのだ";
    skillSubTitle[0].Lines = "17";
    skillSubTitle[0].pos = { 624.0f,80.0f };
    skillSubTitle[0].UIID = 0;

    skillSubTitle[1].str = L"キーボードのEキーで発動なのだ";
    skillSubTitle[1].Lines = "18";
    skillSubTitle[1].pos = { 621.0f,80.0f };
    skillSubTitle[1].UIID =1;

    skillSubTitle[2].str = L"素晴らしいのだ";
    skillSubTitle[2].Lines = "19";
    skillSubTitle[2].pos = { 780.0f,80.0f };
    skillSubTitle[2].UIID = 2;

    skillSubTitle[3].str = L"次はウルトの説明なのだ";
    skillSubTitle[3].Lines = "20";
    skillSubTitle[3].pos = { 714.0f,80.0f };
    skillSubTitle[3].UIID=3;

#pragma endregion
#pragma region ウルトセリフ

    ultSubTitle[0].str = L"ウルトの基本操作を教えるのだ";
    ultSubTitle[0].Lines = "21";
    ultSubTitle[0].pos = { 628.0f,80.0f};
    ultSubTitle[0].UIID = 0;

    ultSubTitle[1].str = L"キーボードのRキーで発動なのだ";
    ultSubTitle[1].Lines = "22";
    ultSubTitle[1].pos = { 628.0f,80.0f};
    ultSubTitle[1].UIID = 1;

    ultSubTitle[2].str = L"天才なのだ";
    ultSubTitle[2].Lines = "23";
    ultSubTitle[2].pos = { 827.0f,80.0f};
    ultSubTitle[2].UIID = 2;

    ultSubTitle[3].str = L"これでチュートリアルは終わりなのだ";
    ultSubTitle[3].Lines = "24";
    ultSubTitle[3].pos = { 543.0f,80.0f};
    ultSubTitle[3].UIID = 3;

    ultSubTitle[4].str = L"Les’t enjoyなのだ";
    ultSubTitle[4].Lines = "25";
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


//暗転
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

//明転
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

//移動のロジック
void TutorialSystem::MoveTutorialManager(float elapsedTime)
{
    
    if (!linesFlag)
    {
        audioObj->Play(moveSubTitle[moveSubTitleIndex].Lines, false, 1.0f);
        linesFlag = true;
    }

    InputVec = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetLeftStick();

    if (moveSubTitleIndex != 3 && moveSubTitleIndex != 6 && moveSubTitleIndex != 9)
    {
        if (upFlag)
        {
            //タイマー更新
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
                        linesFlag = false;
                    }
                    else
                    {
                        moveSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                        linesFlag = false;
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
    //横移動
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
                    linesFlag = false;
                }

            }
        }

    }
    //縦移動
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
                    linesFlag = false;
                }

            }
        }
    }
    //ジャンプ
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
                    linesFlag = false;
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

    if (!linesFlag)
    {
        audioObj->Play(gunSubTitle[gunSubTitleIndex].Lines, false, 1.0f);
        linesFlag = true;
    }


    if (gunSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //タイマー更新
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
                        linesFlag = false;
                    }
                    else
                    {
                        gunSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                        linesFlag = false;
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

        //攻撃終了処理＆攻撃処理
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
                    linesFlag = false;

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

    if (!linesFlag)
    {
        audioObj->Play(skillSubTitle[skillSubTitleIndex].Lines, false, 1.0f);
        linesFlag = true;
    }

    if (skillSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //タイマー更新
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
                        linesFlag = false;

                    }
                    else
                    {
                        skillSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                        linesFlag = false;

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

        //攻撃終了処理＆攻撃処理
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
                    linesFlag = false;

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

    if (!linesFlag)
    {
        audioObj->Play(ultSubTitle[ultSubTitleIndex].Lines, false, 1.0f);
        linesFlag = true;
    }


    if (ultSubTitleIndex != 1)
    {
        if (upFlag)
        {
            //タイマー更新
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
                        linesFlag = false;
                    }
                    else
                    {
                        ultSubTitleIndex += 1;
                        upFlag = false;
                        downFlag = true;
                        linesFlag = false;
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

        //攻撃終了処理＆攻撃処理
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
                    linesFlag = false;
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
    GameObjectManager::Instance().Find("testFont")->GetComponent<Font>()->color.w = 0.0f;

    tutorialID = 0;

    moveSubTitleIndex = 0;
    gunSubTitleIndex = 0;
    skillSubTitleIndex = 0;
    ultSubTitleIndex = 0;

    lightTimer = 0.0f;

    blackTimer = 0.0f;

    moveInspectionFlag = false;         //動きのチュートリアル検査
    gunInspectionFlag = false;          //銃のチュートリアル検査
    skillInspectionFlag = false;        //スキルのチュートリアル検査
    ultInspectionFlag = false;          //ウルトのチュートリアル検査

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


    // moveSubTitle のリセット
    for (auto& subtitle : moveSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // gunSubTitle のリセット
    for (auto& subtitle : gunSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // skillSubTitle のリセット
    for (auto& subtitle : skillSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }

    // ultSubTitle のリセット
    for (auto& subtitle : ultSubTitle)
    {
        subtitle.subtitleTimer = 0.0f;
    }


}



void TutorialSystem::OnGui()
{
#ifdef _DEBUG
    if (ImGui::Begin("TutorialSystem", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::DragInt("moveIndex", &moveSubTitleIndex);
        ImGui::DragInt("gunIndex", &gunSubTitleIndex);
        ImGui::DragInt("skillIndex", &skillSubTitleIndex);
        ImGui::DragFloat2("float", &skillSubTitle[skillSubTitleIndex].pos.x, 0.1);
        ImGui::DragInt("ultIndex", &ultSubTitleIndex);

    }
    ImGui::End();
#endif
}

#pragma endregion

