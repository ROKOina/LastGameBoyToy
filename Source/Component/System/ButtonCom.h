#pragma once

#include "Component/System/Component.h"
#include <PVPGameSystem\PVPGameSystem.h>

class GameObject;

//ゲームモードのボタン
class ButtonCom : public Component
{
public:
    ButtonCom();
    ~ButtonCom() override {}

    // 名前取得
    const char* GetName() const override { return "Button"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    int GetPushCount() { return pushCount; }

private:

    //ボタンに触れたら
    void ButtonTouch(float elapsedTime);

private:

    float nonPushTimer = 0; //押した後に押せなくする
    int pushCount = 0;
};