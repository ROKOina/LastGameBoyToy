#include "Physxlib.h"
#include "Components\TransformCom.h"

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


    //静的オブジェクトの追加
    physx::PxRigidStatic* rigid_static
        = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    // 形状(Box)を作成
    physx::PxShape* box_shape
        = gPhysics->createShape(
            // Boxの大きさ
            physx::PxBoxGeometry(5.f, 1.f, 5.f),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
    // 形状のローカル座標を設定
    box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    // 形状を紐づけ
    rigid_static->attachShape(*box_shape);
    // 剛体を空間に追加
    gScene->addActor(*rigid_static);
}

void PhysXLib::Update(float elapsedTime)
{
    // シミュレーション速度を指定する
    gScene->simulate(elapsedTime);
    // PhysXの処理が終わるまで待つ
    gScene->fetchResults(true);
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, Model* model)
{
    //for (auto& mesh : model->GetResource()->GetMeshes())
    //{
    //    PxTriangleMeshDesc meshDesc;
    //    meshDesc.points.count = mesh.vertices.size();
    //    meshDesc.points.stride = sizeof(PxVec3);
    //    meshDesc.points.data = vertices;  // FBXから抽出した頂点データ

    //    for()
    //}

    //PxTriangleMeshDesc meshDesc;
    //meshDesc.points.count = numVertices;
    //meshDesc.points.stride = sizeof(PxVec3);
    //meshDesc.points.data = vertices;  // FBXから抽出した頂点データ

    //meshDesc.triangles.count = numTriangles;
    //meshDesc.triangles.stride = 3 * sizeof(PxU32);
    //meshDesc.triangles.data = indices;  // FBXから抽出したインデックスデータ

    //PxDefaultMemoryOutputStream writeBuffer;
    //PxTriangleMeshCookingResult::Enum result;
    //if (!cooking->cookTriangleMesh(meshDesc, writeBuffer, &result)) {
    //    // メッシュの生成が失敗
    //}

    //PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    //PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

    return nullptr;
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, GameObj obj)
{
    physx::PxRigidActor* rigidObj = nullptr;

    if (isStatic) {
        //動かない(静的)剛体を作成
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    }
    else{
        // 動かすことのできる(動的)剛体を作成
        rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    }

    // 形状(Box)を作成
    physx::PxShape* shape = nullptr;

    switch (type)
    {
    case NodeCollsionCom::CollsionType::BOX:
        shape = gPhysics->createShape(
            // Boxの大きさ
            physx::PxBoxGeometry(1.f, 1.f, 1.f),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::SPHER:
        shape = gPhysics->createShape(
            // Boxの大きさ
            physx::PxSphereGeometry(1.f),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::CYLINDER:
        shape = gPhysics->createShape(
            // Boxの大きさ
            physx::PxCapsuleGeometry(1.f,1.f),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    default:
        return nullptr;
        break;
    }

    // 形状を紐づけ
    shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    rigidObj->attachShape(*shape);

    physx::PxTransform transform;
    transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };
    rigidObj->setGlobalPose(transform);

    // 剛体を空間に追加
    gScene->addActor(*rigidObj);

    return rigidObj;
}
