#include "EventSpriteShaftScaleBehavior.h"

void EventSpriteShaftScaleBehavior::EventMoveInitialize()
{
    m_timer = 0;
    SetScale(start);
    m_decisionEnable = false;
}

void EventSpriteShaftScaleBehavior::Update()
{
    if (m_decisionEnable) return;// 動作が完了したら更新させない

    EventMoveParameterBehaviorBase::TimerUpdate();

    if (m_timer <= 0)return;

    if (m_eventUpdateEnable)
    {
        if (m_timer > m_duration)return;
    }

    float easePos = m_curve.Evaluate(std::min(m_timer / m_duration, 0.999f));
    float moveScale = (m_reverseEnable) ? Mathf::Lerp(end, start, easePos) : Mathf::Lerp(start, end, easePos);

    SetScale(moveScale);

    EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteShaftScaleBehavior::SetStartValue()
{
    SetScale(start);
}

void EventSpriteShaftScaleBehavior::SetEndValue()
{
    SetScale(end);
}

void EventSpriteShaftScaleBehavior::SetScale(float scale)
{
    auto getScale = m_owner.lock()->GetGameObject()->transform_->GetScale();
    if (scaleShaft == Shaft::X)
    {
        m_owner.lock()->GetGameObject()->transform_->SetScale({ scale,getScale.y,getScale.z });
    }
    if (scaleShaft == Shaft::Y)
    {
        m_owner.lock()->GetGameObject()->transform_->SetScale({ getScale.x,scale,getScale.z });
    }
    if (scaleShaft == Shaft::Z)
    {
        m_owner.lock()->GetGameObject()->transform_->SetScale({ getScale.x,getScale.y,scale });
    }
}

void EventSpriteShaftScaleBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::ComboEnum("Shaft", scaleShaft);
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
