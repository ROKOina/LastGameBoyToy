#include "EventSpriteScaleBehavior.h"

void EventSpriteScaleBehavior::EventMoveInitialize()
{
    m_timer = 0;
    m_owner.lock()->GetGameObject()->transform_->SetScale(start);
    m_decisionEnable = false;
}

void EventSpriteScaleBehavior::Update()
{
#ifdef DEBUG
    if (!m_debugEnable)return;
#endif // DEBUG
    if (m_decisionEnable) return;// 動作が完了したら更新させない

    EventMoveParameterBehaviorBase::TimerUpdate();

    if (m_timer <= 0)return;

    if (m_eventUpdateEnable)
    {
        if (m_timer > m_duration)return;
    }

    float easePos = m_curve.Evaluate(std::min(m_timer / m_duration, 0.999f));
    float moveScale = (m_reverseEnable) ? Mathf::Lerp(end, start, easePos) : Mathf::Lerp(start, end, easePos);

    m_owner.lock()->GetGameObject()->transform_->SetScale(moveScale);

    EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteScaleBehavior::SetStartValue()
{
    m_owner.lock()->GetGameObject()->transform_->SetScale(start);
}

void EventSpriteScaleBehavior::SetEndValue()
{
    m_owner.lock()->GetGameObject()->transform_->SetScale(end);
}

void EventSpriteScaleBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::DragFloat("Start Value", &start);
    if (ImGui::Button("Set Valu To Transform##1"))
    {
        m_owner.lock()->GetGameObject()->transform_->SetScale(start);
    }
    ImGui::DragFloat("End Value", &end);
    if (ImGui::Button("Set Valu To Transform##2"))
    {
        m_owner.lock()->GetGameObject()->transform_->SetScale(end);
    }
}
