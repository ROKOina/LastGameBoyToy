#pragma once

#include "Components/System/Component.h"

class SpawnCom :public Component
{
public:

    SpawnCom();
    ~SpawnCom() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Spawn"; }

    //複製機能
    void SpawnGameObject();

public:

    //更新するか否か
    const bool OnTrigger() { return spwntrigger; }
    void SetOnTrigger(bool flag) { spwntrigger = flag; }

private:

    //生成間隔
    float spawnInterval = 0.6f;
    float spawnRadius = 5.0f;

    // 生成個数
    int spawnCount = 2;

    // 最後に生成した時間
    float lastSpawnTime = 0.0f;

    // 現在生成されたオブジェクトのカウント
    int currentSpawnedCount = 0; // 生成されたオブジェクトの数を追跡

    //生成フラグ
    bool spwntrigger = false;
};