#include "Physxlib.h"
#include "Component/System/TransformCom.h"
#include <list>

void PhysXLib::Initialize()
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    // Physics�I�u�W�F�N�g�̍쐬
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), false);
    // �V�[���̐ݒ�
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // �d�͐ݒ�
    gDispatcher = PxDefaultCpuDispatcherCreate(2);  // 2�X���b�h�Ńf�B�X�p�b�`���[���쐬
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    PxInitExtensions(*gPhysics, nullptr);

    // �V�[���̍쐬
    gScene = gPhysics->createScene(sceneDesc);
    //gScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
    //gScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

    ////�ÓI�I�u�W�F�N�g�̒ǉ�
    //rigid_static
    //    = gPhysics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    //// �`��(Box)���쐬
    //box_shape
    //    = gPhysics->createShape(
    //        // Box�̑傫��
    //        physx::PxBoxGeometry(5.f, 0.5f, 5.f),
    //        // ���C�W���Ɣ����W���̐ݒ�
    //        *gPhysics->createMaterial(0.5f, 0.5f, 0.5f)
    //    );
    //// �`��̃��[�J�����W��ݒ�
    //box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    //// �`���R�Â�
    //rigid_static->attachShape(*box_shape);
    //// ���̂���Ԃɒǉ�
    //gScene->addActor(*rigid_static);
}

#define SAFE_RELEASE(p) {if (p) { (p)->release(); (p) = nullptr; }}
void PhysXLib::Finalize()
{
    //�I����������O�ɃV�~�����[�V�������I��炷�K�v����
//m_scene->simulate(m_simurationUpdateTimeStep);
    gScene->simulate(0.001f);
    gScene->fetchResults(true);

    /*
    �I�������Ƃ��ĊJ�����鏇�Ԃ́APxCloseExtensions()���֐�, PxControllerManager, PxScene, PxPhysics, PxPvd, PxDefaultCpuDispatcher, PxPvdTransport, PxFoundation �̏�
    */

    PxCloseExtensions();
    SAFE_RELEASE(gScene);
    SAFE_RELEASE(gDispatcher);
    SAFE_RELEASE(gPhysics);
    SAFE_RELEASE(gFoundation);
}

void PhysXLib::Update(float elapsedTime)
{
    // �V�~�����[�V�������x���w�肷��
    gScene->simulate(elapsedTime);
    // PhysX�̏������I���܂ő҂�
    gScene->fetchResults(true);
}

bool PhysXLib::RayCast_PhysX(const PxVec3& origin, const PxVec3& unitDir, const PxReal maxDistance, PxRaycastBuffer& hitBuffer)
{
    return gScene->raycast(origin, unitDir, maxDistance, hitBuffer);
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, ModelResource* model)
{
    ModelResource::Node rootNode = *model->GetNodes().data();

    for (auto& mesh : model->GetMeshes())
    {
        //���_����DirectX����PhysX�ɒu������
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
        cooking_params.convexMeshCookingType = physx::PxConvexMeshCookingType::Enum::eQUICKHULL;
        cooking_params.gaussMapLimit = 256;

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
            PxTransform& pos = rigidObj->getGlobalPose();
            pos.p = { 0,0,0 };
            rigidObj->setGlobalPose(pos);
        }
        else {
            // ���������Ƃ̂ł���(���I)���̂��쐬
            rigidObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
        }

        physx::PxMeshScale scale(PxVec3(1.0f, 1.0f, 1.0f));
        PxTriangleMeshGeometry meshGeometry(triangle_mesh, scale);

        PxShape* shape = gPhysics->createShape(meshGeometry, *gPhysics->createMaterial(0.5f, 0.5f, 0.5f));
        rigidObj->attachShape(*shape);

        // ���̂���Ԃɒǉ�
        gScene->addActor(*rigidObj);

        return rigidObj;
    }

    return nullptr;
}

physx::PxRigidActor* PhysXLib::GenerateCollider(bool isStatic, NodeCollsionCom::CollsionType type, GameObj obj)
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
        rigidObj = gPhysics->createRigidDynamic(transform);
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
            physx::PxSphereGeometry(0.01f),
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