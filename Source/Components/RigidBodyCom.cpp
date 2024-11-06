#include "RigidBodyCom.h"
#include "Phsix\Physxlib.h"
#include "TransformCom.h"

RigidBodyCom::RigidBodyCom(bool isStatic, NodeCollsionCom::CollsionType type)
{
    this->isStatic = isStatic;
    this->type = type;
}

RigidBodyCom::~RigidBodyCom()
{
    PhysXLib& p = PhysXLib::Instance();// .GetScene()->removeActor(*rigidActor);
    rigidActor->release();
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
        PxVec3 pos = rigidActor->getGlobalPose().p;

        GetGameObject()->transform_->SetWorldPosition({ pos.x,pos.y,pos.z });
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

    if(ImGui::Button("Push!!"))
    {
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
        dynamicActor->addForce(rigidTransform.p, physx::PxForceMode::eIMPULSE);
    }
}

void RigidBodyCom::GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale)
{
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject(),scale);
}

void RigidBodyCom::GenerateCollider(ModelResource* rc)
{
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, rc, GetGameObject());
}
