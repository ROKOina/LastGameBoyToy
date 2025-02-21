#include "EventSpriteToMoveBehavior.h"

void EventSpriteToMoveBehavior::EventMoveInitialize()
{
    m_timer = 0;
    m_decisionEnable = false;
}

void EventSpriteToMoveBehavior::SetPosition(Vector2 setValue)
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

void EventSpriteToMoveBehavior::Update()
{
#ifdef DEBUG
    if (!m_debugEnable)return;
#endif // DEBUG
    //if (m_decisionEnable) return;// “®ì‚ªŠ®—¹‚µ‚½‚çXV‚³‚¹‚È‚¢

    //EventMoveParameterBehaviorBase::TimerUpdate();

    //if (m_timer <= 0)return;

    //if (m_eventUpdateEnable)
    //{
    //    if (m_timer > m_duration)return;
    //}

    //float easePos = m_curve.Evaluate(std::min(m_timer / m_duration, 0.999f));
    //Vector2 movePos = Lerp(m_startValue, m_endValue, easePos);

    //SetPosition(movePos);

    //EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteToMoveBehavior::SetStartValue()
{
    //SetPosition(m_startValue);
}

void EventSpriteToMoveBehavior::SetEndValue()
{
    SetPosition(m_endValue);
}

void EventSpriteToMoveBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::Checkbox("LocalEnable", &m_localEnable);
    auto trans = m_owner.lock()->GetGameObject()->transform_;
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
