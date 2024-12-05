#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class JankratBulletCom : public Component
{
public:
    JankratBulletCom(){};
    ~JankratBulletCom() override {};

    // ���O�擾
    const char* GetName() const override { return "JankratBullet"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

    //�Z�b�^�[�E�Q�b�^�[
    void SetLifeTime(float time) { lifeTime = time; };
    void SetAddGravity(float value) { addGravity = value; }

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

private:
    float lifeTimer = 0.0f;

    float explosionTime = 1.5f;
    float lifeTime = 5.0f;

    bool explosionFlag = false;
    int damageValue = 10;

    float addGravity = 0.0f;
};
