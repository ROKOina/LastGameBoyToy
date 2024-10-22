#pragma once
#include "PxPhysicsAPI.h"
#include "Components\RendererCom.h"
#include "Components\NodeCollsionCom.h"
#include <memory>
#include <map>
#include <vector>

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

    bool RayCast_PhysX(
        const PxVec3& origin,
        const PxVec3& unitDir,
        const PxReal maxDistance,
        PxRaycastBuffer& hitBuffer);


    //Modelの形の当たり判定作成
    physx::PxRigidActor* GenerateCollider(bool isStatic,ModelResource* model);
    //矩形、球、カプセルの当たり判定作成
    physx::PxRigidActor* GenerateCollider(bool isStatic,NodeCollsionCom::CollsionType type, GameObj obj);


private:
    //meshColliderの保存場所


    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysXの初期化とシーンの作成
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};