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

// 初期化
void SceneGame::Initialize()
{
    std::shared_ptr<GameObject> camera = GameObjectManager::Instance().Create();
    camera->SetName("camera");

    Graphics& graphics = Graphics::Instance();
    std::shared_ptr<CameraCom> c = camera->AddComponent<CameraCom>();
    c->SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        0.1f, 1000.0f
    );
    camera->transform_->SetWorldPosition({ 0, 5, -10 });

    ConstantBufferInitialize();




    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    obj->SetName("test");
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 1.00f, 1.00f, 1.00f });

    //const char* filename = "Data/picola/pi.mdl";
    const char* filename = "Data/OneCoin/robot.mdl";
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>();
    r->LoadModel(filename);
    r->GetModel()->PlayAnimation(0, true);

    //std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();

}

// 終了化
void SceneGame::Finalize()
{
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //コンポーネントゲット
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("test");
    std::shared_ptr<RendererCom> r = obj->GetComponent<RendererCom>();

    ConstantBufferUpdate();
}

// 描画処理
void SceneGame::Render(float elapsedTime)
{
    GameObjectManager::Instance().Render();
}

