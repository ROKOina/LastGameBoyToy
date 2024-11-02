#include "GameSource/Scene/ScenePVE/Direction/PVEDirection.h"
#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\RayCollisionCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\AimIKCom.h"
#include "Components\MovementCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\UenoCharacterCom.h"
#include "Components\Character\NomuraCharacterCom.h"
#include "Components\Character\HaveAllAttackCharacter.h"
#include "Components\Character\RegisterChara.h"
#include "Components/CPUParticle.h"
#include "Components\FootIKcom.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "GameSource/GameScript/FPSCameraCom.h"
#include "GameSource/GameScript/EventCameraCom.h"
#include "Components/CPUParticle.h"
#include "Components/GPUParticle.h"
#include "Graphics/Sprite/Sprite.h"
#include "Components/StageEditorCom.h"
#include "Components/SpawnCom.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components/InstanceRendererCom.h"
#include "Components\EasingMoveCom.h"
#include "Components\PushBackCom.h"

#include "GameSource/GameScript/EventCameraManager.h"

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
	GameObjectManager::Instance().Find("BOSS")->GetComponent<RendererCom>()->SetEnabled(false);
	GameObjectManager::Instance().Find("player")->GetComponent<RendererCom>()->SetEnabled(false);

	{
		auto& DirectionBoss = GameObjectManager::Instance().Create();
		DirectionBoss->SetName("Direction");
		std::shared_ptr<RendererCom> r = DirectionBoss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
		r->LoadModel("Data/Jammo/jammo.mdl");
		DirectionBoss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
		DirectionBoss->transform_->SetScale({ 0.06f, 0.06f, 0.06f });
		t = DirectionBoss->transform_;
		DirectionBoss->AddComponent<AnimationCom>();
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
		EventCameraManager::Instance().PlayEventCamera("Data/EventCamera/test.eventcamera");
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
		EventCameraManager::Instance().PlayEventCamera("Data/EventCamera/two.eventcamera");
		flag = true;
	}
}