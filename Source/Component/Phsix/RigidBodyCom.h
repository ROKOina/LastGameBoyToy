#pragma once
#include "Phsix\Physxlib.h"
#include "Component\System\GameObject.h"
#include "Component\System\Component.h"
#include "Component\Collsion\NodeCollsionCom.h"
#include "Graphics\Model\ModelResource.h"

class RigidBodyCom : public Component
{
    enum class RigidType;
public:
    RigidBodyCom(bool isStatic, RigidType type);
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

    void GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale);
    void GenerateCollider(ModelResource* rc);

    float GetNormalizeScale() { return normalizeScale; }
    void SetNormalizeScale(float scale) { normalizeScale = scale; }

public:
    enum class RigidType
    {
        Primitive, //BoxやSphere
        Mesh,
        Complex,   //ステージなどの複数のMeshが集まったMesh
        Max,
    };

private:
    bool isStatic = false;
    RigidType rigidType = RigidType::Max;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};
    float normalizeScale = 1.0f;
};
