#pragma once

#include "../System\Component.h"

class CharaStatusCom : public Component
{
public:
    CharaStatusCom() {};
    ~CharaStatusCom() override {};

    // ���O�擾
    const char* GetName() const override { return "CharaStatus"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

public:

    //HP�擾
    void SetHitPoint(float value) { hitPoint = value; }
    float GetHitPoint() { return hitPoint; }

    //HP�̌����Ƒ���
    void AddDamagePoint(float value) { hitPoint += value; }
    void AddHealPoint(float value) { hitPoint += value; }

private:
    int hitPoint = 100;
};
