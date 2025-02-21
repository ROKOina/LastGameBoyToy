#pragma once
#include "..//EventMoveBase.h"

class EventSpriteShaftScaleBehavior : public EventMoveParameterBehaviorBase
{
public:
    EDITABLE_ON_GUI(EventSpriteShaftScaleBehavior)

        EventSpriteShaftScaleBehavior() {};
    ~EventSpriteShaftScaleBehavior() {};
private:
    // 初期化処理
    virtual void EventMoveInitialize() override;

public:
    virtual void Update() override;

    virtual void SetStartValue() override;
    virtual void SetEndValue() override;

private:
    void SetScale(float scale);
private:
    Shaft scaleShaft = Shaft::X;

    float start = {};
    float end = {};

private:
    virtual void OnGUI()override;

private:
    friend class cereal::access;
    template <class Archive>
    void serialize(
        Archive& archive, const uint32_t version)
    {
        if (version == 0)
        {
            archive(
                cereal::base_class<EventMoveParameterBehaviorBase>(this),
                start,
                end,
                scaleShaft
            );
        }
    }
};

CEREAL_REGISTER_TYPE(EventSpriteShaftScaleBehavior)
CEREAL_CLASS_VERSION(EventSpriteShaftScaleBehavior, 0)

