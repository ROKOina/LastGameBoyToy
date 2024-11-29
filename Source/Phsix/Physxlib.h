#pragma once
#include "PxPhysicsAPI.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Collsion\NodeCollsionCom.h"
#include <memory>
#include <map>
#include <vector>

using namespace physx;

class PhysXLib
{
private:
    PhysXLib() {};
    ~PhysXLib() { Finalize(); };

private:
    void Finalize();
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

    //更新処理
    void Update(float elapsedTime);

    bool SphereCast_PhysX(
        const DirectX::XMFLOAT3& pos, 
        const DirectX::XMFLOAT3& dir, 
        float radius, float dist, 
        PxSweepBuffer& hit);

    bool RayCast_PhysX(
        const DirectX::XMFLOAT3& origin,
        const  DirectX::XMFLOAT3& unitDir,
        const float maxDistance,
        PxRaycastBuffer& hitBuffer);

    //オブジェクトの塊を分解してColliderを作る（スタティック専用・主にステージで使う）
    void GenerateManyCollider(ModelResource* model, float worldScale);
    void GenerateManyCollider_Convex(ModelResource* model, float worldScale);


    //Modelの形の当たり判定作成
    physx::PxRigidActor* GenerateMeshCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale);
    physx::PxRigidActor* GenerateConvexCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale);
    //矩形、球、カプセルの当たり判定作成
    physx::PxRigidActor* GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale);

    //MeshDescを保存しているmap取得
    std::map<std::string, physx::PxTriangleMeshDesc>& GetMeshStlege() { return meshStlege; }
    //作成されたMeshDesc取得
    physx::PxTriangleMeshDesc& GetStlegeInMeshDesc(std::string filename) { return meshStlege[filename]; }
    //Scene取得
    PxScene* GetScene() { return gScene; }
    //Physcs取得
    PxPhysics* GetPhysics() { return gPhysics; }

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

    //デバッガーアプリと通信用
    static constexpr auto HostID = "127.0.0.1";
    static constexpr auto PostID = 5425;
    static constexpr auto TimeoutMilliSecounds = 10;

    physx::PxPvd* m_device = nullptr;
    physx::PxPvdTransport* m_transport = nullptr;
    physx::PxPvdSceneClient* m_cliant = nullptr;
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
        f1.x += f2.x;
        f1.y += f2.y;
        f1.z += f2.z;
    }
    static inline void operator+=(DirectX::XMFLOAT3& f1, const PxVec3& f2)
    {
        f1.x += f2.x;
        f1.y += f2.y;
        f1.z += f2.z;
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