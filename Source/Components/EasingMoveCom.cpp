#include "EasingMoveCom.h"

#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include "Dialog.h"
#include "Logger.h"
#include "Graphics/Graphics.h"
#include "GameSource/Math/easing.h"
#include "GameSource/Math/Mathf.h"
#include "Components\TransformCom.h"

CEREAL_CLASS_VERSION(EasingMoveCom::EasingMoveParameter, 1)

// シリアライズ
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
        CEREAL_NVP(timescale),
        CEREAL_NVP(easingtype),
        CEREAL_NVP(easingmovetype),
        CEREAL_NVP(loop),
        CEREAL_NVP(comback),
        CEREAL_NVP(easingposition),
        CEREAL_NVP(easingscale),
        CEREAL_NVP(objectname)
    );
}

//コンストラクタ
EasingMoveCom::EasingMoveCom(const char* filename)
{
    //ファイルの名前があれば読み込む
    if (filename)
    {
        Deserialize(filename);
    }
}

//初期設定
void EasingMoveCom::Start()
{
}

//更新処理
void EasingMoveCom::Update(float elapsedTime)
{
    // イージングが有効な場合
    if (play)
    {
        //イージング更新
        easingresult = EasingUpdate(EMP.easingtype, EMP.easingmovetype, easingtime);

        //オブジェクトがあれば
        if (!EMP.objectname.empty())
        {
            auto& gameObject = GameObjectManager::Instance().Find(EMP.objectname.c_str());

            if (gameObject != nullptr && !one)
            {
                EMP.easingposition = gameObject->transform_->GetWorldPosition();
                one = true;
            }
        }

        // イージング計算
        GetGameObject()->transform_->SetWorldPosition(Mathf::Lerp(savepos, EMP.easingposition, easingresult));
        //GetGameObject()->transform_->SetScale(Mathf::Lerp(savescale, EMP.easingscale, easingresult));

        // イージング時間の更新
        easingtime += (loop ? -1.0f : 1.0f) * elapsedTime * EMP.timescale;

        // イージング時間の範囲チェック
        if (easingtime > 1.0f)
        {
            // ループまたは戻り値が有効な場合
            if (EMP.loop || EMP.comback)
            {
                loop = !loop;
                easingtime = 1.0f;
            }
            else
            {
                StopEasing(); // イージングを停止
            }
        }
        else if (easingtime < 0.0f)
        {
            // ループが有効な場合
            if (EMP.loop)
            {
                loop = !loop;
                easingtime = 0.0f;
            }
            else
            {
                StopEasing(); // イージングを停止
            }
        }
    }
}

//imgui
void EasingMoveCom::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"保存"))
    {
        StopEasing();
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"読み込み"))
    {
        LoadDeserialize();
    }
    easingresult = EasingImGui(EMP.easingtype, EMP.easingmovetype, easingtime);
    if (ImGui::Button("Play") && !play)
    {
        savepos = GetGameObject()->transform_->GetWorldPosition();
        savescale = GetGameObject()->transform_->GetScale();
        play = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        StopEasing();
    }
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"ループ再生", &EMP.loop);
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"ワンカット再生", &EMP.comback);

    //オブジェクトの名前をコピー
    char name[256];
    ::strncpy_s(name, sizeof(name), EMP.objectname.c_str(), sizeof(name));
    if (ImGui::InputText((char*)u8"オブジェクトの名前", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        EMP.objectname = name;
    }

    ImGui::DragFloat("timescale", &EMP.timescale, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat3("easingpostion", &EMP.easingposition.x, 0.1f);
    ImGui::DragFloat3("easingscale", &EMP.easingscale.x, 0.1f);
}

//イージング停止
void EasingMoveCom::StopEasing()
{
    play = false;
    loop = false;
    one = false;
    easingtime = 0.0f;

    GetGameObject()->transform_->SetWorldPosition(savepos);
    //GetGameObject()->transform_->SetScale(savescale);

    if (!EMP.loop && !EMP.comback)
    {
        //今の位置を保存した位置に代入する
        GetGameObject()->transform_->SetWorldPosition(EMP.easingposition);
        //GetGameObject()->transform_->SetScale(EMP.easingscale);
    }
}

//シリアライズ
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

// デシリアライズ
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

// デシリアライズの読み込み
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