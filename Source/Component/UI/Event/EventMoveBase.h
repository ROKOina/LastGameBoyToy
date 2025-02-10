#pragma once
//#include "SystemStruct\Curve.h"
//#include "Component/System/Component.h"

//namespace chimera
//{
//    class EventMoveBase : public Component
//    {
//    public:
//        //名前設定
//        const char* GetName() const override { return "EventMoveBase"; }
//
//        // タイマーの更新処理
//        void TimerUpdate();
//
//        // 終了判定監視処理
//        void FinishJudge();
//    public:
//        friend class EventDirectBehavior;
//        friend class EventDirectEditorWindow;
//        void SetTimer(float timer) { this->timer = timer; }
//
//        void SetIsDecision(bool isDecision) { this->isDecision = isDecision; }
//
//        void SetReverse(bool isReverse) { this->isReverse = isReverse; }
//
//    public:
//        void SetDuration(float duration) { this->duration = duration; }
//        void SetStart(float value) { this->startValue = value; }
//        void SetEnd(float value) { this->endValue = value; }
//
//    public:
//        virtual void Start() override;
//        virtual void Update(float elapsedTime) override {};
//
//        virtual void SetStartValue() = 0;
//        virtual void SetEndValue() = 0;
//
//        bool GetFinishEnable() { return isDecision; }
//
//        // GUI描画
//        virtual void OnGUI();
//    protected:
//        float timer = 0.0f;
//
//        Curve curve;
//        float duration = 10.0f;
//        float startValue = {};
//        float endValue = {};
//        bool  isUseUnscaled = false;
//        bool  isAutoInitialize = true;
//        bool  loop = false;
//        bool  isReverse = false;
//        bool  isDecision = false;
//        bool  startInitializeEnable = true;
//
//        friend struct EventDirect;
//        friend class EventDirectEditorWindow;
//        bool eventUpdateEnable = false;
//
//    private:
//        friend class cereal::access;
//        template <class Archive>
//        void serialize(
//            Archive& archive, const uint32_t version)
//        {
//            if (version == 2)
//            {
//                archive(
//                    cereal::base_class<BehaviorComponent>(this),
//                    curve,
//                    duration,
//                    startValue,
//                    endValue,
//                    isUseUnscaled,
//                    isAutoInitialize,
//                    loop,
//                    isReverse,
//                    isDecision,
//                    eventUpdateEnable,
//                    startInitializeEnable
//                );
//            }
//        }
//    };
//}
//
//CEREAL_REGISTER_TYPE(chimera::InflexionBaseBehavior)
//CEREAL_CLASS_VERSION(chimera::InflexionBaseBehavior, 2)

