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

    //�e�e�ƓG�̍U���Ƃ̏Փˏ���
    void BulletVSEnemyMissile();

    //�Z�b�^�[�E�Q�b�^�[
    void SetAliveTime(float time) { aliveTime = time; };

    void SetOwnerID(int id) { ownerID = id; }

    void SetDamageValue(int value) { damageValue = value; }
    int GetDamageValue() { return damageValue; }

    void SetViewBullet(std::shared_ptr<GameObject> obj) { viewBullet = obj; }
    std::weak_ptr<GameObject> GetViewBullet() { return viewBullet; }

private:

    //�q�b�g�G�t�F�N�g�����폜
    void HitEffect();

private:
    float timer = 0.0f;
    float aliveTime = 1.0f;
    int ownerID;
    int damageValue = 10;

    std::weak_ptr<GameObject> viewBullet;
};

class BulletCreate
{
public:
    //�_���[�W�e����
    static void DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int damageValue = 10);

    //�X�^���e����
    static void StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int stanValue = 3);

    //�m�b�N�o�b�N�e����
    static void KnockbackFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1);

    //�t�@���ʏ�e
    static GameObj FarahDamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int damageValue = 10);
    static GameObj FarahKnockBack(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1);

    //�W�����N���̒e
    static GameObj JankratBulletFire(std::shared_ptr<GameObject> parent, DirectX::XMFLOAT3 pos, int id);
    static GameObj JankratMineFire(std::shared_ptr<GameObject> parent, DirectX::XMFLOAT3 pos, float force, float damage, int id);
};
