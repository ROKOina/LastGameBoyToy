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
    void   SetHitPoint(float value) { hitPoint = value; }
    float* GetHitPoint() { return &hitPoint; }
    void   SetMaxHitPoint(float value) { maxHitPoint = value; }
    float  GetMaxHitpoint() { return maxHitPoint; }

    // HP�̌����Ƒ���
    void AddDamagePoint(float value);
    void AddHealPoint(float value) { hitPoint += value; }
    float GetFrameDamage() { return frameDamage; }

    // ���G���Ԃ�ݒ�
    void SetInvincibleTime(float time) { invincibleTime = time; }
    bool IsInvincible() const { return currentInvincibleTime > 0.0f; }

    bool IsDeath() { return isDeath; }

private:
    float hitPoint = 100;
    float maxHitPoint = 100;              //HP�̍ő�l
    float invincibleTime = 3.0f;        // ���G���Ԃ̒����i�b�j
    float currentInvincibleTime = 0.0f; // �c��̖��G����
    float frameDamage = 0.0f;           // ���̃t���[�����Ɏ󂯂��_���[�W

    bool isDeath = false;   //���S�t���O
};
