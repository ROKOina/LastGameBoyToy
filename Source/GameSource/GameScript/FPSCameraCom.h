#pragma once

#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

//FPSカメラ
class FPSCameraCom : public CameraCom
{
    // コンポーネントオーバーライド
public:
    FPSCameraCom();
    ~FPSCameraCom() {}

    // 名前取得
    const char* GetName() const override { return "FPSCamera"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;
};