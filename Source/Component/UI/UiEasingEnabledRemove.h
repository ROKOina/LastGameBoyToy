#pragma once
#include "UiSystem.h"

class UiEasingEnabledRemoveCom : public Component
{
    //コンポーネントオーバーライド
public:
    UiEasingEnabledRemoveCom() {}
    ~UiEasingEnabledRemoveCom() {}

    // 名前取得
    const char* GetName() const override { return "UiEasingEnabledRemove"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override {
        timerUI -= elapsedTime;
        if (timerUI < 0)
            GetGameObject()->SetEnabled(false);
    }

    void OnGUI()override {}

    void SetRemoveTimer(float time) {
        timerUI = time;
        GetGameObject()->SetEnabled(true);
    }

private:
    float timerUI = 0;
};
