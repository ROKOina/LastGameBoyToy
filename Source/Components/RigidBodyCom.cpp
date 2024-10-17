#include "RigidBodyCom.h"
#include "Phsix\Physxlib.h"
#include "TransformCom.h"

RigidBodyCom::RigidBodyCom(bool isStatic, NodeCollsionCom::CollsionType type)
{
    this->isStatic = isStatic;
    this->type = type;
}

void RigidBodyCom::Start()
{
    //NodeCollisionCom‚ð‚à‚Æ‚ÉColliderì¬
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject());
}

void RigidBodyCom::Update(float elapsedTime)
{
    physx::PxTransform transform = rigidActor->getGlobalPose();
    GetGameObject()->transform_->SetWorldPosition({transform.p.x,transform.p.y,transform.p.z});
    GetGameObject()->transform_->SetRotation({ transform.q.x,transform.q.y,transform.q.z,transform.q.w });
}

void RigidBodyCom::OnGUI()
{
    ImGui::Text("pos");
    ImGui::SameLine();
    ImGui::DragFloat("x", &rigidTransform.p.x);
    ImGui::SameLine();
    ImGui::DragFloat("y", &rigidTransform.p.y);
    ImGui::SameLine();
    ImGui::DragFloat("z", &rigidTransform.p.z);

    if(ImGui::Button("TransformSet"))
    {
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
        dynamicActor->addForce(rigidTransform.p, physx::PxForceMode::eIMPULSE);
    }
}
