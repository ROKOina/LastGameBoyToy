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

    // ���O�擾
    const char* GetName() const override { return "RigidBody"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
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

    // �Ռ���������(isStatic��false�̏ꍇ�̂�,SetUp�֐��I�������g����
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

private:
    std::string useResourcePath;//RenderCom�̃��f���Ƃ͕ʂ�Mesh�Ŕ������肽���ꍇ���f���̃p�X������
    physx::PxRigidActor* rigidActor = nullptr;
    physx::PxTransform rigidTransform = {};

    PhysXLib::CollisionLayer layer = PhysXLib::CollisionLayer::Public;
    float rigidScale = 1.0f;//�������E�̑傫���𒲐�����悤�i�������ɂ����g�p���Ȃ�

    physx::PxRigidBodyFlag::Enum rigidBodyFlag = physx::PxRigidBodyFlag::eENABLE_CCD;

    PhysXLib::RigidData data;
};
