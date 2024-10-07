#include "StageEditorCom.h"
#include "Input\Input.h"
#include "./Dialog.h"
#include "Framework.h"
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
#include "././GameSource/Scene/SceneManager.h"
#include <cstring>
#include "./Components/RayCollisionCom.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

void StageEditorCom::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (Mouse::BTN_LEFT & mouse.GetButtonDown() && nowEdit && onImGui)
    {
        HitResult hit;
        if (MouseVsStage(hit))
        {
           GameObj obj = GameObjectManager::Instance().Create();
           obj->transform_->SetWorldPosition(hit.position);
           obj->transform_->SetScale({ 0.02f,0.02f,0.02f });

           RendererCom* render = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
           render->LoadModel("Data/pico/pico.mdl");
           render->LoadMaterial("Data/pico/pico.Material");

           objList.emplace_back(obj);
        }
    }
}

void StageEditorCom::OnGUI()
{
    static char* edit = "EditStart";

    onImGui = !ImGui::IsWindowHovered();
    if (ImGui::Button(edit))
    {
        nowEdit = !nowEdit;
        nowEdit ? edit = "EditNow" : edit = "EditStart";
    }

    if (ImGui::Button("Save"))
    {
        ObjectSave();
    }
    if (ImGui::Button("Load"))
    {
        ObjectLoad();
    }

    //StageSelect();
}

void StageEditorCom::StageSelect()
{
    //ボタンでオープン
    //if (ImGui::Button("StageSelect"))
    //{
    //    //ステージモデルの設定
    //    static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

    //    char filename[256] = { 0 };
    //    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
    //    if (result == DialogResult::OK)
    //    {
    //        stageObj = ImportModel(filename);
    //    }
    //}
}

void StageEditorCom::ObjectRegister()
{
    //ImGui::InputText("RegistObjName", registerObjName, sizeof(registerObjName));
    //if (ImGui::Button("Regist"))
    //{
    //    std::string copyname;
    //    copyname = registerObjName;

    //    gameObjcts[copyname] = "";
    //    registerObjName[0] = '\0';
    //}
}

void StageEditorCom::ObjectPlace()
{
    //Mouse& mouse = Input::Instance().GetMouse();
    //if (stageObj != nullptr && mouse.GetButtonDown() & Mouse::BTN_LEFT)
    //{
    //    HitResult result;
    //    if (MouseVsStage(result))
    //    {
    //        //配置
    //        GameObj obj = GameObjectManager::Instance().Create();
    //        obj->transform_->SetWorldPosition(result.position);
    //    }
    //}
}

bool StageEditorCom::MouseVsStage(HitResult& hit)
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

    return Collision::IntersectRayVsModel(world_start, world_end, GetGameObject()->GetComponent<RendererCom>()->GetModel(), hit);

}

void StageEditorCom::ObjectSave()
{
    nlohmann::json j;

    for (auto& obj : objList)
    {
        j["Pos"] += { {"x", obj->transform_->GetWorldPosition().x}, { "y", obj->transform_->GetWorldPosition().y }, { "z", obj->transform_->GetWorldPosition().z }};
    }

    // 相対パスでファイルを作成
    static const char* filter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "json", Framework::GetInstance()->GetHWND());

    std::ofstream file(filename);

    if (!file.is_open())
    {
        std::cout << "ファイルを開くことができませんでした。" << std::endl;
    }
    else
    {
        file << std::setw(4) << j;
        std::cout << "JSONファイルが書き込まれました。" << std::endl;
    }
}

void StageEditorCom::ObjectLoad()
{
    using namespace std;

    //ステージモデルの設定
    static const char* filter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
    if (result == DialogResult::OK)
    {
        //ファイルを開く
        fstream ifs(filename);
        if (ifs.good())
        {
            //Json型を取得
            nlohmann::json j;
            ifs >> j;

            for (int i = 0; i < j["Pos"].size(); ++i)
            {
                GameObj obj = GameObjectManager::Instance().Create();
                obj->transform_->SetWorldPosition(DirectX::XMFLOAT3(j["Pos"].at(i)["x"], j["Pos"].at(i)["y"], j["Pos"].at(i)["z"]));
                obj->transform_->SetScale({ 0.02f,0.02f,0.02f });

                RendererCom* render = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
                render->LoadModel("Data/pico/pico.mdl");
                render->LoadMaterial("Data/pico/pico.Material");

                objList.emplace_back(obj);
            }
        }
    }

    
}
