#pragma once

#include "../System\Component.h"
#include "Math\Mathf.h"
#include "Component\System\HitProcessCom.h"

class JankratUltCom : public Component
{
public:
    JankratUltCom() {};
    ~JankratUltCom() override {};

    // 名前取得
    const char* GetName() const override { return "JankratUlt"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:

    //起爆
    void Fire(float elapsedTime);

private:

    float time = 0.0f;
};