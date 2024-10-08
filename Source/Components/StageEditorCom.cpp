#include "StageEditorCom.h"
#include "Input\Input.h"
#include "./Dialog.h"
#include "Graphics/Graphics.h"
#include "imgui.h"
#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "./ColliderCom.h"
#include "Components\RendererCom.h"
#include "Graphics/Model/ResourceManager.h"
#include "Framework.h"
#include "././GameSource/Scene/SceneManager.h"
#include <cstring>
#include "./Components/RayCollisionCom.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "Components\NodeCollsionCom.h"

void StageEditorCom::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    //���N���b�N���z�u�t���O���}�E�X��GUI�̏�ɂȂ���΃I�u�W�F�N�g��u��
    if (Mouse::BTN_LEFT & mouse.GetButtonDown() && nowEdit && onImGui)
    {
        //�}�E�X�ƃX�e�[�W�̓����蔻��
        HitResult hit;
        if (MouseVsStage(hit))
        {
           //�o�^����Ă����I�u�W�F�N�g��z�u
           GameObj obj = GameObjectManager::Instance().Create();
           std::string objName = objType + std::to_string(placeObjcts[objType.c_str()].objList.size());
           obj->SetName(objName.c_str());

           obj->transform_->SetWorldPosition(hit.position);
           obj->transform_->SetScale({ 0.02f,0.02f,0.02f });

           obj->AddComponent<NodeCollsionCom>(nullptr);

           RendererCom* render = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
           render->LoadModel(placeObjcts[objType.c_str()].filePath.c_str());

           placeObjcts[objType.c_str()].objList.emplace_back(obj);
        }
    }
}

