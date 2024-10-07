#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "imgui.h"
#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\RendererCom.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "Graphics/Model/ResourceManager.h"
#include "./Dialog.h"
#include "Framework.h"
#include "SceneStageEditor.h"
#include "./SceneManager.h"
#include <cstring>
#include "./Components/RayCollisionCom.h"

void SceneStageEditor::Initialize()
{
    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

#pragma region グラフィック系の設定
    //平行光源を追加
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);

    // スカイボックスの設定
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\snowy_hillside_4k.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

#pragma endregion
}

void SceneStageEditor::Finalize()
{
    GameObjectManager::Instance().AllRemove();
}

void SceneStageEditor::Update(float elapsedTime)
{
    //オブジェクト配置
    ObjectPlace();

    // ゲームオブジェクトの更新
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneStageEditor::Render(float elapsedTime)
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

    //imgui
    ImGui();
}

//imgui
void SceneStageEditor::ImGui()
{
    StageSelect();
    ObjectRegister();

    //配置するオブジェクト選択
    for (auto& objName : gameObjcts)
    {
        ImGui::Text(objName.first.c_str());

        if (!objName.second.empty())
        {
            if (ImGui::Button("Place"))
            {
                selectObjName = objName.second;
            }
        }
        else
        {
            if (ImGui::Button("None"))
            {
                //ステージモデルの設定
                static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

                char filename[256] = { 0 };
                DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
                if (result == DialogResult::OK)
                {
                    objName.second = filename;
                }
            }
        }
    }
}

// モデル入力
GameObj SceneStageEditor::ImportModel(const char* filename)
{
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("stage");
    obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
    obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel(filename);

    return obj;
}

void SceneStageEditor::StageSelect()
{
    //ボタンでオープン
    if (ImGui::Button("StageSelect"))
    {
        //ステージモデルの設定
        static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

        char filename[256] = { 0 };
        DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
        if (result == DialogResult::OK)
        {
            stageObj = ImportModel(filename);
        }
    }
}

//配置するオブジェクト登録
void SceneStageEditor::ObjectRegister()
{
    ImGui::InputText("RegistObjName",registerObjName, sizeof(registerObjName));
    if (ImGui::Button("Regist"))
    {
        std::string copyname;
        copyname = registerObjName;

        gameObjcts[copyname] = "";
        registerObjName[0] = '\0';
    }
}

//ステージに配置
void SceneStageEditor::ObjectPlace()
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (stageObj != nullptr && mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        HitResult result;
        if (MouseVsStage(result))
        {
            //配置
            GameObj obj = GameObjectManager::Instance().Create();
            obj->transform_->SetWorldPosition(result.position);
        }
    }
}

bool SceneStageEditor::MouseVsStage(HitResult hit)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    CameraCom* cameraCom = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>().get();

    // ビューポート
    D3D11_VIEWPORT view_port;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &view_port);

    // 変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&cameraCom->GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&cameraCom->GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    Mouse& mouse = Input::Instance().GetMouse();

    // マウスカーソル座標を取得
    DirectX::XMFLOAT3 screen_position;
    screen_position.x = static_cast<float>(mouse.GetPositionX());
    screen_position.y = static_cast<float>(mouse.GetPositionY());
    screen_position.z = 0;

    // レイの始点
    DirectX::XMFLOAT3 world_start;
    DirectX::XMStoreFloat3(&world_start,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	ビューポート左上X座標
            view_port.TopLeftY,	//	ビューポート左上Y座標
            view_port.Width,	//	ビューポート幅
            view_port.Height,	//	ビューポート高さ
            0.0f,	// 深度値の最小値
            1.0f,	// 深度値の最大値
            Projection,	//	プロジェクション行列
            View,	//	ビュー行列
            World	//	ワールド行列
        )
    );

    screen_position.z = 1;

    // レイの終点
    DirectX::XMFLOAT3 world_end;
    DirectX::XMStoreFloat3(&world_end,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	ビューポート左上X座標
            view_port.TopLeftY,	//	ビューポート左上Y座標
            view_port.Width,	//	ビューポート幅
            view_port.Height,	//	ビューポート高さ
            0.0f,	// 深度値の最小値
            1.0f,	// 深度値の最大値
            Projection,	//	プロジェクション行列
            View,	//	ビュー行列
            World	//	ワールド行列
        )
    );

    return Collision::IntersectRayVsModel(world_start, world_end, stageObj->GetComponent<RendererCom>()->GetModel(), hit);
}
