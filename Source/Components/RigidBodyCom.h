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
    ~RigidBodyCom();

    // ���O�擾
    const char* GetName() const override { return "RigidBody"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    void GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale);
    void GenerateCollider(ModelResource* rc);

    float GetNormalizeScale() { return normalizeScale; }
    void SetNormalizeScale(float scale) { normalizeScale = scale; }

private:
    bool isStatic = false;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};
    float normalizeScale = 1.0f;
};

