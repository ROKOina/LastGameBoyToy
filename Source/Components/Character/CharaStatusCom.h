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

private:

    //無敵時間更新
    void UpdateInvincibleTime(float elapsedTime);

public:

    //HP取得
    void SetHitPoint(float value) { hitPoint = value; }
    float GetHitPoint() { return hitPoint; }

    //HPの減少と増加
    void AddDamagePoint(float value) { hitPoint += value; }
    void AddHealPoint(float value) { hitPoint += value; }
    void AddDamageAndInvincibleTime(int damage);

private:
    int hitPoint = 100;
    float invincibletimer = 1.0f;
    bool hit = false;
};