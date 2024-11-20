#include "PVEDirection.h"
#include <Component\Camera\EventCameraManager.h>
#include <Component\Camera\CameraCom.h>
#include "Component\Renderer\RendererCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Enemy\BossCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\PostEffect\PostEffect.h"
#include <Scene/SceneManager.h>
#include "Scene/SceneResult/SceneResult.h"
#include "Component\Phsix\RigidBodyCom.h"

PVEDirection::PVEDirection()
{
   
}

PVEDirection::~PVEDirection()
{
}

void PVEDirection::Update(float elapsedTime)
{
    DirectionSupervision(elapsedTime);
}

void PVEDirection::DirectionStart()
{
    //GameObjectManager::Instance().Find("BOSS")->SetEnabled(false);
    GameObjectManager::Instance().Find("player")->SetEnabled(false);

    {
        auto& armParts = GameObjectManager::Instance().Create();
        armParts->SetName("arm");
        std::shared_ptr<RendererCom> r = armParts->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/parts/arm.mdl");
        armParts->transform_->SetWorldPosition({ 10.552f, 20.0f,-32.969f });
        armParts->transform_->SetEulerRotation({ 0.0f,-155.999f, -32.0f });
        armParts->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        armParts->AddComponent<MovementCom>();
        RigidBodyCom* rigid = armParts->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Mesh).get();
        armParts->GetComponent<MovementCom>()->SetGravity(0.0);
        armParts->SetEnabled(false);
    }

    {
        auto& headParts = GameObjectManager::Instance().Create();
        headParts->SetName("head");
        std::shared_ptr<RendererCom> r = headParts->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/parts/head.mdl");
        headParts->AddComponent<MovementCom>();
        headParts->transform_->SetWorldPosition({ 12.721f,30.0f,-33.909f });
        headParts->transform_->SetEulerRotation({ -10.00f,-210.998f, 42.0f });
        headParts->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        RigidBodyCom* rigid = headParts->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Mesh).get();
        headParts->GetComponent<MovementCom>()->SetGravity(0.0);
        headParts->SetEnabled(false);
    }

    {
        auto& shoulderParts = GameObjectManager::Instance().Create();
        shoulderParts->SetName("shoulder");
        std::shared_ptr<RendererCom> r = shoulderParts->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/parts/shoulder.mdl");
        shoulderParts->AddComponent<MovementCom>();
        shoulderParts->transform_->SetWorldPosition({ 13.378f, 25.0f,-29.444f });
        shoulderParts->transform_->SetEulerRotation({ 0.0f,-33.0f, -30.0f });
        shoulderParts->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        RigidBodyCom* rigid = shoulderParts->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Mesh).get();
        shoulderParts->GetComponent<MovementCom>()->SetGravity(0.0);
        shoulderParts->SetEnabled(false);
    }

    {
        eventBoss = GameObjectManager::Instance().Find("BOSS");
        eventBoss->transform_->SetWorldPosition({ -2.878,-0.176,15.196 });
        eventBoss->transform_->SetEulerRotation({ 0.0, 180.0f, 0.0f });
        eventBoss->transform_->SetScale({ 0.23f, 0.23f, 0.23f });

            /*GameObjectManager::Instance().Create();
        eventBoss->SetName("Direction");
        std::shared_ptr<RendererCom> r = eventBoss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        eventBoss->transform_->SetWorldPosition({ -2.878,-0.176,15.196 });
        eventBoss->transform_->SetEulerRotation({ 0.0, 180.0f, 0.0f });

        t = eventBoss->transform_;
        eventBoss->AddComponent<MovementCom>();
        eventBoss->AddComponent<BossCom>();*/

        animationCom = eventBoss->AddComponent<AnimationCom>();

        {
            auto& DirectionBossSeconds = eventBoss->AddChildObject();
            DirectionBossSeconds->SetName("Seconds");
            // DirectionBossSeconds->transform_->SetLocalPosition({ 0,5,0 });
        }

        {
            auto& fixationPoint = GameObjectManager::Instance().Create();
            fixationPoint->SetName("Point");
            fixationPoint->transform_->SetWorldPosition({ -9.289, 4.573,7.350 });
            fixationPoint->AddComponent<MovementCom>();
            fixationPoint->GetComponent<MovementCom>()->SetGravity(0.0f);
        }
    }
    //最初にイベントカメラへ変更
    GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    directionNumber = 0;
}

//シーン演出統括
void PVEDirection::DirectionSupervision(float elapsedTime)
{
    switch (directionNumber)
    {
    case 0:
        DirectionFOne(elapsedTime);
        break;
    case 1:
        DirectionFTwo(elapsedTime);
        break;
    case 2:
        DirectionFEnd(elapsedTime);
        break;
    case 3:
        DirectionCOne(elapsedTime);
        break;
    case 4:
        DirectionCTwo(elapsedTime);
        break;
    case 5:
        DirectionCThi(elapsedTime);
        break;
    case 6:
        DirectionCFou(elapsedTime);
        break;
    case 7:
        DirectionCEnd(elapsedTime);
        break;
    }
}

