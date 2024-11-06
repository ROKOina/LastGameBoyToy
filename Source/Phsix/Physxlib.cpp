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
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    gDispatcher = PxDefaultCpuDispatcherCreate(2);  // 2スレッドでディスパッチャーを作成
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


    ////静的オブジェクトの追加
    auto rigid_static
        = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    // 形状(Box)を作成
    auto box_shape
        = gPhysics->createShape(
            // Boxの大きさ
            physx::PxBoxGeometry(5.f, 0.5f, 5.f),
            // 摩擦係数と反発係数の設定
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
    // 形状のローカル座標を設定
    box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));

    //動かない(静的)剛体を作成
    PxTransform& pos = rigid_static->getGlobalPose();
    pos.p.x = 5.0f;
    pos.p.y = 1.0f;
    pos.p.z = 0.0f;
    rigid_static->setGlobalPose(pos);

    //当たり判定とモデルのスケールを合わせる
    physx::PxMeshScale scale(PxVec3(50,50,50));

    // 形状を紐づけ
    rigid_static->attachShape(*box_shape);
    // 剛体を空間に追加
    gScene->addActor(*rigid_static);
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

bool PhysXLib::RayCast_PhysX(const PxVec3& origin, const PxVec3& unitDir, const PxReal maxDistance, PxRaycastBuffer& hitBuffer)
{
    return gScene->raycast(origin, unitDir, maxDistance, hitBuffer);
}

void PhysXLib::GenerateManyCollider(ModelResource* model)
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

            //動かない(静的)剛体を作成
            rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
            PxTransform& pos = rigidObj->getGlobalPose();
            pos.p.x = node.translate.x;
            pos.p.y = node.translate.y;
            pos.p.z = node.translate.z;

            DirectX::XMFLOAT4 quatF(node.rotate);
            pos.q = { quatF.x,quatF.y,quatF.z, quatF.w };

            rigidObj->setGlobalPose(pos);

            //当たり判定とモデルのスケールを合わせる
            float modelDefaultScale = 100.0f;//モデルエディターの１をこのプロジェクトサイズにする値
            physx::PxMeshScale scale(PxVec3(1, 1, 1));//node.scale.x * 100.0f, node.scale.y * 100.0f, node.scale.z * 100.0f));


            //メッシュを当たり判定にセット
            PxTriangleMeshGeometry meshGeometry(triangle_mesh, scale);
            PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
            rigidObj->attachShape(*shape);

            // 剛体を空間に追加
            gScene->addActor(*rigidObj);
        }
    }
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, ModelResource* model, GameObj obj)
{
    ModelResource::Node rootNode = *model->GetNodes().data();

    std::vector<PxVec3> vertices;
    std::vector<PxU32> indices;
    int meshCount = 0;
    int io = 0;

    UINT total = 0;
    auto debug = Graphics::Instance().GetDebugRenderer();

    for (auto& mesh : model->GetMeshes())
    {
        //頂点情報をDirectXからPhysXに置き換え
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            //debug->DrawSphere({ ver.position.x,ver.position.y,ver.position.z }, 0.1f, { 1,1,1,1 });

            vertices.emplace_back(pos);
        }

        if (mesh.subsets.size() > 0)
        {
            for (auto& ver : mesh.indices)
            {
                indices.emplace_back(ver + total);
            }
        }
        meshCount++;
        //total += mesh.indices.size();
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
        PxTransform& pos = rigidObj->getGlobalPose();
        pos.p.x = obj->transform_->GetWorldPosition().x;
        pos.p.y = obj->transform_->GetWorldPosition().y;
        pos.p.z = obj->transform_->GetWorldPosition().z;

        //auto quat = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(-90), 0, 0);
        //DirectX::XMFLOAT4 quatF(quat.m128_f32);
        //pos.q = { quatF.x,quatF.y,quatF.z, quatF.w };

        rigidObj->setGlobalPose(pos);
    }
    else {
        // 動かすことのできる(動的)剛体を作成
        rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    }

    //当たり判定とモデルのスケールを合わせる
    DirectX::XMFLOAT3 sV = obj->transform_->GetScale();
    float modelDefaultScale = 1;//モデルエディターの１をこのプロジェクトサイズにする値
    physx::PxMeshScale scale(PxVec3(sV.x * 1.0f, sV.y * 1.0f, sV.z * 1.0f));


    //メッシュを当たり判定にセット
    PxTriangleMeshGeometry meshGeometry(triangle_mesh, scale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
    rigidObj->attachShape(*shape);

    // 剛体を空間に追加
    gScene->addActor(*rigidObj);

    return rigidObj;

    return nullptr;
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
            physx::PxBoxGeometry(scale.x,scale.y,scale.z),
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