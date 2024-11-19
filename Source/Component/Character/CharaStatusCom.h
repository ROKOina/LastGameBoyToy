#pragma once

#include "../System\Component.h"

class CharaStatusCom : public Component
{
public:
    CharaStatusCom() {};
    ~CharaStatusCom() override {};

    // 名前取得
    const char* GetName() const override { return "CharaStatus"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

public:

    //HP取得
    void   SetHitPoint(float value) { hitPoint = value; }
    float* GetHitPoint() { return &hitPoint; }
    void   SetMaxHitPoint(float value) { maxHitPoint = value; }
    float  GetMaxHitpoint() { return maxHitPoint; }

    // HPの減少と増加
    void AddDamagePoint(float value);
    void AddHealPoint(float value) { hitPoint += value; }
    float GetFrameDamage() { return frameDamage; }

    // 無敵時間を設定
    void SetInvincibleTime(float time) { invincibleTime = time; }
    bool IsInvincible() const { return currentInvincibleTime > 0.0f; }

    bool IsDeath() { return isDeath; }

private:
    float hitPoint = 100;
    float maxHitPoint = 100;              //HPの最大値
    float invincibleTime = 3.0f;        // 無敵時間の長さ（秒）
    float currentInvincibleTime = 0.0f; // 残りの無敵時間
    float frameDamage = 0.0f;           // このフレーム中に受けたダメージ

    bool isDeath = false;   //死亡フラグ
};
