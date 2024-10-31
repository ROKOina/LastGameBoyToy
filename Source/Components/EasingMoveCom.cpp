#include "EasingMoveCom.h"
#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal\types\vector.hpp>
#include "Dialog.h"
#include "Logger.h"
#include "Graphics/Graphics.h"
#include "GameSource/Math/easing.h"
#include "GameSource/Math/Mathf.h"
#include "Components\TransformCom.h"

CEREAL_CLASS_VERSION(EasingMoveCom::EasingMoveParameter, 2)

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
void EasingMoveCom::EasingMoveParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(filename),
        CEREAL_NVP(delaytime),
        CEREAL_NVP(delatimeuse),
        CEREAL_NVP(timescale),
        CEREAL_NVP(easingtype),
        CEREAL_NVP(easingmovetype),
        CEREAL_NVP(easingposition),
        CEREAL_NVP(FlagX),
        CEREAL_NVP(FlagY),
        CEREAL_NVP(FlagZ),
        CEREAL_NVP(objectname)
    );
    // �o�[�W����1�ɂ͑��݂��Ȃ��t�B�[���h�ɂ̓f�t�H���g�l��^����
    if (version == 1)
    {
        trackingtime = 0.0f;
    }
    if (version >= 2)
    {
        archive
        (
            CEREAL_NVP(trackingtime)
        );
    }
}

//�R���X�g���N�^
EasingMoveCom::EasingMoveCom(const char* filename)
{
    //�t�@�C���̖��O������Γǂݍ���
    if (filename)
    {
        Deserialize(filename);
    }
}

//�X�V����
void EasingMoveCom::Update(float elapsedTime)
{
    // �x����L���ɂ��Ă���ꍇ
    if (EMP.delatimeuse)
    {
        time += elapsedTime;

        // �x�����Ԃ𒴂����ꍇ�ɃC�[�W���O���J�n
        if (time >= EMP.delaytime && !play)
        {
            play = true;
            savepos = GetGameObject()->transform_->GetWorldPosition();
        }
    }

    // �C�[�W���O���L���ȏꍇ
    if (play && !EMP.easingposition.empty())
    {
        // �I�u�W�F�N�g�̈ʒu�X�V
        Object();

        // �C�[�W���O�X�V
        easingresult = EasingUpdate(EMP.easingtype[currentTargetIndex], EMP.easingmovetype[currentTargetIndex], easingtime);

        // ���݂̖ڕW�ʒu
        DirectX::XMFLOAT3 targetPos = EMP.easingposition[currentTargetIndex];

        // ���݂̈ʒu���擾
        DirectX::XMFLOAT3 currentPosition = GetGameObject()->transform_->GetWorldPosition();

        // �C�[�W���O�v�Z
        if (EMP.FlagX[currentTargetIndex] == 1)
        {
            currentPosition.x = Mathf::Lerp(savepos.x, targetPos.x, easingresult);
        }
        if (EMP.FlagY[currentTargetIndex] == 1)
        {
            currentPosition.y = Mathf::Lerp(savepos.y, targetPos.y, easingresult);
        }
        if (EMP.FlagZ[currentTargetIndex] == 1)
        {
            currentPosition.z = Mathf::Lerp(savepos.z, targetPos.z, easingresult);
        }

        // �v�Z���ʂ𔽉f
        GetGameObject()->transform_->SetWorldPosition(currentPosition);

        // �C�[�W���O���Ԃ̍X�V
        easingtime += elapsedTime * EMP.timescale[currentTargetIndex];

        // �ڕW�ʒu���B���̃`�F�b�N
        if (easingtime >= 1.0f)
        {
            // ���݈ʒu�����̃C�[�W���O�J�n�ʒu�Ƃ��ĕۑ�
            savepos = GetGameObject()->transform_->GetWorldPosition();

            currentTargetIndex++;

            // �ڕW�ʒu���X�g�𒴂����ꍇ�A�C�[�W���O��~
            if (currentTargetIndex >= EMP.easingposition.size())
            {
                StopEasing();
                return;
            }
            easingtime = 0.0f; // ���̃|�C���g�ւ̈ڍs
        }
    }

    //�폜
    if (stop)
    {
        deletetime += elapsedTime;
        if (deletetime > 0.7f)
        {
            GameObjectManager::Instance().Remove(GetGameObject());
        }
    }
}

