#include "Physxlib.h"

void PhysXLib::Initialize()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // Physicsオブジェクトの作成
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);

    // シーンの設定
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 重力設定
    gDispatcher = PxDefaultCpuDispatcherCreate(2);  // 2スレッドでディスパッチャーを作成
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // シーンの作成
    gScene = gPhysics->createScene(sceneDesc);
}

void PhysXLib::Update(float elapsedTime)
{
    // シミュレーション速度を指定する
    gScene->simulate(elapsedTime);
    // PhysXの処理が終わるまで待つ
    gScene->fetchResults(true);
}

void PhysXLib::GenerateCollider(Model* model)
{
    for (auto& mesh : model->GetResource()->GetMeshes())
    {
        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = mesh.vertices.size();
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = vertices;  // FBXから抽出した頂点データ

        for()
    }

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = numVertices;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices;  // FBXから抽出した頂点データ

    meshDesc.triangles.count = numTriangles;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = indices;  // FBXから抽出したインデックスデータ

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    if (!cooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
        // メッシュの生成が失敗
    }

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);
}
