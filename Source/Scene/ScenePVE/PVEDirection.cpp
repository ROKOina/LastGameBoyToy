#include "PVEDirection.h"
#include <Component\Camera\EventCameraManager.h>
#include <Component\Camera\CameraCom.h>
#include "Component\Renderer\RendererCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Enemy\BossCom.h"
#include "Component\MoveSystem\MovementCom.h"

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
    GameObjectManager::Instance().Find("BOSS")->SetEnabled(false);
    GameObjectManager::Instance().Find("player")->SetEnabled(false);

    {
        auto& DirectionBoss = GameObjectManager::Instance().Create();
        DirectionBoss->SetName("Direction");
        std::shared_ptr<RendererCom> r = DirectionBoss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss.mdl");
        DirectionBoss->transform_->SetWorldPosition({ -2.878,-0.176,15.196 });
        DirectionBoss->transform_->SetEulerRotation({ 0.0, 180.0f, 0.0f });
        DirectionBoss->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        t = DirectionBoss->transform_;
        DirectionBoss->AddComponent<AnimationCom>();
        DirectionBoss->AddComponent<MovementCom>();


        {
            auto& DirectionBossSeconds = DirectionBoss->AddChildObject();
            DirectionBossSeconds->SetName("Seconds");
           // DirectionBossSeconds->transform_->
        }

        {
            auto& fixationPoint = GameObjectManager::Instance().Create();
            fixationPoint->SetName("Point");
            fixationPoint->transform_->SetWorldPosition({ -9.289, 4.573,7.350 });
            fixationPoint->AddComponent<MovementCom>();
        }
    }
    //最初にイベントカメラへ変更
    GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    animationCom = GameObjectManager::Instance().Find("Direction")->GetComponent<AnimationCom>();
}

//シーン演出統括
void PVEDirection::DirectionSupervision(float elapsedTime)
{
    switch (directionNumber)
    {
    case 0:
        DirectionOne(elapsedTime);
        break;
    case 1:
        DirectionTwo(elapsedTime);
        break;
    case 2:
        DirectionEnd(elapsedTime);
        break;
    }
}

//シーン演出
void PVEDirection::DirectionOne(float elapsedTime)
{
    if (!flag)
    {       
        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/test.eventcamera");
        animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true, false, 0.1f);
        flag = true;
    }

    auto& moveCom = GameObjectManager::Instance().Find("Direction")->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = GameObjectManager::Instance().Find("Direction")->transform_->GetWorldFront()*0.27f;
    moveCom->AddForce({ v.x,v.y,v.z });

    auto& pointCom= GameObjectManager::Instance().Find("Point")->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 x = GameObjectManager::Instance().Find("Point")->transform_->GetWorldUp()*0.0008f;
    pointCom->SetGravity(0.0f);
    pointCom->AddForce({ x.x,x.y,x.z });

    //GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->CameraShake(30.0f, 10.0f);

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        directionNumber += 1;
        flag = false;
    }
}

void PVEDirection::DirectionTwo(float elapsedTime)
{
    if (!flag)
    {
        GameObjectManager::Instance().Find("Seconds")->transform_->SetWorldPosition({ -2.878f,5.8f,6.073 });
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/two.eventcamera");
        animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false, false, 0.1f);
        flag = true;
    GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->CameraShake(30.0f, 10.0f);
    }
    DirectX::XMFLOAT3 s = GameObjectManager::Instance().Find("Point")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 e = GameObjectManager::Instance().Find("Seconds")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 tes = Mathf::Lerp(s, e, 0.1f);
    GameObjectManager::Instance().Find("Point")->transform_->SetWorldPosition(tes);

    if (animationCom.lock()->IsPlayAnimation()==false)
    {
        directionNumber += 1;
    }

}

void PVEDirection::DirectionEnd(float elapsedTime)
{

    //最初にイベントカメラへ変更
    GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->ActiveCameraChange();

    //ゲームオブジェクト本体を復活
    GameObjectManager::Instance().Find("BOSS")->SetEnabled(true);
    GameObjectManager::Instance().Find("player")->SetEnabled(true);

    //見世物はいったん使わないから消す
    GameObjectManager::Instance().Find("Direction")->SetEnabled(false);
   
    GameObjectManager::Instance().Find("player")->transform_->SetWorldPosition({ -2.3,-1,-49.3 });


}