#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/Light/Light.h"
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

    //ポストエフェクト追加
    m_posteffect = std::make_unique<PostEffect>();

    //平行光源を追加
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
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
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //デファードの設定
    m_posteffect->DeferredFirstSet();

    //オブジェクト描画
    GameObjectManager::Instance().Render();

    //デファードのリソース設定
    m_posteffect->DeferredResourceSet();

    //ポストエフェクト
    m_posteffect->PostEffectRender();

    //imgui描画
    m_posteffect->PostEffectImGui();

    //オブジェクト描画
    GameObjectManager::Instance().DrawGuizmo(sc->data.view, sc->data.projection);
}