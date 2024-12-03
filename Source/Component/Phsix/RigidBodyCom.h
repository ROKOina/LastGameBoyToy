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

    // ���O�擾
    const char* GetName() const override { return "RigidBody"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    void SetUp();

    void GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale);
    void GenerateCollider(ModelResource* rc, bool isConvex);

    NodeCollsionCom::CollsionType GetPrimitiveType() { return type; }
    void SetPrimitiveType(NodeCollsionCom::CollsionType t) { type = t; }
    float GetNormalizeScale() { return normalizeScale; }
    void SetNormalizeScale(float scale) { normalizeScale = scale; }
    std::string GetUseResourcePath() { return useResourcePath; }
    void SetUseResourcePath(std::string path) { useResourcePath = path; }

    physx::PxRigidActor* GetRigidActor() { return rigidActor; }
    PxTransform GetPxTransform() { return rigidActor->getGlobalPose(); }
    void SetPxTransform(PxTransform trans) { rigidActor->setGlobalPose(trans); }

    //SetUp�֐��I���ォ��g�p�\////////////////////

// �Ռ���������(isStatic��false�̏ꍇ�̂�
    void AddForce(DirectX::XMFLOAT3 force);
    //���ʐݒ�(isStatic��false�̏ꍇ�̂�
    void SetMass(float value);
    //���C�͐ݒ�
    void SetFriction(float value);
    //�����W���ݒ�
    void SetRestitution(float value);
    //�d�͂��g�p���邩�ǂ���
    void SetUseGravity(bool flag);
    void SetRigidFlag(physx::PxRigidBodyFlag::Enum rigidFlag, bool flag);
    void SetMaterial(float m, float r);

    ////////////////////////////////////////////////


public:
    enum class RigidType
    {
        PrimitiveBox, //Box��Sphere
        PrimitiveSphere, //Box��Sphere
        Mesh,
        Convex,
        Complex,   //�X�e�[�W�Ȃǂ̕�����Mesh���W�܂���Mesh
        Max,
    };

private:
    bool isStatic = false;
    RigidType rigidType = RigidType::Max;
    NodeCollsionCom::CollsionType type = NodeCollsionCom::CollsionType::MAX;

    std::string useResourcePath;
    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};
    float normalizeScale = 1.0f;

    float mass = 0.5f;
    float friction = 0.5f;
    float restitution = 0.5f;
    bool useGravity = true;
};
