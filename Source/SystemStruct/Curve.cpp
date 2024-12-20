#include "Curve.h"
#include "Dialog.h"
#include "Logger.h"
#include <cstdlib>
#include <fstream>
#include <imgui.h>
#include <imgui_curve.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

Curve::Curve()
{
    frames.resize(5);
    values.resize(5);
    for (int i = 0; i < 5; i++)
    {
        frames[i] = (float)i / 5;
        values[i] = (float)i / 5;
    }

    curvePoint = 0.0f;
    selectedPoint = -1;
    fitCurveInEditor = false;
    isSelectable = false;
    isHovered = false;
}

Curve::Curve(int count)
{
    frames.resize(count);
    values.resize(count);
    for (int i = 0; i < count; i++)
    {
        float px = (float)i / float(count - 1);
        frames[i] = px / 1;
        values[i] = px / 1;
    }

    curvePoint = 0.0f;
    selectedPoint = -1;
    fitCurveInEditor = false;
    isSelectable = false;
    isHovered = false;
}

bool Curve::ShowGraph(std::string guiname)
{
    bool out = false;
    int changed_idx = -1;

    void* unique_id = reinterpret_cast<void*>(&guiname);
    ImGui::PushID(unique_id);
    if (ImGui::TreeNode(guiname.c_str()))
    {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 20);
        static ImVec2 size(-1, 200);

        ImVec2 points[15];
        assert(frames.size() < _countof(points));
        for (int i = 0; i < frames.size(); ++i)
        {
            points[i].x = frames[i];
            points[i].y = values[i];
        }
        int new_count;
        float last_frame = frames.back();
        int flags = (int)ImGui::CurveEditorFlags::NO_TANGENTS | (int)ImGui::CurveEditorFlags::SHOW_GRID;
        if (fitCurveInEditor)
        {
            flags |= (int)ImGui::CurveEditorFlags::RESET;
            fitCurveInEditor = false;
        }

        isHovered = false;
        changed_idx = ImGui::CurveEditor("curve", (float*)points, frames.size(), _countof(points), size, flags, &new_count, &selectedPoint, curvePoint, &isSelectable, &isHovered);
        if (changed_idx >= 0)
        {
            frames[changed_idx] = points[changed_idx].x;
            values[changed_idx] = points[changed_idx].y;
            frames.back() = last_frame;
            frames[0] = 0;
        }
        if (new_count != frames.size())
        {
            frames.resize(new_count);
            values.resize(new_count);
            for (int i = 0; i < new_count; ++i)
            {
                frames[i] = points[i].x;
                values[i] = points[i].y;
            }
        }
        if (isSelectable)
        {
            for (int i = 0; i < new_count; ++i)
            {
                frames[i] = points[i].x;
                values[i] = points[i].y;
            }
            isSelectable = false;
            out = true;
        }

        ImGui::PopItemWidth();

        if (ImGui::BeginPopupContextItem("curve"))
        {
            if (ImGui::Selectable("Fit data")) fitCurveInEditor = true;

            ImGui::EndPopup();
        }

        bool changed = false;
        if (selectedPoint >= 0 && selectedPoint < frames.size())
        {
            ImGui::Label("Frame");
            ImGui::DragFloat("##frame", &curvePoint, 0.005f, 0.0f);
            ImVec2 value_you_care_about;
            value_you_care_about.x = ImGui::CurveValue(curvePoint, frames.size(), points);
            value_you_care_about.y = ImGui::CurveValueSmooth(curvePoint, frames.size(), points);
            ImGui::Label("FrameValue:(normal,smooth)");
            ImGui::InputFloat2("##frameval", &value_you_care_about.x);
        }

        ImGui::HSplitter("sizer", &size);

        HWND hwnd = NULL;
        static const char* filter = "Curve(*.curve)\0*.curve;\0All Files(*.*)\0*.*;\0\0";

        if (ImGui::Button("Load"))
        {
            char filename[256] = { 0 };
            DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, "Curveデータ読み込み", hwnd);
            if (result == DialogResult::OK)
            {
                Deserialize(filename);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            char filename[256] = { 0 };
            DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, "Curveデータ保存", ".curve", hwnd);
            if (result == DialogResult::OK)
            {
                Serialize(filename, true);
            }
        }

        // ファイルパスを表示
        char filename[256];
        ::strncpy_s(filename, sizeof(filename), filepath.c_str(), sizeof(filename));
        if (ImGui::InputText((char*)u8"ファイルパス", filename, sizeof(filename), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            filepath = filename;
        }

        auto f = ImGui::GetCurrentWindow()->Flags;

        if (isHovered)ImGui::GetCurrentWindow()->Flags = f | ImGuiWindowFlags_NoScrollWithMouse;
        else ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::TreePop();
    }
    ImGui::PopID();

    return out || (changed_idx >= 0);
}

void Curve::LoadCurve(std::string filename)
{
    std::string name = filename;
    Deserialize(name.c_str());
}

float Curve::Evaluate(float time, bool smooth)
{
    ImVec2 points[15];
    assert(frames.size() < _countof(points));
    for (int i = 0; i < frames.size(); ++i)
    {
        points[i].x = frames[i];
        points[i].y = values[i];
    }

    return smooth ? ImGui::CurveValueSmooth(time, frames.size(), points) : ImGui::CurveValue(time, frames.size(), points);
}

// CEREALバージョン定義
CEREAL_CLASS_VERSION(Curve, 1)

// シリアライズ
void Curve::Serialize(const char* filename, bool ascii)
{
    try
    {
        // JSON
        if (ascii)
        {
            std::ofstream stream(filename);
            if (stream.is_open())
            {
                cereal::JSONOutputArchive archive(stream);

                archive(CEREAL_NVP(frames));
                archive(CEREAL_NVP(values));

                LOG("Curveデータの保存(JSON)に成功しました。 [ファイル] %s\n", filename)
            }
        }
        // バイナリ
        else
        {
            std::ofstream stream(filename, std::ios::binary);
            if (stream.is_open())
            {
                cereal::BinaryOutputArchive archive(stream);

                archive(CEREAL_NVP(frames));
                archive(CEREAL_NVP(values));

                LOG("Curveデータの保存(Binary)に成功しました。 [ファイル] %s\n", filename)
            }
        }
    }
    catch (std::exception& e)
    {
        LOG("Curveデータの保存に失敗しました。[理由] %s [ファイル] %s\n", e.what(), filename)
    }
}

// デシリアライズ
bool Curve::Deserialize(const char* filename)
{
    std::ifstream stream(filename, std::ios::binary);

    if (stream.is_open())
    {
        if (stream.is_open())
        {
            if (stream.peek() == '{')
            {
                cereal::JSONInputArchive archive(stream);

                archive(CEREAL_NVP(frames));
                archive(CEREAL_NVP(values));

                LOG("Curveデータの読み込みに成功しました。\n")
            }
            else
            {
                cereal::BinaryInputArchive archive(stream);

                archive(CEREAL_NVP(frames));
                archive(CEREAL_NVP(values));

                LOG("Curveデータの読み込みに成功しました。\n")
            }

            filepath = filename;
        }
        else
        {
            LOG("Curveデータの読み込みに失敗しました。\n%s\n", filename)
                return false;
        }
    }
    else
    {
        LOG("Curveデータの読み込みに失敗しました。[ファイル] %s\n%s\n", filename)
            return false;
    }

    return true;
}