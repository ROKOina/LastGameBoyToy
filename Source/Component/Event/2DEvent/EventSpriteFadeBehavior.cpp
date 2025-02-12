#include "fade_behavior.h"
#ifdef DEBUG
#include "utility\gui_helper.h"
#endif
#include "game\sprite_renderer.h"

namespace chimera
{
    void FadeBehavior::MoveInitialize()
    {
        auto sprites = GetOwner()->GetComponentsInChildren<SpriteRenderer>();
        float setAlpha = (fadeType == FadeType::In) ? (this->isReverse) ? 0 : maxAlpha : (this->isReverse) ? maxAlpha : 0;
        for (auto& sprite : sprites)
        {
            sprite->SetAlpha(setAlpha);
        }

        timer = 0;
        isDecision = false;
    }

    void FadeBehavior::Update()
    {
        if (isDecision) return;// 動作が完了したら更新させない

        SpriteMoveBaseBehavior::TimerUpdate();

        if (timer <= 0)return;

        if (eventUpdateEnable)
        {
            if (timer > wateTime)return;
        }

        float easePos = curve.Evaluate(std::min(timer / wateTime, 0.999f));
        float moveAlpha = (fadeType == FadeType::In) ?
              (this->isReverse) ? Lerp(0.0f, maxAlpha, easePos) : Lerp(maxAlpha, 0.0f, easePos)
            : (this->isReverse) ? Lerp(maxAlpha, 0.0f, easePos) : Lerp(0.0f, maxAlpha, easePos);
        
        auto sprites = GetOwner()->GetComponentsInChildren<SpriteRenderer>();
        for (auto& sprite : sprites)
        {
            sprite->SetAlpha(moveAlpha);
        }

        SpriteMoveBaseBehavior::FinishJudge();
    }

    void FadeBehavior::SetStartValue()
    {
        auto sprites = GetOwner()->GetComponentsInChildren<SpriteRenderer>();
        for (auto& sprite : sprites)
        {
            sprite->SetAlpha(0);
        }
    }

    void FadeBehavior::SetEndValue()
    {
        auto sprites = GetOwner()->GetComponentsInChildren<SpriteRenderer>();
        for (auto& sprite : sprites)
        {
            sprite->SetAlpha(maxAlpha);
        }
    }

#ifdef DEBUG
    void FadeBehavior::Editor()
    {
        SpriteMoveBaseBehavior::Editor();
        ImGui::InputEnum("Fade Type : ", fadeType);
        ImGui::DragFloat("Max Alpha", &maxAlpha);
    }
#endif
}
