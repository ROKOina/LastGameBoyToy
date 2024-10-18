#pragma once
#include "Phsix\Physxlib.h"
#include "System\GameObject.h"
#include "System\Component.h"
#include "Components\NodeCollsionCom.h"
#include "Graphics\Model\ModelResource.h"

class RigidBodyCom : public Component
{
public:
    RigidBodyCom(bool isStatic, NodeCollsionCom::CollsionType type);
    ~RigidBodyCom() {}

    // 名前取得
    const char* GetName() const override { return "RigidBody"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

    void GenerateCollider(NodeCollsionCom::CollsionType type);
    void GenerateCollider(ModelResource* rc);

private:
    bool isStatic = false;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};
};

