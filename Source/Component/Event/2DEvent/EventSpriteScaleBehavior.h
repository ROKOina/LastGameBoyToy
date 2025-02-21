#pragma once

#include "..//EventMoveBase.h"

class EventSpriteScaleBehavior : public EventMoveParameterBehaviorBase
{
public:
    EDITABLE_ON_GUI(EventSpriteScaleBehavior)

        EventSpriteScaleBehavior() {};
    ~EventSpriteScaleBehavior() {};
private:
    // 初期化処理
    virtual void EventMoveInitialize() override;

public:
    virtual void Update() override;

    virtual void SetStartValue() override;
    virtual void SetEndValue() override;
private:
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
                end
            );
        }
    }
};

CEREAL_REGISTER_TYPE(EventSpriteScaleBehavior)
CEREAL_CLASS_VERSION(EventSpriteScaleBehavior, 0)

