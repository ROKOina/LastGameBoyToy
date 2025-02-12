#pragma once

#include "sprite_move_base.h"

namespace chimera
{
    enum class FadeType
    {
        Out,
        In,
    };

    class FadeBehavior : public SpriteMoveBaseBehavior
    {
    public:
#ifdef DEBUG
        CLASS_NAME(FadeBehavior)
            EDITABLE_ON_GUI(FadeBehavior)
#endif
            using SpriteMoveBaseBehavior::SpriteMoveBaseBehavior;

    protected:
        std::shared_ptr<FadeBehavior> shared_from_this() {
            return std::static_pointer_cast<FadeBehavior>(SpriteMoveBaseBehavior::shared_from_this());
        }

    protected:
        // 初期化処理
        virtual void MoveInitialize() override;

    public:
        virtual void Update() override;

        virtual void SetStartValue() override;
        virtual void SetEndValue() override;
    private:
        FadeType fadeType = FadeType::Out;
        float maxAlpha = 1;

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
                    fadeType,
                    maxAlpha
                );
            }
        }
    };
}

CEREAL_REGISTER_TYPE(chimera::FadeBehavior)
CEREAL_CLASS_VERSION(chimera::FadeBehavior, 0)

