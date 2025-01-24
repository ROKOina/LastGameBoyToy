#include "TimeManager.h"
#include <imgui.h>

//更新処理
void TimeManager::Update(const float& elapsedTime)
{
    m_effecttime -= elapsedTime * m_timescale;

    if (m_effecttime < 0.0f)
    {
        m_effecttime = 0.0f;
    }

    m_elapsedtime = elapsedTime * m_timescale;
}

//imgui
void TimeManager::ImGui()
{
    ImGui::Begin("TimeManager");
    ImGui::DragFloat("TimeScale", &m_timescale, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("EffectTime", &m_effecttime, 0.1f, 0.0f, 10.0f);

    ImGui::Text("ElapsedTime: %.3f", m_elapsedtime); // 現在の経過時間を表示
    ImGui::End();
}