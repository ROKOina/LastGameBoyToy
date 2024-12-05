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


public:
    //�R���W�������C���[
    enum class CollisionLayer
    {
        Stage = (1 << 0),
        Battle = (1 << 1),
        Public = (1 << 2),
    };

    enum class ShapeType
    {
        Triangle,
        Convex,
        Box,
        Sphere,
        Cupsule,
        Sylnder,
        Complex,
        None
    };

    //�����I�u�W�F�N�g�������Ɏg�p����\����
    struct RigidData
    {
        DirectX::XMFLOAT3 pos = {};
        DirectX::XMFLOAT4 rotate = {};
        DirectX::XMFLOAT3 scale = {};

        bool isStatic = true;
        ShapeType type = ShapeType::None;
        std::shared_ptr<ModelResource> model = nullptr;
        CollisionLayer layer = CollisionLayer::Public;

        float mass = 0.5f;
        float friction = 0.5f;
        float restitution = 0.5f;
        bool useGravity = true;
    };

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

    //���C���[����o�[�W����
    bool RayCast_PhysX(
        const DirectX::XMFLOAT3& origin,
        const  DirectX::XMFLOAT3& unitDir,
        const float maxDistance,
        PxRaycastBuffer& hitBuffer, PhysXLib::CollisionLayer layer);


    //�I�u�W�F�N�g�̉�𕪉�����Collider�����i�X�^�e�B�b�N��p�E��ɃX�e�[�W�Ŏg���j
    void GenerateComplexCollider(ModelResource* model, float worldScale, CollisionLayer layer);

    //�����蔻��쐬
    physx::PxRigidActor* GenerateCollider(RigidData& data);
    
    //����̌`�󐶐�
    physx::PxShape* MakeShape(RigidData& data);
    physx::PxShape* Make_TriangleShape(RigidData& data);
    physx::PxShape* Make_ConvexShape(RigidData& data);
    physx::PxShape* Make_SphereShape(RigidData& data);
    physx::PxShape* Make_BoxShape(RigidData& data);

    //Scene�擾
    PxScene* GetScene() { return gScene; }
    //Physcs�擾
    PxPhysics* GetPhysics() { return gPhysics; }

private:
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