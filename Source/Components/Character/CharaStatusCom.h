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

    // HP�̌����Ƒ���
    void AddDamagePoint(float value);
    void AddHealPoint(float value) { hitPoint += value; }

    // ���G���Ԃ�ݒ�
    void SetInvincibleTime(float time) { invincibleTime = time; }
    bool IsInvincible() const { return currentInvincibleTime > 0.0f; }

private:
    int hitPoint = 100;
    float invincibleTime = 3.0f;        // ���G���Ԃ̒����i�b�j
    float currentInvincibleTime = 0.0f; // �c��̖��G����
};
