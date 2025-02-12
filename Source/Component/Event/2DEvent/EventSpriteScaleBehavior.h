#pragma once

#include "sprite_move_base.h"

namespace chimera
{
    class ScaleUpDownBehavior : public SpriteMoveBaseBehavior
    {
    public:
#ifdef DEBUG
        CLASS_NAME(ScaleUpDownBehavior)
        EDITABLE_ON_GUI(ScaleUpDownBehavior)
#endif
            using SpriteMoveBaseBehavior::SpriteMoveBaseBehavior;

    protected:
        std::shared_ptr<ScaleUpDownBehavior> shared_from_this() {
            return std::static_pointer_cast<ScaleUpDownBehavior>(SpriteMoveBaseBehavior::shared_from_this());
        }

    protected:
        // 初期化処理
        virtual void MoveInitialize() override;

    public:
        virtual void Update() override;

        virtual void SetStartValue() override;
        virtual void SetEndValue() override;
    private:
        float start = {};
        float end = {};

#ifdef DEBUG
    private:
        friend class ImGuiManager;
        virtual void Editor();
#endif

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(
            Archive& archive, const uint32_t version)
        {
            if (version == 0)
            {
                archive(
                    cereal::base_class<BehaviorComponent>(this),
                    //easeType,
                    //easeInOutType,
                    timer,
                    start,
                    end,
                    wateTime,
                    loop,
                    isReverse,
                    isUseUnscale,
                    isDecision
                );
            }
            if (version == 1)
            {
                archive(
                    cereal::base_class<SpriteMoveBaseBehavior>(this),
                    start,
                    end
                );
            }
        }
    };
}

CEREAL_REGISTER_TYPE(chimera::ScaleUpDownBehavior)
CEREAL_CLASS_VERSION(chimera::ScaleUpDownBehavior, 1)

