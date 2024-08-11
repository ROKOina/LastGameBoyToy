#pragma once

#include "Components/System/Component.h"

class HitProcessCom : public Component
{
public:
    HitProcessCom(std::shared_ptr<GameObject> obj)
        : hitIntervalTimer(0), myObj(obj)
    {}
    ~HitProcessCom() override {}

    // 名前取得
    const char* GetName() const override { return "HitProcess"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    enum class HIT_TYPE
    {
        DAMAGE,
        HEAL,
        STAN,
    };
    void SetHitType(HitProcessCom::HIT_TYPE type) { hitType = type; }

    void SetHitInterval(float sec) { hitIntervalTime = sec; }

    void SetValue(float value) { this->value = value; }

    bool IsHit() { return isHit; }

private:
    void HitProcess(int myID, int hitID);

private:
    //弾の持ち主
    std::weak_ptr<GameObject> myObj;

    //種類
    HIT_TYPE hitType = HIT_TYPE::DAMAGE;

    //ヒット間隔
    float hitIntervalTime = 0.1f;
    float hitIntervalTimer;

    //与える数
    int value = 1;

    //当たっているか
    bool isHit = false;
};