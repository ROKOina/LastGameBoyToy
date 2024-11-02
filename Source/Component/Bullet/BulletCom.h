#pragma once

#include "../System\Component.h"
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Math\Mathf.h"
#include "Input\Input.h"
#include "Component\System\HitProcessCom.h"

class BulletCom : public Component
{
public:
    BulletCom(int ownerID) : ownerID(ownerID) {};
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

    void SetOwnerID(int id) { ownerID = id; }

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

private:
    float timer = 0.0f;
    float aliveTime = 1.0f;
    int ownerID;
    int damageValue = 10;
};

class BulletCreate
{
public:
    //�_���[�W�e����
    static void DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int damageValue = -10);

    //�X�^���e����
    static void StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int stanValue = -3);

    //�m�b�N�o�b�N�e����
    static void KnockbackFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1);
};