#pragma once

#include <DirectXMath.h>

#include "CameraCom.h"

//フリーカメラ
class FreeCameraCom : public CameraCom
{
    // コンポーネントオーバーライド
public:
    FreeCameraCom();
    ~FreeCameraCom() {}

    // 名前取得
    const char* GetName() const override { return "FreeCamera"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    void SetFocusPos(DirectX::XMFLOAT3 pos) { focusPos = pos; }

    void SetDistance(float dis) { distance = dis; }

    //FreeCameraクラス
public:

private:
    DirectX::XMFLOAT3 focusPos = { 0,0,0 };
    float distance = 10;

    DirectX::XMFLOAT2	oldCursor = {};
    DirectX::XMFLOAT2	newCursor = {};
};