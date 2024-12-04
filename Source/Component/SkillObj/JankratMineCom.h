#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class JankratMineCom : public Component
{
public:
    JankratMineCom() {};
    ~JankratMineCom() override {};

    // 名前取得
    const char* GetName() const override { return "JankratMine"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

    //セッター・ゲッター
    void SetLifeTime(float time) { lifeTime = time; };

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

    bool GetIsPlant() { return isPlant; }

    bool GetExplosionFlag() { return explosionFlag; }
    void SetExplosionFlag(bool flag) { explosionFlag = flag; }

private:

    float explosionTime = 1.5f;
    float lifeTimer = 0.0f;
    float lifeTime = 2.0f;

    //設置フラグ
    bool isPlant = false;

    bool explosionFlag = false;
    int damageValue = 10;
};
