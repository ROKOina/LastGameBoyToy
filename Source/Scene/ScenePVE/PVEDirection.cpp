#include "PVEDirection.h"
#include <Component\Camera\EventCameraManager.h>
#include <Component\Camera\CameraCom.h>
#include "Component\Renderer\RendererCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Enemy\BossCom.h"

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
        DirectionBoss->transform_->SetWorldPosition({ -1.268f,-5.251f,37.918f });
        DirectionBoss->transform_->SetEulerRotation({ 0.0f,200.999f,0.00f });
        DirectionBoss->transform_->SetScale({ 0.06f, 0.06f, 0.06f });
        t = DirectionBoss->transform_;
        DirectionBoss->AddComponent<AnimationCom>();
    }

    {
        auto& fixationPoint = GameObjectManager::Instance().Create();
        fixationPoint->SetName("Point");
        fixationPoint->transform_->SetWorldPosition({ -3.814f, -5.078f, 29.042f });
    }

    //最初にイベントカメラへ変更
    GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
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
    }
}

//シーン演出
void PVEDirection::DirectionOne(float elapsedTime)
{
    if (!flag)
    {
       
        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/test.eventcamera");
        
        flag = true;
    }

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
        //EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/two.eventcamera");
        flag = true;
    }
}