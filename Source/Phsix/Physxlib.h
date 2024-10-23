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
    // �C���X�^���X�擾
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //������
    void Initialize();

    //�I����
    void Finalize();

    //�X�V����
    void Update(float elapsedTime);

    bool RayCast_PhysX(
        const PxVec3& origin,
        const PxVec3& unitDir,
        const PxReal maxDistance,
        PxRaycastBuffer& hitBuffer);

    //Model�̌`�̓����蔻��쐬
    physx::PxRigidActor* GenerateCollider(bool isStatic,ModelResource* model);
    //��`�A���A�J�v�Z���̓����蔻��쐬
    physx::PxRigidActor* GenerateCollider(bool isStatic,NodeCollsionCom::CollsionType type, GameObj obj);

    //MeshDesc��ۑ����Ă���map�擾
    std::map<std::string, physx::PxTriangleMeshDesc>& GetMeshStlege() { return meshStlege; }
    //�쐬���ꂽMeshDesc�擾
    physx::PxTriangleMeshDesc& GetStlegeInMeshDesc(std::string filename) { return meshStlege[filename]; }
    //Scene�擾
    PxScene* GetScene() { return gScene; }
private:
    //meshCollider�̕ۑ��ꏊ
    std::map<std::string, physx::PxTriangleMeshDesc> meshStlege;

    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysX�̏������ƃV�[���̍쐬
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};

namespace physx
{
#if(1)
  // XMFLOAT3 �Ƃ̑����Z
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

    // XMFLOAT4 �Ƃ̑����Z
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