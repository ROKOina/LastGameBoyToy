#include "Physxlib.h"
#include "Component/System/TransformCom.h"
#include "Component/Phsix/RigidBodyCom.h"
#include "Graphics\Model\ResourceManager.h"
#include <list>

void PhysXLib::Initialize()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // PVD�Ɛڑ�����ݒ�
    m_device = physx::PxCreatePvd(*gFoundation);
    //m_transport = PxDefaultPvdFileTransportCreate("PHYSX_TEST");
    m_transport = physx::PxDefaultPvdSocketTransportCreate(HostID, PostID, TimeoutMilliSecounds);
    bool result = m_device->connect(*m_transport, physx::PxPvdInstrumentationFlag::eALL);

    // Physics�I�u�W�F�N�g�̍쐬
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, m_device);
    // �V�[���̐ݒ�
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // �d�͐ݒ�
    //  sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    gDispatcher = PxDefaultCpuDispatcherCreate(12);  // 2�X���b�h�Ńf�B�X�p�b�`���[���쐬
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    PxInitExtensions(*gPhysics, m_device);

    // �V�[���̍쐬
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
    //�I����������O�ɃV�~�����[�V�������I��炷�K�v����
    gScene->simulate(0.001f);
    gScene->fetchResults(true);

    /*
    �I�������Ƃ��ĊJ�����鏇�Ԃ́APxCloseExtensions()���֐�, PxControllerManager, PxScene, PxPhysics, PxPvd, PxDefaultCpuDispatcher, PxPvdTransport, PxFoundation �̏�
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
    // �V�~�����[�V�������x���w�肷��
    gScene->simulate(elapsedTime);
    // PhysX�̏������I���܂ő҂�
    gScene->fetchResults(true);
}

bool PhysXLib::SphereCast_PhysX(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, float radius, float dist, PxSweepBuffer& hit)
{
    //�X�t�B�A�̃W�I���g���ݒ�
    PxSphereGeometry sphereGeom(radius);     // �X�t�B�A�W�I���g�����쐬

    // 2. �X�t�B�A�̎n�_�iTransform�j�ƕ����ݒ�
    PxTransform startTransform(PxVec3(pos.x, pos.y, pos.z));  // �X�t�B�A�̎n�_
    PxVec3 direction(dir.x, dir.y, dir.z);                   // �X�t�B�A�̃L���X�g�����i�������j

    // 4. �X�t�B�A�L���X�g�̎��s
    bool answer = gScene->sweep(
        sphereGeom,              // �X�t�B�A�W�I���g��
        startTransform,          // �n�_�̃g�����X�t�H�[��
        direction,               // �L���X�g�̕���
        dist,             // �ő勗��
        hit                  // ���ʂ̃o�b�t�@
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
            // ��؂蕶�� "(" �̈ʒu������
            size_t delimiterPos = node.name.find("__");

            // ��؂蕶�������������ꍇ�A���̈ʒu�܂ł̕�����������擾
            std::string name = (delimiterPos != std::string::npos) ? node.name.substr(0, delimiterPos) : node.name;
            name += static_cast<std::string>(".mdl");
            std::string path = static_cast<std::string>("Data/MatuokaStage/") + name;

            ID3D11Device* device = Graphics::Instance().GetDevice();
            std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();

            //���\�[�X�}�l�[�W���[�ɓo�^����Ă��邩
            if (!ResourceManager::Instance().JudgeModelFilename(path.c_str()))
            {
                m->Load(device, path.c_str());
                ResourceManager::Instance().RegisterModel(path.c_str(), m);	//���\�[�X�}�l�[�W���[�ɒǉ�����
            }
            else
            {
                m = ResourceManager::Instance().LoadModelResource(path.c_str());	//���[�h����
            }

            std::vector<PxVec3> vertices;
            std::vector<PxU32> indices;

            for (auto& mesh : m->GetMeshes())
            {
                //���_����DirectX����PhysX�ɒu������
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

            //�����Ȃ�(�ÓI)���̂��쐬
            rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
            PxTransform& trans = rigidObj->getGlobalPose();

            //���b�V���f�[�^�̍쐬
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
            //�쐬�ɐ�������meshDesc��ۑ�
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

            //�����蔻��ƃ��f���̃X�P�[�������킹��
            physx::PxMeshScale s(PxVec3(scale.x, scale.y, scale.z));

            //���b�V���𓖂��蔻��ɃZ�b�g
            PxTriangleMeshGeometry meshGeometry(triangle_mesh, s);
            PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
            rigidObj->attachShape(*shape);

            // ���̂���Ԃɒǉ�
            gScene->addActor(*rigidObj);
        }
    }
}

void PhysXLib::GenerateManyCollider_Convex(ModelResource* model, float worldScale)
{
    //����̌`
    int convexIndex = INT_MAX;
    int triangleIndex = INT_MAX;
    int boxIndex = INT_MAX;

    int nodeCount = 0;

    //�X�e�[�W�̃I�u�W�F�N�g���̕����I�u�W�F�N�g�𐶐�����
    for (auto& node : model->GetNodes())
    {
        //����̐e�m�[�h���L��
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

        //����ɂ���ăr�b�g�𗧂Ă�
        int colliderType = 0;
        if (node.parentIndex == triangleIndex) { colliderType |= 1 << 0; }
        if (node.parentIndex == convexIndex) { colliderType |= 1 << 1; }
        if (node.parentIndex == boxIndex) { colliderType |= 1 << 2; }

        //�r�b�g�������Ă����ꍇModel�̌`���擾����
        if (colliderType > 0)
        {
            // ��؂蕶�� "(" �̈ʒu������
            size_t delimiterPos = node.name.find("__");

            // ��؂蕶�������������ꍇ�A���̈ʒu�܂ł̕�����������擾
            std::string name = (delimiterPos != std::string::npos) ? node.name.substr(0, delimiterPos) : node.name;
            name += static_cast<std::string>(".mdl");
            std::string path = static_cast<std::string>("Data/Model/MatuokaStage/") + name;

            ID3D11Device* device = Graphics::Instance().GetDevice();
            std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();

            //���\�[�X�}�l�[�W���[�ɓo�^����Ă��邩
            if (!ResourceManager::Instance().JudgeModelFilename(path.c_str()))
            {
                m->Load(device, path.c_str());
                ResourceManager::Instance().RegisterModel(path.c_str(), m);	//���\�[�X�}�l�[�W���[�ɒǉ�����
            }
            else
            {
                m = ResourceManager::Instance().LoadModelResource(path.c_str());	//���[�h����
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
            //    //���_����DirectX����PhysX�ɒu������
            //    for (auto& ver : mesh.vertices)
            //    {
            //        PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            //        vertices.emplace_back(pos);
            //    }
            //}

            //physx::PxRigidActor* rigidObj = nullptr;

            ////�����Ȃ�(�ÓI)���̂��쐬
            //rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
            //PxTransform& trans = rigidObj->getGlobalPose();

            ////���b�V���f�[�^�̍쐬
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

            ////�����蔻��ƃ��f���̃X�P�[�������킹��
            //physx::PxMeshScale s(PxVec3(scale.x, scale.y, scale.z));

            ////���b�V���𓖂��蔻��ɃZ�b�g
            //PxConvexMeshGeometry meshGeometry(triangle_mesh, s);
            //PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
            //rigidObj->attachShape(*shape);

            //// ���̂���Ԃɒǉ�
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
        //���_����DirectX����PhysX�ɒu������
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

    //���b�V���f�[�^�̍쐬
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
    //�쐬�ɐ�������meshDesc��ۑ�
    meshStlege[model->GetFileName()] = meshDesc;

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;

    physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
    triangle_mesh = gPhysics->createTriangleMesh(read_buffer);


    physx::PxRigidActor* rigidObj = nullptr;
    if (isStatic) {
        //�����Ȃ�(�ÓI)���̂��쐬
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
        // ���������Ƃ̂ł���(���I)���̂��쐬
        rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    }

    //�����蔻��ƃ��f���̃X�P�[�������킹��
    DirectX::XMFLOAT3 sV = scale * worldScale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));


    //���b�V���𓖂��蔻��ɃZ�b�g
    PxTriangleMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
    rigidObj->attachShape(*shape);

    // ���̂���Ԃɒǉ�
    gScene->addActor(*rigidObj);

    return rigidObj;
}

physx::PxRigidActor* PhysXLib::GenerateConvexCollider(bool isStatic, ModelResource* model, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT3& scale, float worldScale)
{
    std::vector<PxVec3> vertices;
    std::vector<PxU32> indices;

    for (auto& mesh : model->GetMeshes())
    {
        //���_����DirectX����PhysX�ɒu������
        for (auto& ver : mesh.vertices)
        {
            PxVec3 pos = { ver.position.x,ver.position.y,ver.position.z };
            vertices.emplace_back(pos);
        }
    }

    //���b�V���f�[�^�̍쐬
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

    //�����Ȃ�(�ÓI)���̂��쐬
    rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    PxTransform& trans = rigidObj->getGlobalPose();

    trans.p.x = pos.x;
    trans.p.y = pos.y;
    trans.p.z = pos.z;

    DirectX::XMFLOAT4 quatF(rotate);
    trans.q = { quatF.x,quatF.y,quatF.z, quatF.w };

    rigidObj->setGlobalPose(trans);


    //�����蔻��ƃ��f���̃X�P�[�������킹��
    DirectX::XMFLOAT3 sV = scale * worldScale;
    physx::PxMeshScale meshScale(PxVec3(sV.x, sV.y, sV.z));


    //���b�V���𓖂��蔻��ɃZ�b�g
    PxConvexMeshGeometry meshGeometry(triangle_mesh, meshScale);
    PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
    rigidObj->attachShape(*shape);

    // ���̂���Ԃɒǉ�
    gScene->addActor(*rigidObj);

    return rigidObj;
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, GameObj obj, DirectX::XMFLOAT3 scale)
{
    physx::PxRigidActor* rigidObj = nullptr;
    physx::PxTransform transform(physx::PxIdentity);
    transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };

    if (isStatic) {
        //�����Ȃ�(�ÓI)���̂��쐬
        rigidObj = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    }
    else {
        // ���������Ƃ̂ł���(���I)���̂��쐬
        auto dynamic = gPhysics->createRigidDynamic(transform);
        dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

        rigidObj = dynamic;
    }

    // �`��(Box)���쐬
    physx::PxShape* shape = nullptr;

    switch (type)
    {
    case NodeCollsionCom::CollsionType::BOX:
        shape = gPhysics->createShape(
            // Box�̑傫��
            physx::PxBoxGeometry(scale.x, scale.y, scale.z),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
        );
        break;

    case NodeCollsionCom::CollsionType::SPHER:
        shape = gPhysics->createShape(
            // SPHER�̑傫��
            physx::PxSphereGeometry((scale.x * obj->GetComponent<RigidBodyCom>()->GetNormalizeScale())),
            // ���C�W���Ɣ����W���̐ݒ�
            *gPhysics->createMaterial(0.5f, 0.5f, 0.f)
        );
        break;

    case NodeCollsionCom::CollsionType::CYLINDER:
        shape = gPhysics->createShape(
            // Box�̑傫��
            physx::PxCapsuleGeometry(1.f, 1.f),
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
    shape->release();

    //physx::PxTransform transform;
    //transform.p = { obj->transform_->GetWorldPosition().x,obj->transform_->GetWorldPosition().y,obj->transform_->GetWorldPosition().z };
    //rigidObj->setGlobalPose(transform);

    // ���̂���Ԃɒǉ�
    gScene->addActor(*rigidObj);

    return rigidObj;
}