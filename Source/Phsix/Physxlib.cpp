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

void PhysXLib::GenerateManyCollider(ModelResource* model, float worldScale)
{
    for (auto& node : model->GetNodes())
    {
        if (node.parentIndex == 1)
        {
            // 区切り文字 "(" の位置を検索
            size_t delimiterPos = node.name.find("__");

            // 区切り文字が見つかった場合、その位置までの部分文字列を取得
            std::string name = (delimiterPos != std::string::npos) ? node.name.substr(0, delimiterPos) : node.name;
            name += static_cast<std::string>(".mdl");
            std::string path = static_cast<std::string>("Data/MatuokaStage/") + name;

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

            std::vector<PxVec3> vertices;
            std::vector<PxU32> indices;

            for (auto& mesh : m->GetMeshes())
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

            physx::PxRigidActor* rigidObj = nullptr;

            //動かない(静的)剛体を作成
            rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
            PxTransform& trans = rigidObj->getGlobalPose();

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
            //作成に成功したmeshDescを保存
            meshStlege[model->GetFileName()] = meshDesc;

            PxDefaultMemoryOutputStream writeBuffer;
            PxTriangleMeshCookingResult::Enum result;

            physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
            triangle_mesh = gPhysics->createTriangleMesh(read_buffer);


            DirectX::XMMATRIX transVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform(node.translate, node.rotate, node.scale));
            DirectX::XMMATRIX parentVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform({ 0,0,0 }, { worldScale, worldScale, worldScale }));
            transVec = DirectX::XMMatrixMultiply(transVec, parentVec);
            DirectX::XMFLOAT4X4 answer{};
            DirectX::XMStoreFloat4x4(&answer, transVec);

            DirectX::XMFLOAT3 pos = Mathf::TransformSamplePosition(answer);

            trans.p.x = pos.x;
            trans.p.y = pos.y;
            trans.p.z = pos.z;

            DirectX::XMFLOAT4 quatF(node.rotate);
            trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };

            rigidObj->setGlobalPose(trans);

            DirectX::XMFLOAT3 scale;
            scale.x = std::sqrt(answer._11 * answer._11 + answer._12 * answer._12 + answer._13 * answer._13);
            scale.y = std::sqrt(answer._21 * answer._21 + answer._22 * answer._22 + answer._23 * answer._23);
            scale.z = std::sqrt(answer._31 * answer._31 + answer._32 * answer._32 + answer._33 * answer._33);

            //当たり判定とモデルのスケールを合わせる
            physx::PxMeshScale s(PxVec3(scale.x, scale.y, scale.z));

            //メッシュを当たり判定にセット
            PxTriangleMeshGeometry meshGeometry(triangle_mesh, s);
            PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
            rigidObj->attachShape(*shape);

            // 剛体を空間に追加
            gScene->addActor(*rigidObj);
        }
    }
}

