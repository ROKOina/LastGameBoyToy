#include "Graphics/Graphics.h"
#include "Scene/SceneManager.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "SceneLGBT.h"
#include "Component\Light\LightCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component/Character/RegisterChara.h"
#include "Component/Renderer/RendererCom.h"
#include <Component\Camera\FreeCameraCom.h>

#include <Component\Stage\StageEditorCom.h>

//初期化
void SceneLGBT::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        auto& c = freeCamera->AddComponent<FreeCameraCom>();
        c->SetFocusPos({ 1000,1000,1000 });
        freeCamera->transform_->SetWorldPosition({ 100, 500, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //LGBT
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("LGBT");
        obj->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/lgbt.ui", Sprite::SpriteShader::DISSOLVE, false);
    }

    //キャラのシリアル情報登録
    for (int i = 0; i< int(RegisterChara::CHARA_LIST::MAX); ++i)
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName(std::string("p" + std::to_string(i)).c_str());
        obj->transform_->SetWorldPosition({ 100,0,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(i), obj);
        {
            std::shared_ptr<GameObject> arm = GameObjectManager::Instance().Create();
            arm->SetName("armChild");
            arm->transform_->SetScale({ 0.5f,0.5f,0.5f });
            std::shared_ptr<RendererCom> r = arm->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");

        }
    }

    //ステージのシリアル情報登録
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, -1000 });
        stageObj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        //ステージ
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //判定生成
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
        //Jsonからオブジェクト配置
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");

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