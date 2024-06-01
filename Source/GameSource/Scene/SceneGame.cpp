#include "Graphics/Graphics.h"
#include "Input\Input.h"

#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\ParticleSystemCom.h"

#include "Components/ParticleComManager.h"

// ‰Šú‰»
void SceneGame::Initialize()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    obj->SetName("test");
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 1.00f, 1.00f, 1.00f });

    const char* filename = "Data/OneCoin/robot.mdl";
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>();
    r->LoadModel(filename);

}

// I—¹‰»
void SceneGame::Finalize()
{
}

// XVˆ—
void SceneGame::Update(float elapsedTime)
{
    GameObjectManager::Instance().Update(elapsedTime);

    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("test");
    std::shared_ptr<Component> r = obj->GetComponent<RendererCom>();
    int i = 0;
}

// •`‰æˆ—
void SceneGame::Render(float elapsedTime)
{
}

