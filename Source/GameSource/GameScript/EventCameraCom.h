#pragma once

#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

//イベントカメラ
class EventCameraCom : public CameraCom
{
    // コンポーネントオーバーライド
public:
    EventCameraCom();
    ~EventCameraCom() {}

    // 名前取得
    const char* GetName() const override { return "EventCamera"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

private:
    std::string focusObjName;
    std::weak_ptr<GameObject> cameraCopy;
};