#pragma once
#include "PxPhysicsAPI.h"
#include <memory>

class PhysXLib
{
private:
    PhysXLib() {};
    ~PhysXLib() {};

public:
    // インスタンス取得
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //初期化
    void Initialize();

    //終了化
    void Finalize() {};

    //更新処理
    void Update(float elapsedTime);

private:
    // PhysX内で利用するアロケーター
    physx::PxDefaultAllocator m_defaultAllocator;
    // エラー時用のコールバックでエラー内容が入ってる
    physx::PxDefaultErrorCallback m_defaultErrorCallback;
    // 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
    std::unique_ptr<physx::PxFoundation> m_pFoundation = nullptr; 
    // 実際に物理演算を行う
    std::unique_ptr<physx::PxPhysics> m_pPhysics = nullptr;
    // シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
    std::unique_ptr<physx::PxDefaultCpuDispatcher> m_pDispatcher = nullptr;
    // シミュレーションする空間の単位でActorの追加などもここで行う
    std::unique_ptr<physx::PxScene> m_pScene = nullptr;
    // PVDと通信する際に必要
    std::unique_ptr<physx::PxPvd> m_pPvd = nullptr;
};