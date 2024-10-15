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
    // Foundationのインスタンス化
    //m_pFoundation.reset(PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback));
    //m_pPvd.reset(physx::PxCreatePvd(*m_pFoundation));
    //m_pPhysics.reset(PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd.get()));

    //// 処理に使うスレッドを指定する
    //m_pDispatcher.reset(physx::PxDefaultCpuDispatcherCreate(8));
    //// 空間の設定
    //physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
    //scene_desc.gravity = physx::PxVec3(0, -9, 0);
    //scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
    //scene_desc.cpuDispatcher = m_pDispatcher.get();
    //// 空間のインスタンス化
    //m_pScene.reset(m_pPhysics->createScene(scene_desc));
}

void PhysXLib::Update(float elapsedTime)
{
    //// シミュレーション速度を指定する
    //m_pScene->simulate(1.f / 60.f);
    //// PhysXの処理が終わるまで待つ
    //m_pScene->fetchResults(true);
}
