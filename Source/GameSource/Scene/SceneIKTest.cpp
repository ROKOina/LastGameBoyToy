#include "Graphics/Graphics.h"

#include "Graphics/Light/LightManager.h"
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
#include "Components\FootIKcom.h"
#include "Components/CPUParticle.h"
#include "Components\RayCollisionCom.h"

#include "GameSource/GameScript/FreeCameraCom.h"

// 初期化
void SceneIKTest::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
        std::shared_ptr<FootIKCom> f = obj->AddComponent<FootIKCom>();
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0.0f, 0 });
        obj->transform_->SetScale({ 0.6f, 0.6f, 0.6f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/IKTestStage/ExampleStage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/IKTestStage/ExampleStage.collision");
    }

    //平行光源を追加
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

//更新処理
void SceneIKTest::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneIKTest::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());
}