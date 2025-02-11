#include "EventMoveBase.h"
#include <SystemStruct\TimeManager.h>
#include <Input\Input.h>
#include <Graphics\Graphics.h>

void EventMoveParameterBehaviorBase::TimerUpdate()
{
    // イベントマネージャーに任せる
    if (m_eventUpdateEnable)return;

    TimeManager& timeManager = TimeManager::Instance();
    if (m_useUnscaledEnable) m_timer += 1.0f * timeManager.GetElapsedTime();
    else m_timer += 1.0f * timeManager.GetDeltaTime();
}

void EventMoveParameterBehaviorBase::FinishJudge()
{
    // イベントマネージャーに任せる
    if (m_eventUpdateEnable)return;
    if (m_timer < m_duration)return;
    m_decisionEnable = true;

    if (m_loopEnable)
    {
        m_reverseEnable = false;
        EventMoveInitialize();
    }
}

void EventMoveParameterBehaviorBase::Start()
{
    if (!m_decisionEnable)
    {
        if (m_startInitializeEnable)
        {
            EventMoveInitialize();
        }
    }
}

void EventMoveParameterBehaviorBase::OnGUI()
{
    ImGui::Text((char*)u8"EventUpdateEnable：GUI上であまり変更しないで下さい");
    ImGui::Checkbox("EventUpdateEnable", &m_eventUpdateEnable);
    ImGui::Checkbox("StartInitializeEnable", &m_startInitializeEnable);
    ImGui::Checkbox("Is Auto Initialize", &m_autoInitializeEnable);
    if (!m_eventUpdateEnable)
    {
        if (ImGui::Button("Initialize"))
        {
            EventMoveInitialize();
        }
        ImGui::Checkbox("UnscaledEnable", &m_useUnscaledEnable);
        ImGui::Checkbox("ReverseEnable", &m_reverseEnable);
        ImGui::DragFloat("Change Duration", &m_duration);
        ImGui::Checkbox("Loop", &m_loopEnable);
    }

    m_curve.ShowGraph("Ease Curve");
}

void EventMoveDefaultValueBase::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();

    ImGui::DragFloat("Start Value", &m_startValue);
    ImGui::DragFloat("End Value", &m_endValue);
}

