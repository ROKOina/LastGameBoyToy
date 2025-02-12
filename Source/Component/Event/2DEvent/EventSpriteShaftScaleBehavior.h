#pragma once

#include "sprite_move_base.h"

namespace chimera
{
    class ShaftScaleBehavior : public SpriteMoveBaseBehavior
    {
    public:
#ifdef DEBUG
        CLASS_NAME(ShaftScaleBehavior)
            EDITABLE_ON_GUI(ShaftScaleBehavior)
#endif
            using SpriteMoveBaseBehavior::SpriteMoveBaseBehavior;

    protected:
        std::shared_ptr<ShaftScaleBehavior> shared_from_this() {
            return std::static_pointer_cast<ShaftScaleBehavior>(SpriteMoveBaseBehavior::shared_from_this());
        }

    protected:
        // 初期化処理
        virtual void MoveInitialize() override;

    public:
        virtual void Update() override;

        virtual void SetStartValue() override;
        virtual void SetEndValue() override;

    private:
        void SetScale(float scale);
    private:
        enum class ScaleShaft
        {
            X, Y, Z,
        };
        ScaleShaft scaleShaft = ScaleShaft::X;

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
                    cereal::base_class<SpriteMoveBaseBehavior>(this),
                    start,
                    end,
                    scaleShaft
                );
            }
        }
    };
}

CEREAL_REGISTER_TYPE(chimera::ShaftScaleBehavior)
CEREAL_CLASS_VERSION(chimera::ShaftScaleBehavior, 0)

