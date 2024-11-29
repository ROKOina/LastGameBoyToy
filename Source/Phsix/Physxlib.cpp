#include "Physxlib.h"
#include "Component/System/TransformCom.h"
#include "Component/Phsix/RigidBodyCom.h"
#include "Graphics\Model\ResourceManager.h"
#include <list>

void PhysXLib::Initialize()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // PVDと接続する設定
    m_device = physx::PxCreatePvd(*gFoundation);
    //m_transport = PxDefaultPvdFileTransportCreate("PHYSX_TEST");
    m_transport = physx::PxDefaultPvdSocketTransportCreate(HostID, PostID, TimeoutMilliSecounds);
    bool result = m_device->connect(*m_transport, physx::PxPvdInstrumentationFlag::eALL);

    // Physicsオブジェクトの作成
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, m_device);
    // シーンの設定
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 重力設定
    //  sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    gDispatcher = PxDefaultCpuDispatcherCreate(12);  // 2スレッドでディスパッチャーを作成
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    PxInitExtensions(*gPhysics, m_device);

    // シーンの作成
    gScene = gPhysics->createScene(sceneDesc);
    gScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
    gScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

    m_cliant = gScene->getScenePvdClient();
    if (m_cliant)
    {
        m_cliant->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        m_cliant->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        m_cliant->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

#define SAFE_RELEASE(p) {if (p) { (p)->release(); (p) = nullptr; }}
void PhysXLib::Finalize()
{
    //終了処理する前にシミュレーションを終わらす必要あり
    gScene->simulate(0.001f);
    gScene->fetchResults(true);

    /*
    終了処理として開放する順番は、PxCloseExtensions()※関数, PxControllerManager, PxScene, PxPhysics, PxPvd, PxDefaultCpuDispatcher, PxPvdTransport, PxFoundation の順
    */

    PxCloseExtensions();
    SAFE_RELEASE(gScene);
    SAFE_RELEASE(gDispatcher);
    SAFE_RELEASE(gPhysics);

    m_transport->disconnect();
    SAFE_RELEASE(m_device);
    SAFE_RELEASE(m_transport);
    SAFE_RELEASE(gFoundation);
}

void PhysXLib::Update(float elapsedTime)
{
    // シミュレーション速度を指定する
    gScene->simulate(elapsedTime);
    // PhysXの処理が終わるまで待つ
    gScene->fetchResults(true);
}

bool PhysXLib::SphereCast_PhysX(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, float radius, float dist, PxSweepBuffer& hit)
{
    //スフィアのジオメトリ設定
    PxSphereGeometry sphereGeom(radius);     // スフィアジオメトリを作成

    // 2. スフィアの始点（Transform）と方向設定
    PxTransform startTransform(PxVec3(pos.x, pos.y, pos.z));  // スフィアの始点
    PxVec3 direction(dir.x, dir.y, dir.z);                   // スフィアのキャスト方向（下向き）

    // 4. スフィアキャストの実行
    bool answer = gScene->sweep(
        sphereGeom,              // スフィアジオメトリ
        startTransform,          // 始点のトランスフォーム
        direction,               // キャストの方向
        dist,             // 最大距離
        hit                  // 結果のバッファ
    );

    return answer;
}

bool PhysXLib::RayCast_PhysX(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& unitDir, const float maxDistance, PxRaycastBuffer& hitBuffer)
{
    physx::PxVec3 start = { origin.x,origin.y,origin.z };
    physx::PxVec3 dir = { unitDir.x,unitDir.y,unitDir.z };

    return gScene->raycast(start, dir, maxDistance, hitBuffer);
}

bool PhysXLib::RayCast_PhysX(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& unitDir, const float maxDistance, PxRaycastBuffer& hitBuffer, PhysXLib::CollisionLayer layer)
{
    physx::PxVec3 start = { origin.x,origin.y,origin.z };
    physx::PxVec3 dir = { unitDir.x,unitDir.y,unitDir.z };

    PxQueryFilterData filterData;
    filterData.flags = (PxQueryFlag::Enum)layer;

    return gScene->raycast(start, dir, maxDistance, hitBuffer, PxHitFlag::eDEFAULT, filterData);
}

void PhysXLib::GenerateComplexCollider(ModelResource* model, float worldScale, CollisionLayer layer)
{
    //判定の形
    int convexIndex = INT_MAX;
    int triangleIndex = INT_MAX;
    int boxIndex = INT_MAX;

    int nodeCount = 0;

    //ステージのオブジェクト分の物理オブジェクトを生成する
    for (auto& node : model->GetNodes())
    {
        //形状の種類を記憶した親ノードを取得
        if (node.name == "Triangle") { triangleIndex = nodeCount; }
        if (node.name == "Convex"){ convexIndex = nodeCount; }
        if (node.name == "Box"){ boxIndex = nodeCount; }
        nodeCount++;

        //形状によってビットを立てる
        ShapeType shapeType = ShapeType::None;
        if (node.parentIndex == triangleIndex) { shapeType = ShapeType::Triangle; }
        if (node.parentIndex == convexIndex) { shapeType = ShapeType::Convex; }
        if (node.parentIndex == boxIndex) { shapeType = ShapeType::Box; }

        //ビットが立っていた場合Modelの形を取得する
        if (shapeType != ShapeType::None)
        {
            #pragma region モデルリソース取得

            // 区切り文字 "(" の位置を検索
            size_t delimiterPos = node.name.find("__");

            // 区切り文字が見つかった場合、その位置までの部分文字列を取得
            std::string name = (delimiterPos != std::string::npos) ? node.name.substr(0, delimiterPos) : node.name;
            name += static_cast<std::string>(".mdl");
            std::string path = static_cast<std::string>("Data/Model/MatuokaStage/") + name;

            ID3D11Device* device = Graphics::Instance().GetDevice();
            std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();

            //リソースマネージャーに登録されているか
            if (!ResourceManager::Instance().JudgeModelFilename(path.c_str()))
            {
                m->Load(device, path.c_str());
                ResourceManager::Instance().RegisterModel(path.c_str(), m);	//リソースマネージャーに追加する
            }
            else
            {
                m = ResourceManager::Instance().LoadModelResource(path.c_str());	//ロードする
            }
            #pragma endregion

            RigidData data;
            data.isStatic = true;
            data.type = shapeType;
            data.model = m.get();

            //親ノードを元にトランスフォームを更新
            DirectX::XMMATRIX transVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform(node.translate, node.rotate, node.scale));
            DirectX::XMMATRIX parentVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform({ 0,0,0 }, { worldScale, worldScale, worldScale }));
            transVec = DirectX::XMMatrixMultiply(transVec, parentVec);
            DirectX::XMFLOAT4X4 answer{};
            DirectX::XMStoreFloat4x4(&answer, transVec);

            data.pos = Mathf::TransformSamplePosition(answer);
            data.rotate = node.rotate;
            data.scale = Mathf::TransformSampleScale(answer);

            GenerateCollider(data);
        }
    }
}

