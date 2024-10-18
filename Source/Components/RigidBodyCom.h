#pragma once
#include "Phsix\Physxlib.h"
#include "System\GameObject.h"
#include "System\Component.h"
#include "Components\NodeCollsionCom.h"

class RigidBodyCom : public Component
{
public:
    RigidBodyCom(bool isStatic, NodeCollsionCom::CollsionType type);
    ~RigidBodyCom() {}

    // ���O�擾
    const char* GetName() const override { return "RigidBody"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

private:
    bool isStatic = false;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    physx::PxRigidActor* rigidActor = nullptr;

    physx::PxTransform rigidTransform = {};
};

