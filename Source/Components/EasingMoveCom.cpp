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
    // バージョン1には存在しないフィールドにはデフォルト値を与える
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

//コンストラクタ
EasingMoveCom::EasingMoveCom(const char* filename)
{
    //ファイルの名前があれば読み込む
    if (filename)
    {
        Deserialize(filename);
    }
}

//更新処理
void EasingMoveCom::Update(float elapsedTime)
{
    // 遅延を有効にしている場合
    if (EMP.delatimeuse)
    {
        time += elapsedTime;

        // 遅延時間を超えた場合にイージングを開始
        if (time >= EMP.delaytime && !play)
        {
            play = true;
            savepos = GetGameObject()->transform_->GetWorldPosition();
        }
    }

    // イージングが有効な場合
    if (play && !EMP.easingposition.empty())
    {
        // オブジェクトの位置更新
        Object();

        // イージング更新
        easingresult = EasingUpdate(EMP.easingtype[currentTargetIndex], EMP.easingmovetype[currentTargetIndex], easingtime);

        // 現在の目標位置
        DirectX::XMFLOAT3 targetPos = EMP.easingposition[currentTargetIndex];

        // 現在の位置を取得
        DirectX::XMFLOAT3 currentPosition = GetGameObject()->transform_->GetWorldPosition();

        // イージング計算
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

        // 計算結果を反映
        GetGameObject()->transform_->SetWorldPosition(currentPosition);

        // イージング時間の更新
        easingtime += elapsedTime * EMP.timescale[currentTargetIndex];

        // 目標位置到達時のチェック
        if (easingtime >= 1.0f)
        {
            // 現在位置を次のイージング開始位置として保存
            savepos = GetGameObject()->transform_->GetWorldPosition();

            currentTargetIndex++;

            // 目標位置リストを超えた場合、イージング停止
            if (currentTargetIndex >= EMP.easingposition.size())
            {
                StopEasing();
                return;
            }
            easingtime = 0.0f; // 次のポイントへの移行
        }
    }

    //削除
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

    if (ImGui::Button(play ? (char*)u8"停止" : (char*)u8"プレイ"))
    {
        if (play) StopEasing();
        else { savepos = GetGameObject()->transform_->GetWorldPosition(); play = true; }
    }

    char name[256];
    ::strncpy_s(name, sizeof(name), EMP.objectname.c_str(), sizeof(name));
    if (ImGui::InputText((char*)u8"オブジェクトの名前", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        EMP.objectname = name;
    }
    // イージング進捗表示
    ImGui::Text((char*)u8"現在のインデックス: %d", currentTargetIndex);
    ImGui::ProgressBar(easingresult, ImVec2(0.0f, 0.0f), (char*)u8"進行状況");  // 進捗バーを追加

    ImGui::Checkbox((char*)u8"開始時間の遅延", &EMP.delatimeuse);
    ImGui::DragFloat((char*)u8"経過時間", &time);
    ImGui::DragFloat((char*)u8"遅延時間", &EMP.delaytime, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat((char*)u8"追尾時間", &EMP.trackingtime, 0.1f, 0.0f, 1.0f);

    // timescaleとeasingpositionの追加・削除UI
    for (size_t i = 0; i < EMP.timescale.size(); ++i)
    {
        ImGui::PushID(i);

        //イージングも同様です
        easingresult = EasingImGui(EMP.easingtype[i], EMP.easingmovetype[i], easingtime);

        // timescale の UI
        ImGui::DragFloat((char*)u8"イージングの早さ", &EMP.timescale[i], 0.1f, 0.0f, 10.0f);

        // easingposition の UI
        ImGui::DragFloat3((char*)u8"イージング位置", &EMP.easingposition[i].x, 0.1f);

        //フラグ関係
        ImGui::Checkbox((char*)u8"X有効", reinterpret_cast<bool*>(&EMP.FlagX[i]));
        ImGui::SameLine();
        ImGui::Checkbox((char*)u8"Y有効", reinterpret_cast<bool*>(&EMP.FlagY[i]));
        ImGui::SameLine();
        ImGui::Checkbox((char*)u8"Z有効", reinterpret_cast<bool*>(&EMP.FlagZ[i]));

        // 削除ボタン
        if (ImGui::Button((char*)u8"削除"))
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

    // Add ボタン
    if (ImGui::Button((char*)u8"追加"))
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

    if (ImGui::Button((char*)u8"リセット"))
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

//イージング停止
void EasingMoveCom::StopEasing()
{
    play = false;
    stop = true;
    easingtime = 0.0f;
    currentTargetIndex = 0;
    time = 0.0f;

    // 現在の位置を取得
    DirectX::XMFLOAT3 currentPosition = GetGameObject()->transform_->GetWorldPosition();

    // フラグに基づき位置を最終位置でリセット
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

    // 更新後の位置を反映
    GetGameObject()->transform_->SetWorldPosition(currentPosition);

    // 終了コールバックの実行
    if (onFinishCallback) onFinishCallback();
}

//オブジェクトがあれば
void EasingMoveCom::Object()
{
    // オブジェクト名が指定されている場合
    if (!EMP.objectname.empty())
    {
        auto& gameObject = GameObjectManager::Instance().Find(EMP.objectname.c_str());

        if (gameObject != nullptr)
        {
            // ターゲットオブジェクトの位置を取得して目標位置に設定
            DirectX::XMFLOAT3 targetPosition = gameObject->transform_->GetWorldPosition();

            if (currentTargetIndex == EMP.easingposition.size() - 1 && easingtime <= EMP.trackingtime)
            {
                EMP.easingposition[currentTargetIndex] = targetPosition;
            }
        }
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