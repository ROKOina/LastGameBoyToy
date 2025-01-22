#pragma once

#include "Component/System/Component.h"
#include "Component/System/TransformCom.h"

class PingCom : public Component
{
public:
    PingCom() {}
    ~PingCom() override {}

    // 名前取得
    const char* GetName() const override { return "Ping"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override
    {
        time -= elapsedTime;
        if (time < 0)
            GetGameObject()->SetEnabled(false);
    }

    // GUI描画
    void OnGUI() override {}

    void SetPing(DirectX::XMFLOAT3 pos)
    {
        GetGameObject()->SetEnabled(true);
        GetGameObject()->transform_->SetWorldPosition(pos);
        time = 3;
    }

private:
    float time = 0;
};