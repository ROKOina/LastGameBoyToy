#include "shaft_scale_behavior.h"
#ifdef DEBUG
#include "utility\gui_helper.h"
#endif

namespace chimera
{
    void ShaftScaleBehavior::MoveInitialize()
    {
        timer = 0;
        SetScale(start);
        isDecision = false;
    }

    void ShaftScaleBehavior::Update()
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

        SetScale(moveScale);

        SpriteMoveBaseBehavior::FinishJudge();
    }

    void ShaftScaleBehavior::SetStartValue()
    {
        SetScale(start);
    }

    void ShaftScaleBehavior::SetEndValue()
    {
        SetScale(end);
    }

    void ShaftScaleBehavior::SetScale(float scale)
    {
        if (scaleShaft == ScaleShaft::X)
        {
            transform.lock()->SetScaleX(scale);
        }
        if (scaleShaft == ScaleShaft::Y)
        {
            transform.lock()->SetScaleY(scale);
        }
        if (scaleShaft == ScaleShaft::Z)
        {
            transform.lock()->SetScaleZ(scale);
        }
    }

#ifdef DEBUG
    void ShaftScaleBehavior::Editor()
    {
        SpriteMoveBaseBehavior::Editor();
        ImGui::ComboEnum("Shaft", scaleShaft);
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

