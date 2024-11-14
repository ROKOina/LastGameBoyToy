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
    //NodeCollisionCom‚ð‚à‚Æ‚ÉColliderì¬
    //rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject());
}

void RigidBodyCom::Update(float elapsedTime)
{
    if (!isStatic)
    {
        //transform‚ð•¨—‚ÌŒ‹‰Ê‚Éã‘‚«
        physx::PxTransform transform = rigidActor->getGlobalPose();
        GetGameObject()->transform_->SetWorldPosition({ transform.p.x,transform.p.y,transform.p.z });
        GetGameObject()->transform_->SetRotation({ transform.q.x,transform.q.y,transform.q.z,transform.q.w });
    }
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

    case RigidBodyCom::RigidType::Primitive:
        //NodeCollision‚ð“K‰ž‚³‚¹‚é•K—v‚ ‚è
        GenerateCollider(NodeCollsionCom::CollsionType::SPHER, GetGameObject()->transform_->GetScale());
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
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject(),scale);
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
