#pragma once

#include "../../System\Component.h"

class SetNodeWorldPosCom : public Component
{
public:
    SetNodeWorldPosCom() {};
    ~SetNodeWorldPosCom() override {};

    // 名前取得
    const char* GetName() const override { return "SetNodeWorldPos"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

private:
    std::weak_ptr<GameObject> obj;
};
