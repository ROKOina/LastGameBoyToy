#include "Physxlib.h"

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
}

void PhysXLib::Update(float elapsedTime)
{
    // �V�~�����[�V�������x���w�肷��
    gScene->simulate(elapsedTime);
    // PhysX�̏������I���܂ő҂�
    gScene->fetchResults(true);
}

void PhysXLib::GenerateCollider(Model* model)
{
    for (auto& mesh : model->GetResource()->GetMeshes())
    {
        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = mesh.vertices.size();
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = vertices;  // FBX���璊�o�������_�f�[�^

        for()
    }

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = numVertices;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices;  // FBX���璊�o�������_�f�[�^

    meshDesc.triangles.count = numTriangles;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = indices;  // FBX���璊�o�����C���f�b�N�X�f�[�^

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    if (!cooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
        // ���b�V���̐��������s
    }

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);
}
