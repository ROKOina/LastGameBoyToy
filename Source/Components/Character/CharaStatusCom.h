#pragma once

#include "../System\Component.h"

class CharaStatusCom : public Component
{
public:
    CharaStatusCom(){};
    ~CharaStatusCom() override {};

    // 名前取得
    const char* GetName() const override { return "CharaStatus"; }

    // 開始処理
    void Start() override {};

    // GUI描画
    void OnGUI() override
    {
        float hp = hitPoint;
        ImGui::DragFloat("HP", &hp);
    }

    //HP
    void SetHitPoint(float value) { hitPoint = value; }
    float GetHitPoint() { return hitPoint; }

    void AddDamagePoint(float value) { hitPoint += value; }
    void AddHealPoint(float value) { hitPoint += value; }

private:
    int hitPoint=100;

};

