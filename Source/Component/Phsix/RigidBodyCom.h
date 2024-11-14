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

public:
    enum class RigidType
    {
        Primitive, //Box��Sphere
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
};
