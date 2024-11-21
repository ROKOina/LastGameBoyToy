#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class MissileCom : public Component
{
public:
    MissileCom(){};
    ~MissileCom() override {};

    // 名前取得
    const char* GetName() const override { return "Missile"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:
    bool playFlag = false;
};