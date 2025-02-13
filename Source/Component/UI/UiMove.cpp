#include "UiMove.h"

void UiMove::Update(float elapsedTime)
{
    for (int i = 0; i < (int)SlideType::Max; ++i)
    {
        if (slideDatas[i].slideFlag != nullptr)
        {
            slideDatas[i].slideTimer += elapsedTime;
            if (slideDatas[i].slideTimer >= slideDatas[i].slideTime) slideDatas[i].slideTimer = slideDatas[i].slideTime;

            *slideDatas[i].slideFlag ?
                slideDatas[i].slideValue = Mathf::Lerp(slideDatas[i].origin, slideDatas[i].target, slideDatas[i].slideTimer / slideDatas[i].slideTime) :
                slideDatas[i].slideValue = Mathf::Lerp(slideDatas[i].origin, slideDatas[i].target, 1 - (slideDatas[i].slideTimer / slideDatas[i].slideTime));

            //動いた数値をトランスフォームに入れる
            UseData((SlideType)i);

            if (slideDatas[i].slideTimer >= slideDatas[i].slideTime) { slideDatas[i].slideTimer = slideDatas[i].slideTime; slideDatas[i].slideFlag.reset(); }
        }
    }
}

//Gui
void UiMove::OnGUI()
{
    for (int i = 0; i < (int)SlideType::Max; ++i)
    {
        SlideType type = static_cast<SlideType>(i);
        SlideData& data = slideDatas[i];

        const char* slideTypeName = "";
        switch (type)
        {
        case SlideType::Pos: slideTypeName = "Position"; break;
        case SlideType::Rote: slideTypeName = "Rotation"; break;
        case SlideType::Scale: slideTypeName = "Scale"; break;
        default: break;
        }

        if (ImGui::CollapsingHeader(slideTypeName))
        {
            // スライドフラグ
            bool active = data.slideFlag != nullptr;
            ImGui::Checkbox("Active", &active);
            if (!active && data.slideFlag != nullptr)
            {
                data.slideFlag.reset();
            }
            else if (active && data.slideFlag == nullptr)
            {
                data.slideFlag = std::make_shared<bool>(true);
            }

            // オリジンとターゲット値の設定
            ImGui::Text("Origin");
            ImGui::InputFloat3("##Origin", reinterpret_cast<float*>(&data.origin));

            ImGui::Text("Target");
            ImGui::InputFloat3("##Target", reinterpret_cast<float*>(&data.target));

            // スライドタイムとタイマー
            ImGui::SliderFloat("Slide Time", &data.slideTime, 0.0f, 10.0f, "%.2f s");
            ImGui::SliderFloat("Slide Timer", &data.slideTimer, 0.0f, data.slideTime, "%.2f s");

            // 現在のスライド値
            ImGui::Text("Slide Value");
            ImGui::InputFloat3("##SlideValue", reinterpret_cast<float*>(&data.slideValue), "%.3f", ImGuiInputTextFlags_ReadOnly);

            // 進行状況の表示
            float progress = GetSlideProgress(type);
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), std::to_string(static_cast<int>(progress * 100)).c_str());

            // スライド開始ボタン
            if (ImGui::Button("Start Forward"))
            {
                SlideStart(type, false, data.slideTime);
            }
            ImGui::SameLine();
            if (ImGui::Button("Start Backward"))
            {
                SlideStart(type, true, data.slideTime);
            }

            // スライドリセットボタン
            if (ImGui::Button("Reset Slide"))
            {
                SlideReset(type);
            }
        }
    }

    if (ImGui::Button("Reset All Slides"))
    {
        SlideAllReset();
    }
}

void UiMove::SlideStart(SlideType type, bool isBack, float time)
{
    if (slideDatas[(int)type].slideFlag != nullptr) { slideDatas[(int)type].slideFlag.reset(); }

    slideDatas[(int)type].slideTime = time;
    slideDatas[(int)type].slideTimer = 0.0f;
    slideDatas[(int)type].slideFlag = std::make_shared<bool>();
    *slideDatas[(int)type].slideFlag = isBack;
}

void UiMove::SlideAllReset()
{
    for (int i = 0; i < (int)SlideType::Max; ++i)
    {
        SlideReset((SlideType)i);
    }
}
void UiMove::SlideReset(SlideType type)
{
    slideDatas[(int)type].slideValue = slideDatas[(int)type].origin;
    UseData(type);
}

float UiMove::GetSlideProgress(SlideType type)
{
    return slideDatas[(int)type].slideTime != 0 ? slideDatas[(int)type].slideTimer / slideDatas[(int)type].slideTime : -1;//移動の進行度
}

void UiMove::UseData(SlideType type)
{
    switch (type)
    {
    case SlideType::Pos:
        GetGameObject()->transform_->SetWorldPosition(slideDatas[(int)type].slideValue);
        break;
    case SlideType::Rote:
        GetGameObject()->transform_->SetEulerRotation(slideDatas[(int)type].slideValue);
        break;
    default:
        break;
    }
}