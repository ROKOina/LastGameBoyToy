#include "EventSpriteShaftAngleBehavior.h"

void EventSpriteShaftAngleBehavior::EventMoveInitialize()
{
    m_timer = 0;
    m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(start) });
    m_decisionEnable = false;
}

void EventSpriteShaftAngleBehavior::Update()
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
    float moveAngle = (m_reverseEnable) ? Mathf::Lerp(end, start, easePos) : Mathf::Lerp(start, end, easePos);

    m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(moveAngle) });

    EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteShaftAngleBehavior::SetStartValue()
{
    m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(start) });
}

void EventSpriteShaftAngleBehavior::SetEndValue()
{
    m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(end) });
}

void EventSpriteShaftAngleBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::DragFloat("Start Value", &start);
    if (ImGui::Button("Set Current Angle##1"))
    {
        start = m_owner.lock()->GetGameObject()->transform_->GetEulerRotation().z;
    }
    ImGui::SameLine();
    if (ImGui::Button("Set Valu To Angle##1"))
    {
        m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(start) });
    }

    ImGui::DragFloat("End Value", &end);
    if (ImGui::Button("Set Current Angle##2"))
    {
        end = m_owner.lock()->GetGameObject()->transform_->GetEulerRotation().z;
    }
    ImGui::SameLine();
    if (ImGui::Button("Set Valu To Angle##2"))
    {
        m_owner.lock()->GetGameObject()->transform_->SetEulerRotation({ 0, 0, DirectX::XMConvertToRadians(end) });
    }
}