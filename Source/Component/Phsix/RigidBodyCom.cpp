#include "RigidBodyCom.h"
#include "Phsix\Physxlib.h"
#include "Graphics\Model\ResourceManager.h"
#include "Component/System/TransformCom.h"

RigidBodyCom::RigidBodyCom(bool isStatic, RigidType type)
{
    this->isStatic = isStatic;
    this->rigidType = type;
}

RigidBodyCom::~RigidBodyCom()
{
    PhysXLib& p = PhysXLib::Instance();// .GetScene()->removeActor(*rigidActor);
    if(rigidActor) rigidActor->release();
}

void RigidBodyCom::Start()
{
    //NodeCollisionCom�����Ƃ�Collider�쐬
    //rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject());
}

void RigidBodyCom::Update(float elapsedTime)
{
    if (!isStatic)
    {
        //transform�𕨗��̌��ʂɏ㏑��
        physx::PxTransform transform = rigidActor->getGlobalPose();
        GetGameObject()->transform_->SetWorldPosition({ transform.p.x,transform.p.y,transform.p.z });
        GetGameObject()->transform_->SetRotation({ transform.q.x,transform.q.y,transform.q.z,transform.q.w });
    }
}

void RigidBodyCom::AddForce(DirectX::XMFLOAT3 force)
{
    if (!isStatic)
    {
        //Dynamic�ɃL���X�g
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();

        PxVec3 Force = {};
        Force.x = force.x;
        Force.y = force.y;
        Force.z = force.z;

        dynamicActor->addForce(Force, physx::PxForceMode::eIMPULSE);
    }
}

void RigidBodyCom::SetMass(float value)
{
    mass = value;

    // ���ʂ�ݒ�
    if (!isStatic) {

        //Dynamic�ɃL���X�g���ݒ�
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
        PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);
    }
}

void RigidBodyCom::SetMaterial(float f, float r)
{
    // �A�N�^�[�Ɋ֘A�t����ꂽShape�̐����擾
    PxU32 numShapes = rigidActor->getNbShapes();

    // Shape�̃|�C���^�z���p��
    std::vector<PxShape*> shapes(numShapes);

    // Shape���擾
    rigidActor->getShapes(shapes.data(), numShapes, 0);

    // Shape������
    for (PxU32 i = 0; i < numShapes; ++i) {
        PxMaterial* material = PhysXLib::Instance().GetPhysics()->createMaterial(f, f, r);
        // Shape�̏ڍׂ��m�F�i��: Geometry�̎�ށj
        shapes[i]->setMaterials(&material, 1);
    }
}

void RigidBodyCom::SetFriction(float value)
{
    friction = value;
    SetMaterial(friction, restitution);
}

void RigidBodyCom::SetRestitution(float value)
{
    restitution = value;
    SetMaterial(friction, restitution);
}

void RigidBodyCom::SetUseGravity(bool flag)
{
    rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, flag);
}

void RigidBodyCom::SetRigidFlag(physx::PxRigidBodyFlag::Enum rigidFlag, bool flag)
{
    PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
    dynamicActor->setRigidBodyFlag(rigidFlag, flag);
}

void RigidBodyCom::OnGUI()
{
    DirectX::XMFLOAT3 pos = { rigidActor->getGlobalPose().p.x,rigidActor->getGlobalPose().p.y,rigidActor->getGlobalPose().p.z };
    ImGui::DragFloat3("pos", &pos.x);

    ImGui::Text("PushPower");
    ImGui::DragFloat("x", &rigidTransform.p.x);
    ImGui::DragFloat("y", &rigidTransform.p.y);
    ImGui::DragFloat("z", &rigidTransform.p.z);

    if (ImGui::Button("Push!!"))
    {
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
        dynamicActor->addForce(rigidTransform.p, physx::PxForceMode::eIMPULSE);
    }
}

void RigidBodyCom::SetUp()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    switch (rigidType)
    {
    case RigidBodyCom::RigidType::Mesh:
    case RigidBodyCom::RigidType::Convex:

        if (!useResourcePath.empty())
        {
            GenerateCollider(
                ResourceManager::Instance().GetModelResource(useResourcePath.c_str()).get(),
                rigidType == RigidType::Convex
            );
        }
        else
        {
            GenerateCollider(
                GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource(),
                rigidType == RigidType::Convex
            );
        }

        break;

    case RigidBodyCom::RigidType::PrimitiveBox:
        //NodeCollision��K��������K�v����
        GenerateCollider(NodeCollsionCom::CollsionType::BOX, GetGameObject()->transform_->GetScale());
        break;

    case RigidBodyCom::RigidType::PrimitiveSphere:
        //NodeCollision��K��������K�v����
        GenerateCollider(NodeCollsionCom::CollsionType::SPHER, GetGameObject()->transform_->GetScale() * normalizeScale);
        break;

    case RigidBodyCom::RigidType::Complex:

        if (!useResourcePath.empty())
        {
            PhysXLib::Instance().GenerateManyCollider_Convex(
                ResourceManager::Instance().GetModelResource(useResourcePath.c_str()).get(), 
                GetGameObject()->transform_->GetScale().x * normalizeScale
            );
        }
        else
        {
            PhysXLib::Instance().GenerateManyCollider_Convex(
                GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource(),
                GetGameObject()->transform_->GetScale().x * normalizeScale
            );
        }
        break;
    default:
        break;
    }
}

void RigidBodyCom::GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale)
{
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject()->transform_->GetWorldPosition(), scale);
}

void RigidBodyCom::GenerateCollider(ModelResource* rc, bool isConvex)
{
    if (isConvex)
    {
        rigidActor = PhysXLib::Instance().GenerateConvexCollider(
            isStatic, rc,
            GetGameObject()->transform_->GetWorldPosition(),
            GetGameObject()->transform_->GetRotation(),
            GetGameObject()->transform_->GetScale(),
            normalizeScale);
    }
    else
    {
        rigidActor = PhysXLib::Instance().GenerateMeshCollider(
            isStatic, rc,
            GetGameObject()->transform_->GetWorldPosition(),
            GetGameObject()->transform_->GetRotation(),
            GetGameObject()->transform_->GetScale(),
            normalizeScale);
    }
}
