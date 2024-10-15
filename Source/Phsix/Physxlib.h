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

    //Model�̌`�̓����蔻��쐬
    void GenerateCollider(Model* model);

private:
    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;

    // PhysX�̏������ƃV�[���̍쐬
    PxPhysics* gPhysics = nullptr;
    PxFoundation* gFoundation = nullptr;
    PxDefaultCpuDispatcher* gDispatcher = nullptr;
    PxScene* gScene = nullptr;
};