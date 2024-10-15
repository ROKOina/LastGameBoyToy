#pragma once
#include "PxPhysicsAPI.h"
#include "Components\RendererCom.h"
#include <memory>

using namespace physx;

class PhysXLib
{
private:
    PhysXLib() {};
    ~PhysXLib() {};

public:
    // インスタンス取得
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //初期化
    void Initialize();

    //終了化
    void Finalize() {};

    //更新処理
    void Update(float elapsedTime);

    //Modelの形の当たり判定作成
    void GenerateCollider(Model* model);

private:
    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysXの初期化とシーンの作成
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};