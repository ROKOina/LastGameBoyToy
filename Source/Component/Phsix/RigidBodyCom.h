#pragma once
#include "Phsix\Physxlib.h"
#include "Component\System\GameObject.h"
#include "Component\System\Component.h"
#include "Component\Collsion\NodeCollsionCom.h"
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

    void GenerateCollider(NodeCollsionCom::CollsionType type);
    void GenerateCollider(ModelResource* rc);

private:
    bool isStatic = false;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};
};