//imgui
void EasingMoveCom::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"�ۑ�"))
    {
        StopEasing();
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"�ǂݍ���"))
    {
        LoadDeserialize();
    }

    if (ImGui::Button(play ? (char*)u8"��~" : (char*)u8"�v���C"))
    {
        if (play) StopEasing();
        else { savepos = GetGameObject()->transform_->GetWorldPosition(); play = true; }
    }

    char name[256];
    ::strncpy_s(name, sizeof(name), EMP.objectname.c_str(), sizeof(name));
    if (ImGui::InputText((char*)u8"�I�u�W�F�N�g�̖��O", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        EMP.objectname = name;
    }
    // �C�[�W���O�i���\��
    ImGui::Text((char*)u8"���݂̃C���f�b�N�X: %d", currentTargetIndex);
    ImGui::ProgressBar(easingresult, ImVec2(0.0f, 0.0f), (char*)u8"�i�s��");  // �i���o�[��ǉ�

    ImGui::Checkbox((char*)u8"�J�n���Ԃ̒x��", &EMP.delatimeuse);
    ImGui::DragFloat((char*)u8"�o�ߎ���", &time);
    ImGui::DragFloat((char*)u8"�x������", &EMP.delaytime, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat((char*)u8"�ǔ�����", &EMP.trackingtime, 0.1f, 0.0f, 1.0f);

    // timescale��easingposition�̒ǉ��E�폜UI
    for (size_t i = 0; i < EMP.timescale.size(); ++i)
    {
        ImGui::PushID(i);

        //�C�[�W���O�����l�ł�
        easingresult = EasingImGui(EMP.easingtype[i], EMP.easingmovetype[i], easingtime);

        // timescale �� UI
        ImGui::DragFloat((char*)u8"�C�[�W���O�̑���", &EMP.timescale[i], 0.1f, 0.0f, 10.0f);

        // easingposition �� UI
        ImGui::DragFloat3((char*)u8"�C�[�W���O�ʒu", &EMP.easingposition[i].x, 0.1f);

        //�t���O�֌W
        ImGui::Checkbox((char*)u8"X�L��", reinterpret_cast<bool*>(&EMP.FlagX[i]));
        ImGui::SameLine();
        ImGui::Checkbox((char*)u8"Y�L��", reinterpret_cast<bool*>(&EMP.FlagY[i]));
        ImGui::SameLine();
        ImGui::Checkbox((char*)u8"Z�L��", reinterpret_cast<bool*>(&EMP.FlagZ[i]));

        // �폜�{�^��
        if (ImGui::Button((char*)u8"�폜"))
        {
            EMP.timescale.erase(EMP.timescale.begin() + i);
            EMP.easingposition.erase(EMP.easingposition.begin() + i);
            EMP.easingtype.erase(EMP.easingtype.begin() + i);
            EMP.easingmovetype.erase(EMP.easingmovetype.begin() + i);
            EMP.FlagX.erase(EMP.FlagX.begin() + i);
            EMP.FlagY.erase(EMP.FlagY.begin() + i);
            EMP.FlagZ.erase(EMP.FlagZ.begin() + i);
            i--;
        }

        ImGui::PopID();
    }

    // Add �{�^��
    if (ImGui::Button((char*)u8"�ǉ�"))
    {
        EMP.timescale.push_back(0.0f);
        EMP.easingposition.push_back({ 0.0f, 0.0f, 0.0f });
        EMP.easingtype.push_back(static_cast<int>(EaseType::Linear));
        EMP.easingmovetype.push_back(static_cast<int>(EaseInOutType::In));
        EMP.FlagX.push_back(true);
        EMP.FlagY.push_back(true);
        EMP.FlagZ.push_back(true);
    }

    ImGui::SameLine();

    if (ImGui::Button((char*)u8"���Z�b�g"))
    {
        EMP.timescale.clear();
        EMP.easingposition.clear();
        EMP.easingtype.clear();
        EMP.easingmovetype.clear();
        EMP.FlagX.clear();
        EMP.FlagY.clear();
        EMP.FlagZ.clear();
    }
}

//�C�[�W���O��~
void EasingMoveCom::StopEasing()
{
    play = false;
    stop = true;
    easingtime = 0.0f;
    currentTargetIndex = 0;
    time = 0.0f;

    // ���݂̈ʒu���擾
    DirectX::XMFLOAT3 currentPosition = GetGameObject()->transform_->GetWorldPosition();

    // �t���O�Ɋ�Â��ʒu���ŏI�ʒu�Ń��Z�b�g
    if (EMP.FlagX[currentTargetIndex] == 1)
    {
        currentPosition.x = savepos.x;
    }
    if (EMP.FlagY[currentTargetIndex] == 1)
    {
        currentPosition.y = savepos.y;
    }
    if (EMP.FlagZ[currentTargetIndex] == 1)
    {
        currentPosition.z = savepos.z;
    }

    // �X�V��̈ʒu�𔽉f
    GetGameObject()->transform_->SetWorldPosition(currentPosition);

    // �I���R�[���o�b�N�̎��s
    if (onFinishCallback) onFinishCallback();
}

//�I�u�W�F�N�g�������
void EasingMoveCom::Object()
{
    // �I�u�W�F�N�g�����w�肳��Ă���ꍇ
    if (!EMP.objectname.empty())
    {
        auto& gameObject = GameObjectManager::Instance().Find(EMP.objectname.c_str());

        if (gameObject != nullptr)
        {
            // �^�[�Q�b�g�I�u�W�F�N�g�̈ʒu���擾���ĖڕW�ʒu�ɐݒ�
            DirectX::XMFLOAT3 targetPosition = gameObject->transform_->GetWorldPosition();

            if (currentTargetIndex == EMP.easingposition.size() - 1 && easingtime <= EMP.trackingtime)
            {
                EMP.easingposition[currentTargetIndex] = targetPosition;
            }
        }
    }
}

//�V���A���C�Y
void EasingMoveCom::Serialize()
{
    static const char* filter = "EasingMove Files(*.easingmove)\0*.easingmove;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "easingmove", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(EMP)
                );
            }
            catch (...)
            {
                LOG("easingmove deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

// �f�V���A���C�Y
void EasingMoveCom::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(EMP)
            );
        }
        catch (...)
        {
            LOG("easingmove deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

// �f�V���A���C�Y�̓ǂݍ���
void EasingMoveCom::LoadDeserialize()
{
    static const char* filter = "EasingMove Files(*.easingmove)\0*.easingmove;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}