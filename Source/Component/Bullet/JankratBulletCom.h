#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class JankratBulletCom : public Component
{
public:
    JankratBulletCom() {};
    ~JankratBulletCom() override {};

    // 名前取得
    const char* GetName() const override { return "JankratBullet"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

    //ult更新
    void UltUpdate(float elapsedTime);

    //セッター・ゲッター
    void SetLifeTime(float time) { lifeTime = time; };
    void SetAddGravity(float value) { addGravity = value; }

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

    void SetExplosionTime(float explosionTime_) { explosionTime = explosionTime_; }
    float GetExplosionTime() { return explosionTime; }

private:

    //爆発させる
    void TriggerExplosion();

    //直撃ダメージを与える
    void ApplyDirectHitDamage();

private:
    float lifeTimer = 0.0f;

    float explosionTime = 1.5f;
    float lifeTime = 5.0f;

    bool explosionFlag = false;
    int damageValue = 10;

    float addGravity = 0.0f;
};