physx::PxRigidActor* PhysXLib::GenerateCollider(RigidData& data)
{
    physx::PxRigidActor* rigidObj = nullptr;

    //静的or動的か決める
    if (data.isStatic)
    {
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    }
    else {

        //動的オブジェクトは質量設定
        physx::PxRigidDynamic* dynamic;
        dynamic = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
        PxRigidBodyExt::updateMassAndInertia(*dynamic, data.mass);
        dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !data.useGravity);
        rigidObj = dynamic;
    }

    //位置＆回転値
    PxTransform& trans = rigidObj->getGlobalPose();
    trans.p.x = data.pos.x;
    trans.p.y = data.pos.y;
    trans.p.z = data.pos.z;
    DirectX::XMFLOAT4 quatF(data.rotate);
    trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };
    rigidObj->setGlobalPose(trans);

    //判定の形状設定
    PxShape* shape = MakeShape(data);
    rigidObj->attachShape(*shape);

    // 剛体を空間に追加s
    gScene->addActor(*rigidObj);

    return rigidObj;
}

physx::PxShape* PhysXLib::MakeShape(RigidData& data)
{
    switch (data.type)
    {
    case PhysXLib::ShapeType::Triangle:
        return Make_TriangleShape(data);
        break;
    case PhysXLib::ShapeType::Convex:
        return Make_ConvexShape(data);
        break;
    case PhysXLib::ShapeType::Box:
        return Make_BoxShape(data);
        break;
    case PhysXLib::ShapeType::Sphere:
        return Make_SphereShape(data);
        break;

    case PhysXLib::ShapeType::Cupsule:
        break;
    case PhysXLib::ShapeType::Sylnder:
        break;
    case PhysXLib::ShapeType::None:
        assert(!"RigidDataのTypeが設定されていません");
        break;
    default:
        break;
    }

    assert(!"現在実装されていない物理形状を生成しようとしています");
    return nullptr;
}

