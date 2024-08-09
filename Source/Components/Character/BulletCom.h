#pragma once

#include "../System\Component.h"
#include "../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"

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

private:
    float timer = 0.0f;
    float aliveTime = 1.0f;
    int ownerID;
};