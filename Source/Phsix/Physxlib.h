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
    ~PhysXLib() { PhysXLib::Instance().Finalize(); };

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
    void Finalize();

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

    //MeshDescを保存しているmap取得
    std::map<std::string, physx::PxTriangleMeshDesc>& GetMeshStlege() { return meshStlege; }
    //作成されたMeshDesc取得
    physx::PxTriangleMeshDesc& GetStlegeInMeshDesc(std::string filename) { return meshStlege[filename]; }
    //Scene取得
    PxScene* GetScene() { return gScene; }
private:
    //meshColliderの保存場所
    std::map<std::string, physx::PxTriangleMeshDesc> meshStlege;

    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysXの初期化とシーンの作成
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};

namespace physx
{
#if(1)
  // XMFLOAT3 との足し算
    static inline PxVec3 operator+(const PxVec3& f1, const DirectX::XMFLOAT3& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y, f1.z + f2.z };
    }
    static inline DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& f1, const PxVec3& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y, f1.z + f2.z };
    }
    static inline void operator+=(PxVec3& f1, const DirectX::XMFLOAT3& f2)
    {
        f1 = f1 + f2;
    }
    static inline void operator+=(DirectX::XMFLOAT3& f1, const PxVec3& f2)
    {
        f1 = f1 + f2;
    }

    // XMFLOAT4 との足し算
    static inline PxVec4 operator+(const PxVec4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y, f1.z + f2.z, f1.w + f2.w };
    }
    static inline DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& f1, const PxVec4& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y, f1.z + f2.z, f1.w + f2.w };
    }
    //static inline void operator+=(PxVec4& f1, const DirectX::XMFLOAT4& f2)
    //{
    //    f1 = f1 + f2;
    //}
    //static inline void operator+=(DirectX::XMFLOAT4 f1, const PxVec4& f2)
    //{
    //    f1 = f1 + f2;
    //}

#endif

}