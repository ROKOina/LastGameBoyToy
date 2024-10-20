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
    // �C���X�^���X�擾
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //������
    void Initialize();

    //�I����
    void Finalize() {};

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


private:
    //meshCollider�̕ۑ��ꏊ


    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysX�̏������ƃV�[���̍쐬
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};