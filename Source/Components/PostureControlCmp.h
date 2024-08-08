#pragma once
#include "System\Component.h"

class PostureControl : public Component
{
public:
    PostureControl();
    ~PostureControl();

    // 名前取得
    const char* GetName() const override { return "PostureControl"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;
};
