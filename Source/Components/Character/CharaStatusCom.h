#pragma once

#include "../System\Component.h"

class CharaStatusCom : public Component
{
public:
    CharaStatusCom(){};
    ~CharaStatusCom() override {};

    // ���O�擾
    const char* GetName() const override { return "CharaStatus"; }

    // �J�n����
    void Start() override {};

    // GUI�`��
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

