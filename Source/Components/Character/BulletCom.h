#pragma once

#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"

class BulletCom : public Component
{
public:
    BulletCom() {};
    ~BulletCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Bullet"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

    //�e��������
    void EraseBullet(float elapsedTIme);

    //�Z�b�^�[�E�Q�b�^�[
    void SetAliveTime(float time) { aliveTime = time; };

private:
    float timer = 0.0f;
    float aliveTime = 1.0f;
};