void StageEditorCom::OnGUI()
{
    static char* edit = (char*)u8"�z�u�J�n";

    static ImVec4 gui_color = {};

    nowEdit ? gui_color = { 0.7f,0.0f,0.0f,1.0f } : gui_color = { 0.0f,0.0f,0.7f,1.0f };
    ImGui::PushStyleColor(ImGuiCol_Button, gui_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gui_color);

    onImGui = !ImGui::IsWindowHovered();
    if (ImGui::Button(edit, { 400,50 }))
    {
        nowEdit = !nowEdit;
        nowEdit ? edit = (char*)u8"�z�u�I��" : edit = (char*)u8"�z�u�J�n";
    }
    ImGui::PopStyleColor(2);

    if (ImGui::Button((char*)u8"�ۑ�"))
    {
        ObjectSave();
    }
    if (ImGui::Button("Load"))
    {
        ObjectLoad();
    }

    //�z�u����I�u�W�F�N�g�̓o�^
    ObjectRegister();

    //�z�u����I�u�W�F�N�g�I��
    for (auto& objName : placeObjcts)
    {
        ImGui::Text(objName.first.c_str());
        ImGui::SameLine();

        if (!objName.second.filePath.empty())
        {
            if (objType == objName.first)
            {
                nowEdit ? ImGui::Button((char*)u8"�z�u��") : ImGui::Button((char*)u8"�z�u�ҋ@��");
            }
            else 
            {
                if (ImGui::Button((char*)u8"�z�u�\"))
                {
                    objType = objName.first;
                }
            }
        }
        else
        {
            if (ImGui::Button("None"))
            {
                //�X�e�[�W���f���̐ݒ�
                static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

                char filename[256] = { 0 };
                DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
                if (result == DialogResult::OK)
                {
                    objName.second.filePath = filename;
                }
            }
        }
    }
}

void StageEditorCom::StageSelect()
{
    //�{�^���ŃI�[�v��
    //if (ImGui::Button("StageSelect"))
    //{
    //    //�X�e�[�W���f���̐ݒ�
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
    if (ImGui::Button("Regist") && registerObjName[0] != '\0')
    {
        std::string copyname;
        copyname = registerObjName;

        placeObjcts[copyname].filePath = "";
        registerObjName[0] = '\0';
    }
    ImGui::SameLine();
    ImGui::InputText("ObjName", registerObjName, sizeof(registerObjName));
}

void StageEditorCom::ObjectPlace()
{
    //Mouse& mouse = Input::Instance().GetMouse();
    //if (stageObj != nullptr && mouse.GetButtonDown() & Mouse::BTN_LEFT)
    //{
    //    HitResult result;
    //    if (MouseVsStage(result))
    //    {
    //        //�z�u
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

    // �r���[�|�[�g
    D3D11_VIEWPORT view_port;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &view_port);

    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&cameraCom->GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&cameraCom->GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    Mouse& mouse = Input::Instance().GetMouse();

    // �}�E�X�J�[�\�����W���擾
    DirectX::XMFLOAT3 screen_position;
    screen_position.x = static_cast<float>(mouse.GetPositionX());
    screen_position.y = static_cast<float>(mouse.GetPositionY());
    screen_position.z = 0;

    // ���C�̎n�_
    DirectX::XMFLOAT3 world_start;
    DirectX::XMStoreFloat3(&world_start,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	�r���[�|�[�g����X���W
            view_port.TopLeftY,	//	�r���[�|�[�g����Y���W
            view_port.Width,	//	�r���[�|�[�g��
            view_port.Height,	//	�r���[�|�[�g����
            0.0f,	// �[�x�l�̍ŏ��l
            1.0f,	// �[�x�l�̍ő�l
            Projection,	//	�v���W�F�N�V�����s��
            View,	//	�r���[�s��
            World	//	���[���h�s��
        )
    );

    screen_position.z = 1;

    // ���C�̏I�_
    DirectX::XMFLOAT3 world_end;
    DirectX::XMStoreFloat3(&world_end,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	�r���[�|�[�g����X���W
            view_port.TopLeftY,	//	�r���[�|�[�g����Y���W
            view_port.Width,	//	�r���[�|�[�g��
            view_port.Height,	//	�r���[�|�[�g����
            0.0f,	// �[�x�l�̍ŏ��l
            1.0f,	// �[�x�l�̍ő�l
            Projection,	//	�v���W�F�N�V�����s��
            View,	//	�r���[�s��
            World	//	���[���h�s��
        )
    );

    return Collision::IntersectRayVsModel(world_start, world_end, GetGameObject()->GetComponent<RendererCom>()->GetModel(), hit);

}

void StageEditorCom::ObjectSave()
{
    nlohmann::json j;

    for (auto& placeObj : placeObjcts)
    {
        j[placeObj.first]["TypeName"] = placeObj.first;
        j[placeObj.first]["FileName"] = placeObj.second.filePath;

        int i = 0;
        for (auto& obj : placeObj.second.objList)
        {
            j[placeObj.first]["Position"] += { {"x", obj->transform_->GetWorldPosition().x}, { "y", obj->transform_->GetWorldPosition().y }, { "z", obj->transform_->GetWorldPosition().z }};
            j[placeObj.first]["Scale"] += { {"x", obj->transform_->GetScale().x}, { "y", obj->transform_->GetScale().y }, { "z", obj->transform_->GetScale().z }};
            j[placeObj.first]["Rotation"] += { {"x", obj->transform_->GetRotation().x}, { "y", obj->transform_->GetRotation().y }, { "z", obj->transform_->GetRotation().z } , { "w", obj->transform_->GetRotation().w }};

            ++i;
        }
    }

    // ���΃p�X�Ńt�@�C�����쐬
    static const char* filter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "json", Framework::GetInstance()->GetHWND());

    std::ofstream file(filename);

    if (!file.is_open())
    {
        std::cout << "�t�@�C�����J�����Ƃ��ł��܂���ł����B" << std::endl;
    }
    else
    {
        file << std::setw(4) << j;
        std::cout << "JSON�t�@�C�����������܂�܂����B" << std::endl;
    }
}

void StageEditorCom::ObjectLoad()
{
    using namespace std;

    //�X�e�[�W���f���̐ݒ�
    static const char* filter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
    if (result == DialogResult::OK)
    {
        //�t�@�C�����J��
        fstream ifs(filename);
        if (ifs.good())
        {
            //Json�^���擾
            nlohmann::json json;
            ifs >> json;

            for (auto& item : json.items())
            {
                //�I�u�W�F�N�g�̓��e�ɃA�N�Z�X
                const auto& data = item.value();

                for (int index = 0; index < data["Position"].size(); ++index)
                {
                    DirectX::XMFLOAT3 pos      = { data["Position"].at(index)["x"], data["Position"].at(index)["y"], data["Position"].at(index)["z"] };
                    DirectX::XMFLOAT3 scale    = { data["Scale"].at(index)["x"], data["Scale"].at(index)["y"], data["Scale"].at(index)["z"] };
                    DirectX::XMFLOAT4 rotation = { data["Rotation"].at(index)["x"], data["Rotation"].at(index)["y"], data["Rotation"].at(index)["z"], data["Rotation"].at(index)["w"] };

                    GameObj obj = GameObjectManager::Instance().Create();
                    
                    std::string objName = item.key() + std::to_string(placeObjcts[item.key().c_str()].objList.size());
                    obj->SetName(objName.c_str());

                    obj->transform_->SetWorldPosition(pos);
                    obj->transform_->SetRotation(rotation);
                    obj->transform_->SetScale(scale);

                    obj->AddComponent<NodeCollsionCom>(nullptr);

                    RendererCom* render = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
                    std::string filename = data["FileName"];
                    render->LoadModel(filename.c_str());

                    placeObjcts[item.key()].objList.emplace_back(obj);
                }
            }
        }
    }

    
}
