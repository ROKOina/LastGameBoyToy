#include "EventSpriteFadeBehavior.h"
#include "Component\Sprite\SpriteCom.h"

void EventSpriteFadeBehavior::EventMoveInitialize()
{
    float setAlpha = (fadeType == FadeType::In) ? (m_reverseEnable) ? 0 : maxAlpha : (m_reverseEnable) ? maxAlpha : 0;
    SetAlpha(setAlpha);

    m_timer = 0;
    m_decisionEnable = false;
}

void EventSpriteFadeBehavior::SetAlpha(float alpha)
{
    auto sprite = m_owner.lock()->GetGameObject()->GetComponent<SpriteCom>();
    sprite->spc.color.w = alpha;
}

void EventSpriteFadeBehavior::Update()
{
    if (m_decisionEnable) return;// 動作が完了したら更新させない

    EventMoveParameterBehaviorBase::TimerUpdate();

    if (m_timer <= 0)return;

    if (m_eventUpdateEnable)
    {
        if (m_timer > m_duration)return;
    }

    float easePos = m_curve.Evaluate(std::min(m_timer / m_duration, 0.99f));
    float moveAlpha = (fadeType == FadeType::In) ?
        (m_reverseEnable) ? Mathf::Lerp(0.0f, maxAlpha, easePos) : Mathf::Lerp(maxAlpha, 0.0f, easePos)
        : (m_reverseEnable) ? Mathf::Lerp(maxAlpha, 0.0f, easePos) : Mathf::Lerp(0.0f, maxAlpha, easePos);

    SetAlpha(moveAlpha);

    EventMoveParameterBehaviorBase::FinishJudge();
}

void EventSpriteFadeBehavior::SetStartValue()
{
    SetAlpha(0);
}

void EventSpriteFadeBehavior::SetEndValue()
{
    SetAlpha(maxAlpha);
}

void EventSpriteFadeBehavior::OnGUI()
{
    EventMoveParameterBehaviorBase::OnGUI();
    ImGui::InputEnum("Fade Type : ", fadeType);
    ImGui::DragFloat("Max Alpha", &maxAlpha);
}
