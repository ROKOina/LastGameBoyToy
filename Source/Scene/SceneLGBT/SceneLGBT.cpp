#include "Graphics/Graphics.h"
#include "Scene/SceneManager.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "SceneLGBT.h"
#include "Component\Light\LightCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include <Component\Camera\FreeCameraCom.h>

//初期化
void SceneLGBT::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //LGBT
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("LGBT");
        obj->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/lgbt.ui", Sprite::SpriteShader::DISSOLVE, false);
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();
}

//更新
void SceneLGBT::Update(float elapsedTime)
{
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //シーン遷移やアイコンの処理
    SceneTransition(elapsedTime);
}

//描画
void SceneLGBT::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, { 0,0,0 });
}

//シーン遷移
void SceneLGBT::SceneTransition(float elapsedTime)
{
    const auto& lgbt = GameObjectManager::Instance().Find("LGBT")->GetComponent<Sprite>();
    easingtime += elapsedTime;
    if (easingtime > 2.0f && easingtime < 2.1f)
    {
        lgbt->EasingPlay();
    }

    if (easingtime > 7.5f)
    {
        dissolvetime += elapsedTime / 3;
        lgbt->SetClipTime(dissolvetime);

        if (lgbt->GetClipTime() > 1.0f)
        {
            SceneManager::Instance().ChangeScene(new SceneTitle);
        }
    }
}