//シーン演出
void PVEDirection::DirectionFOne(float elapsedTime)
{
    if (!flag)
    {
        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/test.eventcamera");

        //イベントシーン用の歩きステートへ遷移
        eventBoss->GetComponent<BossCom>()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_WALK);

        flag = true;
    }

    auto& pointCom = GameObjectManager::Instance().Find("Point")->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 x = GameObjectManager::Instance().Find("Point")->transform_->GetWorldUp() * 0.0008f;
    pointCom->SetGravity(0.0f);
    pointCom->AddForce({ x.x,x.y,x.z });

    //GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->CameraShake(30.0f, 10.0f);

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        directionNumber += 1;
        flag = false;
        pointCom->AddForce({ 0.0f,0.0f,0.0f });
        GameObjectManager::Instance().Find("Point")->transform_->SetWorldPosition(x);
    }

}

void PVEDirection::DirectionFTwo(float elapsedTime)
{
    if (!flag)
    {
        //イベントシーン用のパンチステートへ遷移
        eventBoss->GetComponent<BossCom>()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_PUNCH);

        GameObjectManager::Instance().Find("Seconds")->transform_->SetWorldPosition({ -2.878f,5.8f,6.073 });
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/two.eventcamera");

        flag = true;
        
    }
    DirectX::XMFLOAT3 s = GameObjectManager::Instance().Find("Point")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 e = GameObjectManager::Instance().Find("Seconds")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 tes = Mathf::Lerp(s, e, 0.1f);
    GameObjectManager::Instance().Find("Point")->transform_->SetWorldPosition(tes);

    if (eventBoss->GetComponent<AnimationCom>()->IsPlayAnimation() == false)
    {
        //暗転
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
        GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);
        deleyTimer += elapsedTime;
        if (deleyTime < deleyTimer)
        {
            directionNumber += 1;
            deleyTimer = 0.0f;
            flag = false;
        }
    }
}

void PVEDirection::DirectionFEnd(float elapsedTime)
{
    if (!flag)
    {
        //最初にイベントカメラへ変更
        GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();
        //暗転
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
        GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(1.4f, 7.0f, parameters);
        //ゲームオブジェクト本体を復活
        GameObjectManager::Instance().Find("BOSS")->SetEnabled(true);
        GameObjectManager::Instance().Find("player")->SetEnabled(true);
        

        //見世物はいったん使わないから消す
        //eventBoss->SetEnabled(false);
        eventBoss->GetComponent<BossCom>()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);

        flag = true;
    }
    //GameObjectManager::Instance().Find("player")->transform_->SetWorldPosition({ -2.3,-1,-49.3 });

    if (GameObjectManager::Instance().Find("BOSS")->GetComponent<CharaStatusCom>()->IsDeath())
    {
        //暗転
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
        GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);
        deleyTimer += elapsedTime;
        if (deleyTime < deleyTimer)
        {
            directionNumber += 1;
            flag = false;
        }
    }
}

void PVEDirection::DirectionCOne(float elaspsedTime)
{
    if (!flag)
    {
        //暗転
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
        GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(1.4f, 7.0f, parameters);

        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();

        GameObjectManager::Instance().Find("BOSS")->SetEnabled(false);
        GameObjectManager::Instance().Find("player")->SetEnabled(false);

        eventBoss->SetEnabled(true);
        eventBoss->transform_->SetWorldPosition({ -2.500,-0.005,-11.000 });
        eventBoss->GetComponent<BossCom>()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);

        GameObjectManager::Instance().Find("Seconds")->transform_->SetLocalPosition({ 0.00,23.0,-13.0 });

        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/clearS.eventcamera");


        flag = true;
    }

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        directionNumber += 1;
        flag = false;
    }
}

void PVEDirection::DirectionCTwo(float elaspdTime)
{
    if (!flag)
    {
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/ClearTwo.eventcamera");
        flag = true;
    }

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        directionNumber += 1;
        flag = false;
    }
}

void PVEDirection::DirectionCThi(float elapsedTime)
{
    if (!flag)
    {
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/ClearEnd.eventcamera");
        GameObjectManager::Instance().Find("arm")->SetEnabled(true);
        GameObjectManager::Instance().Find("head")->SetEnabled(true);
        GameObjectManager::Instance().Find("shoulder")->SetEnabled(true);
        GameObjectManager::Instance().Find("arm")->GetComponent<MovementCom>()->SetGravity(0.98f);
        GameObjectManager::Instance().Find("head")->GetComponent<MovementCom>()->SetGravity(0.98f);
        GameObjectManager::Instance().Find("shoulder")->GetComponent<MovementCom>()->SetGravity(0.98f);
        flag = true;
    }

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        directionNumber += 1;
        flag = false;
    }
}

void PVEDirection::DirectionCFou(float elapsedTime)
{
    if (!flag)
    {
        //暗転
        std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
        GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);
        SceneManager::Instance().ChangeSceneDelay(new SceneResult, 2);
        flag = true;
    }
}

void PVEDirection::DirectionCEnd(float elaspdTime)
{
}