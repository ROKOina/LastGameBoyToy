#include "StageEditorCom.h"
#include "Input\Input.h"
#include "SystemStruct\Dialog.h"
#include "Graphics/Graphics.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Graphics/Model/ResourceManager.h"
#include "SystemStruct\Framework.h"
#include "Scene/SceneManager.h"
#include <cstring>
#include "Component/Collsion/RayCollisionCom.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "Component\Collsion\NodeCollsionCom.h"

void StageEditorCom::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    //左クリック＆配置フラグ＆マウスがGUIの上になければオブジェクトを置く
    if (Mouse::BTN_LEFT & mouse.GetButtonDown() && nowEdit && onImGui)
    {
        //マウスとステージの当たり判定
        HitResult hit;
        if (MouseVsStage(hit))
        {
            ObjectPlace(
                objType,              //選択中のオブジェクト
                hit.position,         //位置
                { 0.02f,0.02f,0.02f },//スケール
                { 0,0,0,1 },          //回転値
                placeObjcts[objType.c_str()].filePath.c_str(),    //modelのパス
                placeObjcts[objType.c_str()].collisionPath.c_str()//nodeCollsionのパス
            );
        }
    }
}

void StageEditorCom::OnGUI()
{
    //配置開始ボタン
    {
        //ボタンに使う文字
        static char* edit = (char*)u8"配置開始";
        //ボタンの色
        static ImVec4 gui_color = {};
        nowEdit ? gui_color = { 0.7f,0.0f,0.0f,1.0f } : gui_color = { 0.0f,0.0f,0.7f,1.0f };
        ImGui::PushStyleColor(ImGuiCol_Button, gui_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui_color);

        onImGui = !ImGui::IsWindowHovered();
        if (ImGui::Button(edit, { 400,50 }))
        {
            nowEdit = !nowEdit;
            nowEdit ? edit = (char*)u8"配置終了" : edit = (char*)u8"配置開始";
        }
        ImGui::PopStyleColor(2);
    }

    if (ImGui::Button((char*)u8"保存"))
    {
        ObjectSave();
    }
    if (ImGui::Button("Load"))
    {
        ObjectLoad();
    }

    //配置するオブジェクトの登録
    ObjectRegister();

    //配置するオブジェクト選択
    for (auto& objName : placeObjcts)
    {
        if (!objName.second.filePath.empty())
        {
            if (objType == objName.first)
            {
                nowEdit ? ImGui::Button((char*)u8"配置中") : ImGui::Button((char*)u8"配置待機中");
            }
            else
            {
                if (ImGui::Button((char*)u8"配置可能"))
                {
                    objType = objName.first;
                }
            }
        }
        else
        {
            if (ImGui::Button((char*)u8"モデル設定"))
            {
                FileRead(objName.second.filePath);
            }
        }
        ImGui::SameLine();

        if (ImGui::TreeNode(objName.first.c_str()))
        {
            ImGui::Checkbox("Static", &objName.second.staticFlag);
            if (ImGui::Button((char*)u8"当たり判定設定"))
            {
                FileRead(objName.second.collisionPath);
            }
            ImGui::TreePop();
        }
    }
}

void StageEditorCom::ObjectRegister()
{
    if (ImGui::Button("Regist") && registerObjName[0] != '\0')
    {
        std::string copyname;
        copyname = registerObjName;

        placeObjcts[copyname].filePath = "";
        placeObjcts[copyname].collisionPath = "";
        registerObjName[0] = '\0';
    }
    ImGui::SameLine();
    ImGui::InputText("ObjName", registerObjName, sizeof(registerObjName));
}

void StageEditorCom::ObjectPlace(std::string objType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, const char* model_filename, const char* collision_filename)
{
    //オブジェクトを配置
    GameObj obj = GameObjectManager::Instance().Create();
    std::string objName = objType + std::to_string(placeObjcts[objType.c_str()].objList.size());
    obj->SetName(objName.c_str());

    obj->transform_->SetWorldPosition(position);
    obj->transform_->SetRotation(rotation);
    obj->transform_->SetScale(scale);

    obj->AddComponent<NodeCollsionCom>(collision_filename);

    RendererCom* render = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
    render->LoadModel(model_filename);

    placeObjcts[objType.c_str()].objList.emplace_back(obj);
}

void StageEditorCom::FileRead(std::string& path)
{
    //ステージモデルの設定
    const char* filter = "Model Files(*.mdl;*.nodecollsion;)\0*.mdl;*.nodecollsion;\0All Files(*.*)\0*.*;\0\0";
    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
    if (result == DialogResult::OK)
    {
        path = filename;
    }
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

    for (auto& placeObj : placeObjcts)
    {
        j[placeObj.first]["FileName"] = placeObj.second.filePath;
        j[placeObj.first]["CollsionFileName"] = placeObj.second.collisionPath;
        j[placeObj.first]["StaticFlag"] = placeObj.second.staticFlag;

        int i = 0;
        for (auto& obj : placeObj.second.objList)
        {
            j[placeObj.first]["Position"] += { {"x", obj->transform_->GetWorldPosition().x}, { "y", obj->transform_->GetWorldPosition().y }, { "z", obj->transform_->GetWorldPosition().z }};
            j[placeObj.first]["Scale"] += { {"x", obj->transform_->GetScale().x}, { "y", obj->transform_->GetScale().y }, { "z", obj->transform_->GetScale().z }};
            j[placeObj.first]["Rotation"] += { {"x", obj->transform_->GetRotation().x}, { "y", obj->transform_->GetRotation().y }, { "z", obj->transform_->GetRotation().z }, { "w", obj->transform_->GetRotation().w }};

            ++i;
        }
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
            nlohmann::json json;
            ifs >> json;

            for (auto& item : json.items())
            {
                //オブジェクトの内容にアクセス
                const auto& data = item.value();
                placeObjcts[item.key()].staticFlag = data["StaticFlag"];
                placeObjcts[item.key()].collisionPath = data["CollsionFileName"];
                placeObjcts[item.key()].filePath = data["FileName"];

                for (int index = 0; index < data["Position"].size(); ++index)
                {
                    DirectX::XMFLOAT3 pos = { data["Position"].at(index)["x"], data["Position"].at(index)["y"], data["Position"].at(index)["z"] };
                    DirectX::XMFLOAT3 scale = { data["Scale"].at(index)["x"], data["Scale"].at(index)["y"], data["Scale"].at(index)["z"] };
                    DirectX::XMFLOAT4 rotation = { data["Rotation"].at(index)["x"], data["Rotation"].at(index)["y"], data["Rotation"].at(index)["z"], data["Rotation"].at(index)["w"] };

                    ObjectPlace(
                        item.key(),//選択中のオブジェクト
                        pos,       //位置
                        scale,     //スケール
                        rotation,  //回転値
                        placeObjcts[item.key()].filePath.c_str(),    //modelのパス
                        placeObjcts[item.key()].collisionPath.c_str()//nodeCollsionのパス
                    );
                }
            }
        }
    }
}