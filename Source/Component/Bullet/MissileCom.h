#pragma once

#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Component\System\HitProcessCom.h"
#include <Component/Enemy/BossCom.h>

class MissileCom : public Component
{
public:
    MissileCom() {};
    MissileCom(float soundTime) { soundLen = soundTime; };
    ~MissileCom() override {
        bossSE->GetAudios(BOSS_BULLET)->Audio3DStop();
    };

    // 名前取得
    const char* GetName() const override { return "Missile"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:
    float soundLen = 40.0f;
    bool playFlag = false;

    std::shared_ptr<BossCom> bossSE;
};