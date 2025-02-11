#include "designation_angle_behavior.h"

namespace chimera
{
    void DesignationAngleBehavior::MoveInitialize()
    {
        timer = 0;
        if(transform.lock())
        transform.lock()->SetRotationEuler({ 0, 0, DirectX::XMConvertToRadians(start) });
        isDecision = false;
    }

    void DesignationAngleBehavior::Update()
    {
        if (isDecision) return;// 動作が完了したら更新させない

        SpriteMoveBaseBehavior::TimerUpdate();

        if (timer <= 0)return;

        if (eventUpdateEnable)
        {
            if (timer > wateTime)return;
        }

        float easePos = curve.Evaluate(std::min(timer / wateTime, 0.999f));
        float moveAngle = (isReverse) ? Lerp(end, start, easePos) : Lerp(start, end, easePos);

        transform.lock()->SetRotationEuler({ 0, 0, DirectX::XMConvertToRadians(moveAngle) });

        SpriteMoveBaseBehavior::FinishJudge();
    }

    void DesignationAngleBehavior::SetStartValue()
    {
        transform.lock()->SetRotationEuler({ 0, 0, DirectX::XMConvertToRadians(start) });
    }

    void DesignationAngleBehavior::SetEndValue()
    {
        transform.lock()->SetRotationEuler({ 0, 0, DirectX::XMConvertToRadians(end) });
    }

#ifdef DEBUG
    void DesignationAngleBehavior::Editor()
    {
        SpriteMoveBaseBehavior::Editor();
        ImGui::DragFloat("Start Value", &start);
        if (ImGui::Button("Set Current Pos##1"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            start = rectTrans->GetRotationEuler().z;
        }
        ImGui::SameLine();
        if (ImGui::Button("Set Valu To Transform##1"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            rectTrans->SetRotationEuler(Vector3(0, 0, DirectX::XMConvertToRadians(start)));
        }

        ImGui::DragFloat("End Value", &end);
        if (ImGui::Button("Set Current Pos##2"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            end = rectTrans->GetRotationEuler().z;
        }
        ImGui::SameLine();
        if (ImGui::Button("Set Valu To Transform##2"))
        {
            auto rectTrans = GetOwner()->GetComponent<RectTransform>();
            rectTrans->SetRotationEuler(Vector3(0, 0, DirectX::XMConvertToRadians(end)));
        }
    }
#endif
}