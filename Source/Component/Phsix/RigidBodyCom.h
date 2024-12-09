#pragma once
#include "Phsix\Physxlib.h"
#include "Component\System\GameObject.h"
#include "Component\System\Component.h"
#include "Component\Collsion\NodeCollsionCom.h"
#include "Graphics\Model\ModelResource.h"

class RigidBodyCom : public Component
{
public:
    RigidBodyCom(bool isStatic, PhysXLib::ShapeType type);
    ~RigidBodyCom();

    // 名前取得
    const char* GetName() const override { return "RigidBody"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    void SetUp();

    float GetRigidScale() { return rigidScale; }
    void SetRigidScale(float scale) { rigidScale = scale; }
    std::string GetUseResourcePath() { return useResourcePath; }
    void SetUseResourcePath(std::string path) { useResourcePath = path; }

    physx::PxRigidActor* GetRigidActor() { return rigidActor; }
    PxTransform GetPxTransform() { return rigidActor->getGlobalPose(); }
    void SetPxTransform(PxTransform trans) { rigidActor->setGlobalPose(trans); }

    void SetLayer(PhysXLib::CollisionLayer pLayer);

    // 衝撃を加える(isStaticがfalseの場合のみ,SetUp関数終わったら使える
    void AddForce(DirectX::XMFLOAT3 force);


    //質量設定(isStaticがfalseの場合のみ
    void SetMass(float value);
    //摩擦力設定
    void SetFriction(float value);
    //反発係数設定
    void SetRestitution(float value);
    //重力を使用するかどうか
    void SetUseGravity(bool flag);
    void SetRigidFlag(physx::PxRigidBodyFlag::Enum rigidFlag, bool flag);
    void SetMaterial(float m, float r);

private:
    std::string useResourcePath;//RenderComのモデルとは別のMeshで判定を作りたい場合モデルのパスを入れる
    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};

    PhysXLib::CollisionLayer layer = PhysXLib::CollisionLayer::Public;
    float rigidScale = 1.0f;//物理世界の大きさを調整するよう（生成時にしか使用しない

    physx::PxRigidBodyFlag::Enum rigidBodyFlag = physx::PxRigidBodyFlag::eENABLE_CCD;

    PhysXLib::RigidData data;
};
