#pragma once
#include "SystemStruct\Curve.h"

// イベントのクラスの名前を登録する関数と自身をマネージャーに登録させる関数を追加するマクロ
#define EDITABLE_ON_GUI(CLASS)\
    std::string GetClassName_()override {return GET_CLASS_NAME(CLASS);}\
    static inline AddEventRegistry::AutoRegister<CLASS>addEventItem{ std::make_shared<CLASS>(), GET_CLASS_NAME(CLASS)};\
    std::shared_ptr<EventMoveParameterBehaviorBase> RegisterEvent(std::list<std::shared_ptr<EventMoveParameterBehaviorBase>>& eventMoveParameters)override\
    {\
        auto eventMoveParam= eventMoveParameters.emplace_back(std::make_shared<CLASS>());\
        return eventMoveParam;\
}

enum class Shaft
{
    X, Y, Z,
};

// イベントのパラメータを更新させるクラスのベース
class EventMoveParameterBehaviorBase: public std::enable_shared_from_this<EventMoveParameterBehaviorBase>
{
    friend class EventDirect;
    friend class EventMove;
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

    // イベント登録の際に必要な関数
    virtual std::shared_ptr<EventMoveParameterBehaviorBase> RegisterEvent(std::list<std::shared_ptr<EventMoveParameterBehaviorBase>>& m_eventMoveParameters) = 0;
    virtual std::string GetClassName_() = 0;
protected:
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

    virtual void RegisterEvent(std::vector<std::shared_ptr<EventMoveParameterBehaviorBase>> eventMoveParameters) = 0;

    virtual std::string GetClassName_() = 0;
protected:
    std::shared_ptr<EventMoveDefaultValueBase> shared_from_this() {
        return std::static_pointer_cast<EventMoveDefaultValueBase>(EventMoveParameterBehaviorBase::shared_from_this());
    }

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

// イベントのクラスの名前を登録するクラス
class AddEventRegistry {
public:
    //! @brief  コンストラクタは削除する
    AddEventRegistry() = delete;

    static void RegisterName(const char* name)
    {
        GetClassName_().emplace_back(name);
    }

    static std::vector<const char*>& GetClassName_()
    {
        static  std::vector<const char*> name;
        return name;
    }

    //! @brief  関数の登録
    static void RegisterClass_(std::shared_ptr<EventMoveParameterBehaviorBase> c)
    {
        GetEventMoveParameter().emplace_back(c);
    }

    //! @brief  追加可能イベントを入手できる関数の配列を取得できる
    static std::vector<std::shared_ptr<EventMoveParameterBehaviorBase>>& GetEventMoveParameter()
    {
        static std::vector<std::shared_ptr<EventMoveParameterBehaviorBase>> eventMoveParameter;
        return eventMoveParameter;
    }

public:
    template<typename Class>
    struct AutoRegister {
        AutoRegister(std::shared_ptr <Class> c, const char* name) {
            RegisterClass_(c);
            RegisterName(name);
        }
    };
};