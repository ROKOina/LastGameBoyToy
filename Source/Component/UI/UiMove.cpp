#include "UiMove.h"
#include "Math\Mathf.h"
#include "Component\System\TransformCom.h"

void UiMove::Update(float elapsedTime)
{
    for (int i = 0; i < (int)SlideType::Max; ++i)
    {
        if (slideDatas[i].slideFlag != nullptr)
        {
            slideDatas[i].slideTimer += elapsedTime;
            if (slideDatas[i].slideTimer >= slideDatas[i].slideTime) slideDatas[i].slideTimer = slideDatas[i].slideTime;

            *slideDatas[i].slideFlag ?
            slideDatas[i].slideValue = Mathf::Lerp(slideDatas[i].origin, slideDatas[i].target, slideDatas[i].slideTimer / slideDatas[i].slideTime) :
            slideDatas[i].slideValue = Mathf::Lerp(slideDatas[i].origin, slideDatas[i].target, 1 - (slideDatas[i].slideTimer / slideDatas[i].slideTime));

            //動いた数値をトランスフォームに入れる
            UseData((SlideType)i);

            if (slideDatas[i].slideTimer >= slideDatas[i].slideTime) { slideDatas[i].slideTimer = slideDatas[i].slideTime; slideDatas[i].slideFlag.reset(); }
        }
    }
}

void UiMove::SlideStart(SlideType type, bool isBack, float time)
{
    if (slideDatas[(int)type].slideFlag != nullptr) { slideDatas[(int)type].slideFlag.reset(); }

    slideDatas[(int)type].slideTime = time;
    slideDatas[(int)type].slideTimer = 0.0f;
    slideDatas[(int)type].slideFlag = std::make_shared<bool>();
    *slideDatas[(int)type].slideFlag = isBack;
}

void UiMove::SlideAllReset()
{
    for (int i = 0; i < (int)SlideType::Max; ++i)
    {
        SlideReset((SlideType)i);
    }
}
void UiMove::SlideReset(SlideType type)
{
    slideDatas[(int)type].slideValue = slideDatas[(int)type].origin;
    UseData(type);
}

float UiMove::GetSlideProgress(SlideType type)
{
    return slideDatas[(int)type].slideTime != 0 ? slideDatas[(int)type].slideTimer / slideDatas[(int)type].slideTime : -1;//移動の進行度
}

void UiMove::UseData(SlideType type)
{
    switch (type)
    {
    case SlideType::Pos:
        GetGameObject()->transform_->SetWorldPosition(slideDatas[(int)type].slideValue);
        break;
    case SlideType::Rote:
        GetGameObject()->transform_->SetEulerRotation(slideDatas[(int)type].slideValue);
        break;
    default:
        break;
    }
}
