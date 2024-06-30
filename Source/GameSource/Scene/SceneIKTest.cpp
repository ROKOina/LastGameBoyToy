#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/Light/Light.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneIKTest.h"
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
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components/CPUParticle.h"

#include "GameSource/GameScript/FreeCameraCom.h"

// 初期化
void SceneIKTest::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        std::shared_ptr<FreeCameraCom> f = freeCamera->AddComponent<FreeCameraCom>();
        f->SetPerspectiveFov
        (
            DirectX::XMConvertToRadians(45),
            graphics.GetScreenWidth() / graphics.GetScreenHeight(),
            0.1f, 1000.0f
        );
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
        f->SetActiveInitialize();
    }

    //普通のカメラ
    {
        std::shared_ptr<GameObject> camera = GameObjectManager::Instance().Create();
        camera->SetName("normalcamera");

        std::shared_ptr<CameraCom> c = camera->AddComponent<CameraCom>();
        c->SetPerspectiveFov
        (
            DirectX::XMConvertToRadians(45),
            graphics.GetScreenWidth() / graphics.GetScreenHeight(),
            0.1f, 1000.0f
        );

        camera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();

        auto& oo = obj->AddChildObject();
        oo->SetName("barrier1");
        oo->transform_->SetScale({ 500,500,500 });
        oo->transform_->SetLocalPosition({ 1000,0,0 });
        std::shared_ptr<RendererCom> ro = oo->AddComponent<RendererCom>(SHADERMODE::DEFAULT, BLENDSTATE::ADD);
        ro->LoadModel("Data/Ball/b.mdl");

        auto& oo1 = oo->AddChildObject();
        oo1->SetName("barrier2");
        oo1->transform_->SetScale({ 1,1,1 });
        oo1->transform_->SetLocalPosition({ 3,0,0 });
        std::shared_ptr<RendererCom> ro1 = oo1->AddComponent<RendererCom>(SHADERMODE::DEFAULT, BLENDSTATE::ADD);
        ro1->LoadModel("Data/Ball/b.mdl");
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, -0.4f, 0 });
        obj->transform_->SetScale({ 0.05f, 0.05f, 0.05f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/Stage/Stage.mdl");
    }

    //平行光源を追加
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

//更新処理
void SceneIKTest::Update(float elapsedTime)
{
}

void SceneIKTest::Render(float elapsedTime)
{
}