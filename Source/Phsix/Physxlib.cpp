#include "Physxlib.h"
//#pragma comment(lib, "PhysX_64.lib")
//#pragma comment(lib, "PhysXCommon_64.lib")
//#pragma comment(lib, "PhysXCooking_64.lib")
//#pragma comment(lib, "PhysXExtensions_static_64.lib")
//#pragma comment(lib, "PhysXFoundation_64.lib")
//#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
//#pragma comment(lib, "PhysXTask_static_64.lib")
//#pragma comment(lib, "SceneQuery_static_64.lib")
//#pragma comment(lib, "SimulationController_static_64.lib")

void PhysXLib::Initialize()
{
    // Foundation�̃C���X�^���X��
    //m_pFoundation.reset(PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback));
    //m_pPvd.reset(physx::PxCreatePvd(*m_pFoundation));
    //m_pPhysics.reset(PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd.get()));

    //// �����Ɏg���X���b�h���w�肷��
    //m_pDispatcher.reset(physx::PxDefaultCpuDispatcherCreate(8));
    //// ��Ԃ̐ݒ�
    //physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
    //scene_desc.gravity = physx::PxVec3(0, -9, 0);
    //scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
    //scene_desc.cpuDispatcher = m_pDispatcher.get();
    //// ��Ԃ̃C���X�^���X��
    //m_pScene.reset(m_pPhysics->createScene(scene_desc));
}

void PhysXLib::Update(float elapsedTime)
{
    //// �V�~�����[�V�������x���w�肷��
    //m_pScene->simulate(1.f / 60.f);
    //// PhysX�̏������I���܂ő҂�
    //m_pScene->fetchResults(true);
}
