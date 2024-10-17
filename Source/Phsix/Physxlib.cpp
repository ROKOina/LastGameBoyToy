#include "Physxlib.h"
#include "Components\TransformCom.h"

void PhysXLib::Initialize()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // Physics�I�u�W�F�N�g�̍쐬
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);

    // �V�[���̐ݒ�
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // �d�͐ݒ�
    gDispatcher = PxDefaultCpuDispatcherCreate(2);  // 2�X���b�h�Ńf�B�X�p�b�`���[���쐬
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // �V�[���̍쐬
    gScene = gPhysics->createScene(sceneDesc);


    //�ÓI�I�u�W�F�N�g�̒ǉ�
    physx::PxRigidStatic* rigid_static
        = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    // �`��(Box)���쐬
    physx::PxShape* box_shape
        = gPhysics->createShape(
            // Box�̑傫��
            physx::PxBoxGeometry(5.f, 1.f, 5.f),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
    // �`��̃��[�J�����W��ݒ�
    box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    // �`���R�Â�
    rigid_static->attachShape(*box_shape);
    // ���̂���Ԃɒǉ�
    gScene->addActor(*rigid_static);
}

void PhysXLib::Update(float elapsedTime)
{
    // �V�~�����[�V�������x���w�肷��
    gScene->simulate(elapsedTime);
    // PhysX�̏������I���܂ő҂�
    gScene->fetchResults(true);
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, Model* model)
{
    //for (auto& mesh : model->GetResource()->GetMeshes())
    //{
    //    PxTriangleMeshDesc meshDesc;
    //    meshDesc.points.count = mesh.vertices.size();
    //    meshDesc.points.stride = sizeof(PxVec3);
    //    meshDesc.points.data = vertices;  // FBX���璊�o�������_�f�[�^

    //    for()
    //}

    //PxTriangleMeshDesc meshDesc;
    //meshDesc.points.count = numVertices;
    //meshDesc.points.stride = sizeof(PxVec3);
    //meshDesc.points.data = vertices;  // FBX���璊�o�������_�f�[�^

    //meshDesc.triangles.count = numTriangles;
    //meshDesc.triangles.stride = 3 * sizeof(PxU32);
    //meshDesc.triangles.data = indices;  // FBX���璊�o�����C���f�b�N�X�f�[�^

    //PxDefaultMemoryOutputStream writeBuffer;
    //PxTriangleMeshCookingResult::Enum result;
    //if (!cooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
    //    // ���b�V���̐��������s
    //}

    //PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    //PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

    return nullptr;
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, GameObj obj)
{
    physx::PxRigidActor* rigidObj = nullptr;

    if (isStatic) {
        //�����Ȃ�(�ÓI)���̂��쐬
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    }
    else{
        // ���������Ƃ̂ł���(���I)���̂��쐬
        rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    }

    // �`��(Box)���쐬
    physx::PxShape* shape = nullptr;

    switch (type)
    {
    case NodeCollsionCom::CollsionType::BOX:
        shape = gPhysics->createShape(
            // Box�̑傫��
            physx::PxBoxGeometry(1.f, 1.f, 1.f),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::SPHER:
        shape = gPhysics->createShape(
            // Box�̑傫��
            physx::PxSphereGeometry(1.f),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::CYLINDER:
        shape = gPhysics->createShape(
            // Box�̑傫��
            physx::PxCapsuleGeometry(1.f,1.f),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    default:
        return nullptr;
        break;
    }

    // �`���R�Â�
    shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    rigidObj->attachShape(*shape);

    physx::PxTransform transform;
    transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };
    rigidObj->setGlobalPose(transform);

    // ���̂���Ԃɒǉ�
    gScene->addActor(*rigidObj);

    return rigidObj;
}
