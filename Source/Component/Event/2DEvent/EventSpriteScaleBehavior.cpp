#include "scale_up_down_behavior.h"

namespace chimera
{
    void ScaleUpDownBehavior::MoveInitialize()
    {
        timer = 0;
        transform.lock()->SetScale(start);
        isDecision = false;
    }

    void ScaleUpDownBehavior::Update()
    {
        if (isDecision) return;// 動作が完了したら更新させない

        SpriteMoveBaseBehavior::TimerUpdate();

        if (timer <= 0)return;

        if (eventUpdateEnable)
        {
            if (timer > wateTime)return;
        }

        float easePos = curve.Evaluate(std::min(timer / wateTime, 0.999f));
        float moveScale = (isReverse) ? Lerp(end, start, easePos) : Lerp(start, end, easePos);

        transform.lock()->SetScale(moveScale);

        SpriteMoveBaseBehavior::FinishJudge();
    }

    void ScaleUpDownBehavior::SetStartValue()
    {
        transform.lock()->SetScale(start);
    }

    void ScaleUpDownBehavior::SetEndValue()
    {
        transform.lock()->SetScale(end);
    }

#ifdef DEBUG
    void ScaleUpDownBehavior::Editor()
    {
        SpriteMoveBaseBehavior::Editor();
        ImGui::DragFloat("Start Value", &start);
        if (ImGui::Button("Set Valu To Transform##1"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            rectTrans->SetScale(start);
        }
        ImGui::DragFloat("End Value", &end);
        if (ImGui::Button("Set Valu To Transform##2"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            rectTrans->SetScale(end);
        }
    }
#endif
}