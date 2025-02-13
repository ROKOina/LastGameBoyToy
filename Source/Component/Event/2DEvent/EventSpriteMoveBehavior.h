#pragma once
#include "..//EventMoveBase.h"

class EventSpriteMoveBehavior : public EventMoveParameterBehaviorBase
{
public:
    EDITABLE_ON_GUI(EventSpriteMoveBehavior)

    EventSpriteMoveBehavior() {};
    ~EventSpriteMoveBehavior() {};

private:
    // 初期化処理
    virtual void EventMoveInitialize() override;

    void SetPosition(Vector2 setValue);

public:
    virtual void Update() override;

    virtual void SetStartValue() override;
    virtual void SetEndValue() override;
private:
    Vector2 m_startValue = {};
    Vector2 m_endValue = {};
    bool m_localEnable = true;

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
                m_startValue,
                m_endValue,
                m_localEnable
            );
        }
    }
};

CEREAL_REGISTER_TYPE(EventSpriteMoveBehavior)
CEREAL_CLASS_VERSION(EventSpriteMoveBehavior, 0)

