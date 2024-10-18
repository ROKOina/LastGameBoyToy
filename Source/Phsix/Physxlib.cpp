#include "Physxlib.h"
#include "pxC"
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

bool PhysXLib::RayCast_PhysX(const PxVec3& origin, const PxVec3& unitDir, const PxReal maxDistance, PxRaycastBuffer& hitBuffer)
{
    return gScene->raycast(origin, unitDir, maxDistance, hitBuffer);
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, ModelResource* model)
{
    for (auto& mesh : model->GetMeshes())
    {
        //頂点情報をDirectXからPhysXに置き換え
        std::vector<PxVec3> vertices;
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            vertices.emplace_back(pos);
        }
        std::vector<PxU32> indices;
        for (auto& ver : mesh.indices)
        {
            indices.emplace_back(ver);
        }

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = &vertices[0];

        meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size());
        meshDesc.triangles.stride = sizeof(PxU32)*3;
        meshDesc.triangles.data = &indices[0];

        physx::PxTolerancesScale tolerances_scale;
        PxCookingParams cooking_params(tolerances_scale);
        cooking_params.convexMeshCookingType = physx::PxConvexMeshCookingType::Enum::eQUICKHULL;
        cooking_params.gaussMapLimit = 256;

        physx::PxTriangleMesh* triangle_mesh = nullptr;
        physx::PxDefaultMemoryOutputStream write_buffer;
        if (!PxCookTriangleMesh(cooking_params, meshDesc, write_buffer)) {
            assert(0 && "PxCookTriangleMesh failed.");
        }

        PxDefaultMemoryOutputStream writeBuffer;
        PxTriangleMeshCookingResult::Enum result;

        physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
        triangle_mesh = gPhysics->createTriangleMesh(read_buffer);


        physx::PxRigidActor* rigidObj = nullptr;
        if (isStatic) {
            //動かない(静的)剛体を作成
            rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
        }
        else {
            // 動かすことのできる(動的)剛体を作成
            rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
        }
        PxTriangleMeshGeometry meshGeometry(triangle_mesh);
        ;
        PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
        rigidObj->attachShape(*shape);
        shape->release();
    }

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
