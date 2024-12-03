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
    // �C���X�^���X�擾
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //������
    void Initialize();

    //�I����

    //�X�V����
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

    //�I�u�W�F�N�g�̉�𕪉�����Collider�����i�X�^�e�B�b�N��p�E��ɃX�e�[�W�Ŏg���j
    void GenerateManyCollider(ModelResource* model, float worldScale);
    void GenerateManyCollider_Convex(ModelResource* model, float worldScale);


    //Model�̌`�̓����蔻��쐬
    physx::PxRigidActor* GenerateMeshCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale);
    physx::PxRigidActor* GenerateConvexCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale);
    //��`�A���A�J�v�Z���̓����蔻��쐬
    physx::PxRigidActor* GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale);

    //MeshDesc��ۑ����Ă���map�擾
    std::map<std::string, physx::PxTriangleMeshDesc>& GetMeshStlege() { return meshStlege; }
    //�쐬���ꂽMeshDesc�擾
    physx::PxTriangleMeshDesc& GetStlegeInMeshDesc(std::string filename) { return meshStlege[filename]; }
    //Scene�擾
    PxScene* GetScene() { return gScene; }
    //Physcs�擾
    PxPhysics* GetPhysics() { return gPhysics; }

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

    //�f�o�b�K�[�A�v���ƒʐM�p
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