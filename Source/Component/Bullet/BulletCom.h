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

    // 名前取得
    const char* GetName() const override { return "Bullet"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

    //弾消去処理
    void EraseBullet(float elapsedTIme);

    //セッター・ゲッター
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
    //ダメージ弾生成
    static void DamageFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int damageValue = -10);

    //スタン弾生成
    static void StanFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1, int stanValue = -3);

    //ノックバック弾生成
    static void KnockbackFire(std::shared_ptr<GameObject> objPoint, float bulletSpeed = 40, float power = 1);
};
