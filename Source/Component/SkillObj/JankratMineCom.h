#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class JankratMineCom : public Component
{
public:
    JankratMineCom() {};
    ~JankratMineCom() override {};

    // ���O�擾
    const char* GetName() const override { return "JankratMine"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

    //�N��
    void Fire();


    //�Z�b�^�[�E�Q�b�^�[
    void SetLifeTime(float time) { lifeTime = time; };

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

    bool GetIsPlant() { return isPlant; }

    bool GetExplosionFlag() { return explosionFlag; }
    void SetExplosionFlag(bool flag) { explosionFlag = flag; }

private:

    float explosionTime = 1.5f;
    float lifeTimer = 0.0f;
    float lifeTime = 2.0f;

    bool isPlant = false;    //�ݒu�t���O
    bool explosionFlag = false; //�������t���O

    int damageValue = 10;
};
