#pragma once

#include "../System\Component.h"

class BarrierCom : public Component
{
public:
    BarrierCom() {};
    ~BarrierCom() override {};

    // 名前取得
    const char* GetName() const override { return "Barrier"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:

    float barriertime = 0.0f;
};