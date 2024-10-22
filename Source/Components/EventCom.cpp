#include "EventCom.h"

//初期化
EventCom::EventCom(const char* filename)
{
}

//初期設定
void EventCom::Start()
{
}

//更新処理
void EventCom::Update(float elapsedTime)
{
    for (auto& event : eventList)
    {
        if (event.active && !event.finished)
        {
            // 開始前なら更新しない
            if (event.totaltime < event.starttime) continue;

            // イベント進行中
            event.totaltime += elapsedTime;
            for (auto& event : eventList)
            {
                if (!event.active)continue;// イベントが無効または終了している場合は更新しない

                if (event.totaltime < event.starttime)continue;// 開始前なら状態を更新し、次のイベントへ

                // イベント進行中
                event.totaltime += elapsedTime;

                // イベント終了判定
                if (event.totaltime >= event.endtime)
                {
                    if (event.loop)  // ループならリセット
                    {
                        event.totaltime = event.starttime;
                    }
                    else
                    {
                        event.totaltime = event.endtime;  // 完了状態の場合、合計時間を終了時間に設定
                    }
                }
            }

            // イベント終了判定
            if (event.totaltime >= event.endtime)
            {
                if (event.loop)  // ループならリセット
                {
                    event.totaltime = event.starttime;
                }
                else
                {
                    event.finished = true;
                }
            }
        }
    }
}

//imgui
void EventCom::OnGUI()
{
    for (int i = 0; i < eventList.size(); ++i)
    {
        EventParameter& event = eventList[i];
        std::string eventID = "##Event" + std::to_string(i);

        // イベント名表示
        ImGui::Text(event.eventname.c_str(), i + 1);

        // イベント名編集
        char eventNameBuffer[256];
        strncpy_s(eventNameBuffer, sizeof(eventNameBuffer), event.eventname.c_str(), _TRUNCATE);
        if (ImGui::InputText(("Event Name" + eventID).c_str(), eventNameBuffer, sizeof(eventNameBuffer)))
        {
            event.eventname = eventNameBuffer;
        }

        // 時間パラメータの編集
        ImGui::InputFloat(("Start Time" + eventID).c_str(), &event.starttime);
        ImGui::InputFloat(("End Time" + eventID).c_str(), &event.endtime);
        ImGui::InputFloat(("Total Time" + eventID).c_str(), &event.totaltime);

        // イベントの状態
        ImGui::Checkbox(("Active" + eventID).c_str(), &event.active);
        ImGui::SameLine();
        ImGui::Checkbox(("Loop" + eventID).c_str(), &event.loop);
        ImGui::SameLine();
        ImGui::Checkbox(("Finish" + eventID).c_str(), &event.finished);

        // イベントの状態を表示
        if (event.finished)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: Finished");
        }
        else if (event.totaltime >= event.starttime && event.totaltime < event.endtime)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: In Progress");
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: Not Started");
        }

        // 削除ボタン
        if (ImGui::Button(("Delete" + eventID).c_str()))
        {
            eventList.erase(eventList.begin() + i);
            i--;  // イベントが削除されたため、インデックスを調整
        }

        ImGui::SameLine();

        // コピー＆ペーストボタン
        if (ImGui::Button(("Copy" + eventID).c_str()))
        {
            eventList.push_back(event);  // イベントをコピー
        }

        ImGui::Separator();
    }

    // イベントの追加
    if (ImGui::Button("Add Event"))
    {
        EventParameter newEvent;
        newEvent.eventname = "New Event";
        eventList.push_back(newEvent);
    }
}