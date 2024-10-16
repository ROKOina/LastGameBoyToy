#pragma once

#include "Components/System/Component.h"

//�l�b�g�ɑ���p
class HitProcessCom : public Component
{
public:
    HitProcessCom(std::shared_ptr<GameObject> obj)
        : hitIntervalTimer(0), myObj(obj)
    {}
    ~HitProcessCom() override {}

    // ���O�擾
    const char* GetName() const override { return "HitProcess"; }

    // �J�n����
    void Start() override {}

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    enum class HIT_TYPE
    {
        DAMAGE,
        HEAL,
        STAN,
        KNOCKBACK,
    };
    void SetHitType(HitProcessCom::HIT_TYPE type) { hitType = type; }

    void SetHitInterval(float sec) { hitIntervalTime = sec; }

    void SetValue(float value) { this->value = value; }
    void SetValue3(DirectX::XMFLOAT3 value3) { this->value3 = value3; }

    bool IsHit() { return isHit; }

private:
    void HitProcess(int myID, int hitID);

private:
    //�e�̎�����
    std::weak_ptr<GameObject> myObj;

    //���
    HIT_TYPE hitType = HIT_TYPE::DAMAGE;

    //�q�b�g�Ԋu(���C�L���X�g�p)
    float hitIntervalTime = 0.1f;
    float hitIntervalTimer;

    //�^���鐔
    float value = 1.0f; //DAMAGE, HEAL, STAN
    DirectX::XMFLOAT3 value3 = { 0,0,0 };   //KNOCKBACK

    //�������Ă��邩
    bool isHit = false;

    //�����Ă���(damage)
    int hitDamage = 0;
};