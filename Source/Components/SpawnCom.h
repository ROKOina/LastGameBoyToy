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

private:

    //生成間隔
    float spawnInterval = 5.0f;  // デフォルトでは5秒ごとに生成
    float spawnRadius = 5.0f;    // ランダム生成範囲の半径

    // 生成個数
    int spawnCount = 5;         // デフォルトの生成個数は10

    // 最後に生成した時間
    float lastSpawnTime = 0.0f;

    // 現在生成されたオブジェクトのカウント
    int currentSpawnedCount = 0; // 生成されたオブジェクトの数を追跡
};