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
    //NodeCollisionComをもとにCollider作成
    //rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject());
}

void RigidBodyCom::Update(float elapsedTime)
{
    if (!isStatic)
    {
        //transformを物理の結果に上書き
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
    std::shared_ptr<ModelResource> resource = std::make_shared<ModelResource>();

    if (!useResourcePath.empty())
    {
        //リソースマネージャーに登録されているか
        if (!ResourceManager::Instance().JudgeModelFilename(useResourcePath.c_str()))
        {
            resource->Load(device, useResourcePath.c_str());
            ResourceManager::Instance().RegisterModel(useResourcePath.c_str(), resource);	//リソースマネージャーに追加する
        }
        else
        {
            resource = ResourceManager::Instance().LoadModelResource(useResourcePath.c_str());	//ロードする
        }
    }
       
    switch (rigidType)
    {
    case RigidBodyCom::RigidType::Mesh:
        GenerateCollider(resource.get());
        break;
    case RigidBodyCom::RigidType::Primitive:
        //NodeCollisionを適応させる必要あり
        GenerateCollider(NodeCollsionCom::CollsionType::SPHER, GetGameObject()->transform_->GetScale());
        break;
    case RigidBodyCom::RigidType::Complex:
        PhysXLib::Instance().GenerateManyCollider_Convex(resource.get(), GetGameObject()->transform_->GetScale().x * normalizeScale);
        break;
    default:
        break;
    }
}

void RigidBodyCom::GenerateCollider(NodeCollsionCom::CollsionType type, DirectX::XMFLOAT3 scale)
{
    rigidActor = PhysXLib::Instance().GenerateCollider(isStatic, type, GetGameObject(),scale);
}

void RigidBodyCom::GenerateCollider(ModelResource* rc)
{
    rigidActor = PhysXLib::Instance().GenerateMeshCollider(
        isStatic, rc, 
        GetGameObject()->transform_->GetWorldPosition(),
        GetGameObject()->transform_->GetRotation(),
        GetGameObject()->transform_->GetScale(),
        normalizeScale);
}