void PhysXLib::GenerateManyCollider_Convex(ModelResource* model, float worldScale)
{
    //判定の形
    int convexIndex = INT_MAX;
    int triangleIndex = INT_MAX;
    int boxIndex = INT_MAX;

    int nodeCount = 0;

    //ステージのオブジェクト分の物理オブジェクトを生成する
    for (auto& node : model->GetNodes())
    {
        //判定の親ノードを記憶
        if (node.name == "Triangle") { triangleIndex = nodeCount; }
        if (node.name == "Convex")
        {
            convexIndex = nodeCount;
        }
        if (node.name == "Box")
        {
            boxIndex = nodeCount;
        }
        nodeCount++;

        //判定によってビットを立てる
        int colliderType = 0;
        if (node.parentIndex == triangleIndex) { colliderType |= 1 << 0; }
        if (node.parentIndex == convexIndex) { colliderType |= 1 << 1; }
        if (node.parentIndex == boxIndex) { colliderType |= 1 << 2; }

        //ビットが立っていた場合Modelの形を取得する
        if (colliderType > 0)
        {
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

            DirectX::XMMATRIX transVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform(node.translate, node.rotate, node.scale));
            DirectX::XMMATRIX parentVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform({ 0,0,0 }, { worldScale, worldScale, worldScale }));
            transVec = DirectX::XMMatrixMultiply(transVec, parentVec);
            DirectX::XMFLOAT4X4 answer{};
            DirectX::XMStoreFloat4x4(&answer, transVec);

            switch (colliderType)
            {
            case 1 << 0://TriangleMesh
                GenerateMeshCollider(
                    true, m.get(),
                    Mathf::TransformSamplePosition(answer),
                    node.rotate,
                    Mathf::TransformSampleScale(answer), 1
                );
                break;

            case 1 << 1://ConvexMesh
                GenerateConvexCollider(
                    true, m.get(),
                    Mathf::TransformSamplePosition(answer),
                    node.rotate,
                    Mathf::TransformSampleScale(answer), 1
                );
                break;

            case 1 << 2:
                break;

            default:
                break;
            }


            //std::vector<PxVec3> vertices;
            //std::vector<PxU32> indices;

            //for (auto& mesh : m->GetMeshes())
            //{
            //    //頂点情報をDirectXからPhysXに置き換え
            //    for (auto& ver : mesh.vertices)
            //    {
            //        PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            //        vertices.emplace_back(pos);
            //    }
            //}

            //physx::PxRigidActor* rigidObj = nullptr;

            ////動かない(静的)剛体を作成
            //rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
            //PxTransform& trans = rigidObj->getGlobalPose();

            ////メッシュデータの作成
            //physx::PxConvexMeshDesc meshDesc;
            //meshDesc.setToDefault();
            //meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
            //meshDesc.points.stride = sizeof(PxVec3);
            //meshDesc.points.data = vertices.data();
            //meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

            //physx::PxTolerancesScale tolerances_scale;
            //PxCookingParams cooking_params(tolerances_scale);

            //cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
            //cooking_params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

            //physx::PxConvexMesh* triangle_mesh = nullptr;
            //physx::PxDefaultMemoryOutputStream write_buffer;


            //if (!PxCookConvexMesh(cooking_params, meshDesc, write_buffer)) {
            //    assert(0 && "PxCookTriangleMesh failed.");
            //}

            //PxDefaultMemoryOutputStream writeBuffer;
            //PxTriangleMeshCookingResult::Enum result;

            //physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
            //triangle_mesh = gPhysics->createConvexMesh(read_buffer);


            //DirectX::XMMATRIX transVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform(node.translate, node.rotate, node.scale));
            //DirectX::XMMATRIX parentVec = DirectX::XMLoadFloat4x4(&Mathf::GenerateTransform({ 0,0,0 }, { worldScale, worldScale, worldScale }));
            //transVec = DirectX::XMMatrixMultiply(transVec, parentVec);
            //DirectX::XMFLOAT4X4 answer{};
            //DirectX::XMStoreFloat4x4(&answer, transVec);

            //DirectX::XMFLOAT3 pos = Mathf::TransformSamplePosition(answer);

            //trans.p.x = pos.x;
            //trans.p.y = pos.y;
            //trans.p.z = pos.z;

            //DirectX::XMFLOAT4 quatF(node.rotate);
            //trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };

            //rigidObj->setGlobalPose(trans);

            //DirectX::XMFLOAT3 scale;
            //scale.x = std::sqrt(answer._11 * answer._11 + answer._12 * answer._12 + answer._13 * answer._13);
            //scale.y = std::sqrt(answer._21 * answer._21 + answer._22 * answer._22 + answer._23 * answer._23);
            //scale.z = std::sqrt(answer._31 * answer._31 + answer._32 * answer._32 + answer._33 * answer._33);

            ////当たり判定とモデルのスケールを合わせる
            //physx::PxMeshScale s(PxVec3(scale.x, scale.y, scale.z));

            ////メッシュを当たり判定にセット
            //PxConvexMeshGeometry meshGeometry(triangle_mesh, s);
            //PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
            //rigidObj->attachShape(*shape);

            //// 剛体を空間に追加
            //gScene->addActor(*rigidObj);
        }
    }
}

