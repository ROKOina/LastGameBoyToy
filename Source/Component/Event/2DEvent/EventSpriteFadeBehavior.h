#pragma once
#include "..//EventMoveBase.h"

enum class FadeType
{
    Out,
    In,
};

class EventSpriteFadeBehavior : public EventMoveParameterBehaviorBase
{
public:
    EDITABLE_ON_GUI(EventSpriteFadeBehavior)

     EventSpriteFadeBehavior() {};
    ~EventSpriteFadeBehavior() {};
private:
    // 初期化処理
    virtual void EventMoveInitialize() override;

    void SetAlpha(float alpha);
public:
    virtual void Update() override;

    virtual void SetStartValue() override;
    virtual void SetEndValue() override;
private:
    FadeType fadeType = FadeType::Out;
    float maxAlpha = 1;

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
                fadeType,
                maxAlpha
            );
        }
    }
};

CEREAL_REGISTER_TYPE(EventSpriteFadeBehavior)
CEREAL_CLASS_VERSION(EventSpriteFadeBehavior, 0)