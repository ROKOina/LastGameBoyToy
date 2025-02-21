#include "EventSpriteMoveBehavior.h"

void EventSpriteMoveBehavior::EventMoveInitialize()
{
    m_timer = 0;
    SetPosition(m_startValue);
    m_decisionEnable = false;
}

void EventSpriteMoveBehavior::SetPosition(Vector2 setValue)
{
    if (m_localEnable)
    {
        m_owner.lock()->GetGameObject()->transform_->SetLocalPosition({ setValue.x, setValue.y, 0 });
    }
    else
    {
        m_owner.lock()->GetGameObject()->transform_->SetWorldPosition({ setValue.x, setValue.y, 0 });
    }
}

void EventSpriteMoveBehavior::Update()
{
    if (m_decisionEnable) return;// 動作が完了したら更新させない

    EventMoveParameterBehaviorBase::TimerUpdate();

    if (m_timer <= 0)return;

    if (m_eventUpdateEnable)
    {
        if (m_timer > m_duration)return;
    }

    float easePos = m_curve.Evaluate(std::min(m_timer / m_duration, 0.999f));
    Vector2 movePos = (m_reverseEnable) ? Lerp(m_endValue, m_startValue, easePos) : Lerp(m_startValue, m_endValue, easePos);

    SetPosition(movePos);

    EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteMoveBehavior::SetStartValue()
{
    SetPosition(m_startValue);
}

void EventSpriteMoveBehavior::SetEndValue()
{
    SetPosition(m_endValue);
}

void EventSpriteMoveBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::Checkbox("LocalEnable", &m_localEnable);
    ImGui::DragFloat2("Start Value", &m_startValue.x);
    auto trans = m_owner.lock()->GetGameObject()->transform_;
    if (ImGui::Button("Set Current Pos##1"))
    {
        if (m_localEnable)
        {
            m_startValue = Vector2(trans->GetLocalPosition().x, trans->GetLocalPosition().y);
        }
        else
        {
            m_startValue = Vector2(trans->GetWorldPosition().x, trans->GetWorldPosition().y);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Set Valu To Transform##1"))
    {
        SetPosition(m_startValue);
    }
    ImGui::DragFloat2("End Value", &m_endValue.x);
    if (ImGui::Button("Set Current Pos##2"))
    {
        if (m_localEnable)
        {
            m_endValue = Vector2(trans->GetLocalPosition().x, trans->GetLocalPosition().y);
        }
        else
        {
            m_endValue = Vector2(trans->GetWorldPosition().x, trans->GetWorldPosition().y);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Set Valu To Transform##2"))
    {
        SetPosition(m_endValue);
    }
}