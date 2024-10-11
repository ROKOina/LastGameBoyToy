#include "EventCameraManager.h"

#include "Components/System/GameObject.h"
#include "Components/CameraCom.h"
#include "Components/TransformCom.h"
#include "GameSource/Math/Mathf.h"
#include "imgui.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include "Graphics/Graphics.h"
#include "Dialog.h"
#include <fstream>
#include "Logger.h"

CEREAL_CLASS_VERSION(EventCameraManager::ECTransform, 1)
CEREAL_CLASS_VERSION(EventCameraManager::SaveEventCameraBuff, 1)

// �V���A���C�Y
namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void EventCameraManager::ECTransform::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(frame),
        CEREAL_NVP(pos)
    );
}

template<class Archive>
void EventCameraManager::SaveEventCameraBuff::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(cameraName),
        CEREAL_NVP(focusObjName),
        CEREAL_NVP(ECTra)
    );
}

//�V���A���C�Y
void EventCameraManager::Serialize()
{
    static const char* filter = "EventCamera Files(*.eventcamera)\0*.eventcamera;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "eventcamera", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(saveEventCameraBuff)
                );
            }
            catch (...)
            {
                LOG("eventcamera serialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//�f�V���A���C�Y
void EventCameraManager::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(saveEventCameraBuff)
            );
        }
        catch (...)
        {
            LOG("eventcamera deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//�f�V���A���C�Y�ǂݍ���
void EventCameraManager::LoadDesirialize()
{
    static const char* filter = "eventcamera Files(*.eventcamera)\0*.eventcamera;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}



void EventCameraManager::EventCameraImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("EventCameraManager", nullptr, ImGuiWindowFlags_None);

    //�V���A���C�Y
    {
        if (ImGui::Button("Save"))
        {
            Serialize();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            LoadDesirialize();
            cameraObj = GameObjectManager::Instance().Find(saveEventCameraBuff.cameraName.c_str());
            focusObj = GameObjectManager::Instance().Find(saveEventCameraBuff.focusObjName.c_str());
        }
    }

    ImGui::Separator();

    //�Đ�
    if (ImGui::Button("Play"))
        PlayEventCamera("debugSaisei");
    ImGui::SameLine();
    ImGui::Checkbox("isPlay", &isEventPlay);
    ImGui::SameLine();
    if (ImGui::DragFloat("timer", &timer, 0.01f, 0))
    {
        PlayCameraLerp();
    }

    ImGui::Separator();

    //�J�����̖��O�ۑ�
    {
        char name[256];
        ::strcpy_s(name, sizeof(name), saveEventCameraBuff.cameraName.c_str());
        if (ImGui::InputText("cameraName", name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            saveEventCameraBuff.cameraName = name;
            //�J�����I�u�W�F�ۑ�
            cameraObj = GameObjectManager::Instance().Find(saveEventCameraBuff.cameraName.c_str());
        }
        //���݂��Ă��邩�m�F
        if (cameraObj.lock())
            if (cameraObj.lock()->GetComponent<CameraCom>())
                ImGui::TextColored(ImVec4(0.0f, 0.6f, 1.0f, 1.0f), "FindObj");
            else
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "NullCameraCom");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "NullObj");

        //�����I�u�W�F�̖��O
        ::strcpy_s(name, sizeof(name), saveEventCameraBuff.focusObjName.c_str());
        if (ImGui::InputText("focusObjName", name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            saveEventCameraBuff.focusObjName = name;
            //�����I�u�W�F�ۑ�
            focusObj = GameObjectManager::Instance().Find(saveEventCameraBuff.focusObjName.c_str());
        }
        //���݂��Ă��邩�m�F
        if (focusObj.lock())
            ImGui::TextColored(ImVec4(0.0f, 0.6f, 1.0f, 1.0f), "FindObj");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "NullObj");
    }

    ImGui::Separator();

    //�t���[���ۑ�
    {
        auto& ecVec = saveEventCameraBuff.ECTra;

        //�ǉ�
        if (ImGui::Button("Add"))
        {
            auto& ec = ecVec.emplace_back();

            if (ecVec.size() > 1)
            {
                const auto oneEc = ecVec[ecVec.size() - 2];  //��O�̃t���[���擾
                ec.frame = oneEc.frame + 0.1f;
            }
            else
            {
                ec.frame = 0.0f;
            }

            ec.pos = FocusFromCameraPos();
        }

        //�|�X�ړ�
        int index = 0;
        int deleteID = -1;
        for (auto& ec : ecVec)
        {
            //�t���[���ړ�
            ImGui::SetNextItemWidth(60);
            if (ImGui::DragFloat(("frame" + std::to_string(index)).c_str(), &ec.frame, 0.1f, 0))
            {
                if (index == 0)
                    ec.frame = 0;
                else
                {
                    //�t���[�������Ԃ悤��
                    int oneID = index - 1;
                    if (oneID >= 0)
                    {
                        if (ecVec[oneID].frame >= ec.frame)
                            ec.frame = ecVec[oneID].frame + 0.1f;
                    }
                    int nextID = index + 1;
                    if (nextID < ecVec.size())
                    {
                        if (ecVec[nextID].frame <= ec.frame)
                            ec.frame = ecVec[nextID].frame - 0.1f;
                    }
                }
            }

            ImGui::SameLine();

            //�|�W�V����
            ImGui::SetNextItemWidth(60 * 3);
            ImGui::DragFloat3(("pos" + std::to_string(index)).c_str(), &ec.pos.x, 0.1f);

            ImGui::SameLine();

            //�f���[�g
            if (ImGui::Button(("delete" + std::to_string(index)).c_str()))
                deleteID = index;

            index++;
        }

        if (deleteID >= 0)
        {
            ecVec.erase(ecVec.begin() + deleteID);
        }
    }

    ImGui::End();

}

void EventCameraManager::EventUpdate(float elapsedTime)
{
    if (!isEventPlay)return;
    if (!cameraObj.lock())return;
    if (!focusObj.lock())return;

    //�Đ�����
    auto& ect = saveEventCameraBuff.ECTra;
    PlayCameraLerp();

    timer += elapsedTime;

    //�I������
    if (timer >= ect[ect.size() - 1].frame)
    {
        isEventPlay = false;
    }
}

void EventCameraManager::PlayEventCamera(std::string eventName)
{
    if (eventName != "debugSaisei") //�f�o�b�O�Đ�����Ȃ��Ƃ��ɒʂ�
    {
        Deserialize(eventName.c_str());
        cameraObj = GameObjectManager::Instance().Find(saveEventCameraBuff.cameraName.c_str());
        focusObj = GameObjectManager::Instance().Find(saveEventCameraBuff.focusObjName.c_str());
    }

    //�����ʒu�Ɉړ�
    if (cameraObj.lock())
        if (saveEventCameraBuff.ECTra.size() >= 2)
            cameraObj.lock()->transform_->SetWorldPosition(FocusFromWorldPos(saveEventCameraBuff.ECTra[0].pos));

    timer = 0;
    isEventPlay = true;
}

DirectX::XMFLOAT3 EventCameraManager::FocusFromCameraPos()
{
    if(!cameraObj.lock())return DirectX::XMFLOAT3(0,0,0);
    if(!focusObj.lock())return DirectX::XMFLOAT3(0,0,0);

    DirectX::XMFLOAT3 cameraPos;
    DirectX::XMFLOAT3 focusPos;

    cameraPos= cameraObj.lock()->transform_->GetWorldPosition();
    focusPos = focusObj.lock()->transform_->GetWorldPosition();


    return focusPos - cameraPos;
}

DirectX::XMFLOAT3 EventCameraManager::FocusFromWorldPos(DirectX::XMFLOAT3 pos)
{
    if (!focusObj.lock())return DirectX::XMFLOAT3(0, 0, 0);

    DirectX::XMFLOAT3 focusPos;

    focusPos = focusObj.lock()->transform_->GetWorldPosition();

    return focusPos - pos;
}

void EventCameraManager::PlayCameraLerp()
{
    auto& ect = saveEventCameraBuff.ECTra;
    if (ect.size() < 2)return;
    for (int i = 0; i < ect.size() - 1; ++i)
    {
        if (ect[i].frame > timer || ect[i + 1].frame < timer)continue;

        float t1 = timer - ect[i].frame;
        float t2 = ect[i + 1].frame - ect[i].frame;

        float t = t1 / t2;

        DirectX::XMFLOAT3 lpos = Mathf::Lerp(ect[i].pos, ect[i + 1].pos, t);

        //�J�����ʒu�ݒ�
        if (cameraObj.lock())
            cameraObj.lock()->transform_->SetWorldPosition(FocusFromWorldPos(lpos));

        //�����_�ݒ�
        if (focusObj.lock())
            cameraObj.lock()->GetComponent<CameraCom>()->SetFocus(
                focusObj.lock()->transform_->GetWorldPosition());

        break;
    }
}