physx::PxShape* PhysXLib::Make_TriangleShape(RigidData& data)
{
    //例外処理
    if (!data.model) assert(!"TriangleShape生成時にModelResourceが決められていない");

    #pragma region Shape生成
    //
    std::vector<PxVec3> vertices;
    std::vector<PxU32> indices;

    for (auto& mesh : data.model->GetMeshes())
    {
        //頂点情報をDirectXからPhysXに置き換え
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            vertices.emplace_back(pos);
        }

        if (mesh.subsets.size() > 0)
        {
            for (auto& ver : mesh.indices)
            {
                indices.emplace_back(ver);
            }
        }
    }

    //メッシュデータの作成
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.setToDefault();
    meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices.data();

    meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size()) / 3;
    meshDesc.triangles.stride = sizeof(PxU32) * 3;
    meshDesc.triangles.data = indices.data();

    physx::PxTolerancesScale tolerances_scale;
    PxCookingParams cooking_params(tolerances_scale);


    cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

    physx::PxTriangleMesh* triangle_mesh = nullptr;
    physx::PxDefaultMemoryOutputStream write_buffer;


    if (!PxCookTriangleMesh(cooking_params, meshDesc, write_buffer)) {
        assert(0 && "PxCookTriangleMesh failed.");
    }

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;

    physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
    triangle_mesh = gPhysics->createTriangleMesh(read_buffer);
    #pragma endregion

    //当たり判定とモデルのスケールを合わせる
    DirectX::XMFLOAT3 sV = data.scale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));

    //メッシュを当たり判定にセット
    PxTriangleMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(data.friction, data.friction, data.restitution));

    return shape;
}

physx::PxShape* PhysXLib::Make_ConvexShape(RigidData& data)
{
    if (!data.model) assert(!"ConvexShape生成時使うModelResourceが決められていない");

    #pragma region メッシュデータの作成

    std::vector<PxVec3> vertices;
    for (auto& mesh : data.model->GetMeshes())
    {
        //頂点情報をDirectXからPhysXに置き換え
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            vertices.emplace_back(pos);
        }
    }

    physx::PxConvexMeshDesc meshDesc;
    meshDesc.setToDefault();
    meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices.data();
    meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxTolerancesScale tolerances_scale;
    PxCookingParams cooking_params(tolerances_scale);

    cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

    physx::PxConvexMesh* triangle_mesh = nullptr;
    physx::PxDefaultMemoryOutputStream write_buffer;


    if (!PxCookConvexMesh(cooking_params, meshDesc, write_buffer)) {
        assert(0 && "PxCookConvexMesh failed.");
    }

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;

    physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
    triangle_mesh = gPhysics->createConvexMesh(read_buffer);

    #pragma endregion

    //形状のサイズ
    DirectX::XMFLOAT3 sV = data.scale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));

    //メッシュを当たり判定にセット
    PxConvexMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(data.friction, data.friction, data.restitution));

    return shape;
}

physx::PxShape* PhysXLib::Make_SphereShape(RigidData& data)
{
    physx::PxShape* shape = nullptr;
    shape = gPhysics->createShape(
        // SPHERの大きさ
        physx::PxSphereGeometry(data.scale.x),
        // 摩擦係数と反発係数の設定
        *gPhysics->createMaterial(data.friction, data.friction, data.restitution));

    return shape;
}

physx::PxShape* PhysXLib::Make_BoxShape(RigidData& data)
{
    physx::PxShape* shape = nullptr;
    shape = gPhysics->createShape(
        // Boxの大きさ
        physx::PxBoxGeometry(data.scale.x, data.scale.y, data.scale.z),
        // 摩擦係数と反発係数の設定
        *gPhysics->createMaterial(data.friction, data.friction, data.friction));

    return shape;
}