physx::PxRigidActor* PhysXLib::GenerateMeshCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale)
{
    std::vector<PxVec3> vertices;
    std::vector<PxU32> indices;

    auto debug = Graphics::Instance().GetDebugRenderer();

    for (auto& mesh : model->GetMeshes())
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
    //作成に成功したmeshDescを保存
    meshStlege[model->GetFileName()] = meshDesc;

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;

    physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
    triangle_mesh = gPhysics->createTriangleMesh(read_buffer);


    physx::PxRigidActor* rigidObj = nullptr;
    if (isStatic) {
        //動かない(静的)剛体を作成
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
        PxTransform& trans = rigidObj->getGlobalPose();

        trans.p.x = pos.x;
        trans.p.y = pos.y;
        trans.p.z = pos.z;

        DirectX::XMFLOAT4 quatF(rotate);
        trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };

        rigidObj->setGlobalPose(trans);
    }
    else {
        // 動かすことのできる(動的)剛体を作成
        rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    }

    //当たり判定とモデルのスケールを合わせる
    DirectX::XMFLOAT3 sV = scale * worldScale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));


    //メッシュを当たり判定にセット
    PxTriangleMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
    rigidObj->attachShape(*shape);

    // 剛体を空間に追加
    gScene->addActor(*rigidObj);

    return rigidObj;
}

physx::PxRigidActor* PhysXLib::GenerateConvexCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale)
{
    std::vector<PxVec3> vertices;
    std::vector<PxU32> indices;

    for (auto& mesh : model->GetMeshes())
    {
        //頂点情報をDirectXからPhysXに置き換え
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            vertices.emplace_back(pos);
        }
    }

    //メッシュデータの作成
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


    physx::PxRigidActor* rigidObj = nullptr;

    //動かない(静的)剛体を作成
    rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    PxTransform& trans = rigidObj->getGlobalPose();

    trans.p.x = pos.x;
    trans.p.y = pos.y;
    trans.p.z = pos.z;

    DirectX::XMFLOAT4 quatF(rotate);
    trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };

    rigidObj->setGlobalPose(trans);


    //当たり判定とモデルのスケールを合わせる
    DirectX::XMFLOAT3 sV = scale * worldScale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));


    //メッシュを当たり判定にセット
    PxConvexMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
    rigidObj->attachShape(*shape);

    // 剛体を空間に追加
    gScene->addActor(*rigidObj);

    return rigidObj;
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, GameObj obj, DirectX::XMFLOAT3 scale)
{
    physx::PxRigidActor* rigidObj = nullptr;
    physx::PxTransform transform(physx::PxIdentity);
    transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };

    if (isStatic) {
        //動かない(静的)剛体を作成
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    }
    else {
        // 動かすことのできる(動的)剛体を作成
        auto dynamic = gPhysics->createRigidDynamic(transform);
        dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

        rigidObj = dynamic;
    }

    // 形状(Box)を作成
    physx::PxShape* shape = nullptr;

    switch (type)
    {
    case NodeCollsionCom::CollsionType::BOX:
        shape = gPhysics->createShape(
            // Boxの大きさ
            physx::PxBoxGeometry(scale.x, scale.y, scale.z),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::SPHER:
        shape = gPhysics->createShape(
            // SPHERの大きさ
            physx::PxSphereGeometry((scale.x * obj->GetComponent<RigidBodyCom>()->GetNormalizeScale())),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.f)
        );
        break;

    case NodeCollsionCom::CollsionType::CYLINDER:
        shape = gPhysics->createShape(
            // Boxの大きさ
            physx::PxCapsuleGeometry(1.f, 1.f),
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
    shape->release();

    //physx::PxTransform transform;
    //transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };
    //rigidObj->setGlobalPose(transform);

    // 剛体を空間に追加
    gScene->addActor(*rigidObj);

    return rigidObj;
}