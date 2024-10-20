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

private:

    //���G���ԍX�V
    void UpdateInvincibleTime(float elapsedTime);

public:

    //HP�擾
    void SetHitPoint(float value) { hitPoint = value; }
    float GetHitPoint() { return hitPoint; }

    //HP�̌����Ƒ���
    void AddDamagePoint(float value) { hitPoint += value; }
    void AddHealPoint(float value) { hitPoint += value; }
    void AddDamageAndInvincibleTime(int damage);

private:
    int hitPoint = 100;
    float invincibletimer = 1.0f;
    bool hit = false;
};