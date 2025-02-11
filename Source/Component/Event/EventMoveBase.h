#pragma once
#include "SystemStruct\Curve.h"

enum class Shaft
{
    X, Y, Z,
};

class EventMoveParameterBehaviorBase
{
public:
    EventMoveParameterBehaviorBase() {};
    ~EventMoveParameterBehaviorBase() {};

protected:
    // 初期化処理
    virtual void EventMoveInitialize() {};

    // タイマーの更新処理
    void TimerUpdate();

    // 終了判定監視処理
    void FinishJudge();
public:
    void SetTimer(float timer) { m_timer = timer; }
    void SetIsDecision(bool isDecision) { m_decisionEnable = isDecision; }
    void SetReverse(bool isReverse) { m_reverseEnable = isReverse; }
    void SetDuration(float duration) { m_duration = duration; }
    void SetOwner(std::shared_ptr<Component> owner) { m_owner = owner; }

public:
    virtual void Start();
    virtual void Update(){};

    virtual void SetStartValue() = 0;
    virtual void SetEndValue() = 0;

    bool GetFinishEnable() { return m_decisionEnable; }

protected:
    friend class EventMoveBase;
    // GUI描画
    virtual void OnGUI();

protected:
    std::weak_ptr<Component> m_owner;
    float m_timer = 0.0f;

    EventCurve m_curve;
    float m_duration                = 5.0f;
    bool  m_useUnscaledEnable       = false;
    bool  m_autoInitializeEnable    = true;
    bool  m_loopEnable              = false;
    bool  m_reverseEnable           = false;
    bool  m_decisionEnable          = false;
    bool  m_startInitializeEnable   = true;
    bool  m_eventUpdateEnable       = false;
private:
    friend class cereal::access;
    template <class Archive>
    void serialize(
        Archive& archive, const uint32_t version)
    {
        if (version == 0)
        {
            archive(
                m_curve,
                m_duration,
                m_useUnscaledEnable,
                m_autoInitializeEnable,
                m_loopEnable,
                m_reverseEnable,
                m_decisionEnable,
                m_eventUpdateEnable,
                m_startInitializeEnable
            );
        }
    }
};
CEREAL_REGISTER_TYPE(EventMoveParameterBehaviorBase)
CEREAL_CLASS_VERSION(EventMoveParameterBehaviorBase, 0)

class EventMoveDefaultValueBase : EventMoveParameterBehaviorBase
{
public:
    EventMoveDefaultValueBase() {};
    ~EventMoveDefaultValueBase() {};

protected:
    virtual void SetStartValue() = 0;
    virtual void SetEndValue() = 0;
protected:

    // GUI描画
    virtual void OnGUI();

protected:
    float m_startValue = 0;
    float m_endValue = 0;

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
                m_endValue
            );
        }
    }
};
CEREAL_REGISTER_TYPE(EventMoveDefaultValueBase)
CEREAL_CLASS_VERSION(EventMoveDefaultValueBase, 0)

// イベントのパラメータにアクセスするためのコンポーネント
class EventMoveBase : public Component, public std::enable_shared_from_this<EventMoveBase>
{
public:
    // TODO:シリアライズの処理を追加
    EventMoveBase();
    ~EventMoveBase() {};

    //名前設定
    const char* GetName() const override { return "EventMoveBase"; }

public:
    virtual void Start() override;
    virtual void Update(float elapsedTime);

protected:
    // GUI描画
    virtual void OnGUI();

protected:
    std::shared_ptr<EventMoveParameterBehaviorBase> m_eventMoveParameter;
};