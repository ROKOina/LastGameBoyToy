#include "RigidBodyCom.h"
#include "Phsix\Physxlib.h"
#include "Graphics\Model\ResourceManager.h"
#include "Component/System/TransformCom.h"

RigidBodyCom::RigidBodyCom(bool isStatic, PhysXLib::ShapeType type)
{
    this->data.isStatic = isStatic;
    this->data.type = type;
}

RigidBodyCom::~RigidBodyCom()
{
    if(rigidActor) rigidActor->release();
}

void RigidBodyCom::Start()
{

}

void RigidBodyCom::Update(float elapsedTime)
{
    if (!data.isStatic)
    {
        //transform�𕨗��̌��ʂɏ㏑��
        physx::PxTransform transform = rigidActor->getGlobalPose();
        GetGameObject()->transform_->SetWorldPosition({ transform.p.x,transform.p.y,transform.p.z });
        GetGameObject()->transform_->SetRotation({ transform.q.x,transform.q.y,transform.q.z,transform.q.w });
    }
}

void RigidBodyCom::SetLayer(PhysXLib::CollisionLayer pLayer)
{
    layer = pLayer;

    if (rigidActor)
    {
        // �A�N�^�[�Ɋ֘A�t����ꂽShape�̐����擾
        PxU32 numShapes = rigidActor->getNbShapes();

        // Shape�̃|�C���^�z���p��
        std::vector<PxShape*> shapes(numShapes);

        // Shape���擾
        rigidActor->getShapes(shapes.data(), numShapes, 0);

        // Shape������
        for (PxU32 i = 0; i < numShapes; ++i) {
            PxFilterData filterData;
            filterData.word0 = (PxF32)layer; // ���̃I�u�W�F�N�g�̃��C���[
            shapes[i]->setSimulationFilterData(filterData);
        }
    }
}

void RigidBodyCom::AddForce(DirectX::XMFLOAT3 force)
{
    if (!data.isStatic)
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
    data.mass = value;

    if (rigidActor)
    {
        // ���ʂ�ݒ�
        if (!data.isStatic) {

            //Dynamic�ɃL���X�g���ݒ�
            PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
            PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, data.mass);
        }
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
    data.friction = value;
    if(rigidActor)SetMaterial(data.friction, data.restitution);
}

void RigidBodyCom::SetRestitution(float value)
{
    data.restitution = value;
    if(rigidActor)SetMaterial(data.friction, data.restitution);
}

void RigidBodyCom::SetUseGravity(bool flag)
{
    data.useGravity = flag;
    if(rigidActor)rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, flag);
}

void RigidBodyCom::SetRigidFlag(physx::PxRigidBodyFlag::Enum rigidFlag, bool flag)
{
    if (rigidActor)
    {
        PxRigidDynamic* dynamicActor = rigidActor->is<PxRigidDynamic>();
        dynamicActor->setRigidBodyFlag(rigidFlag, flag);
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

    //�g�����X�t�H�[��
    data.pos = GetGameObject()->transform_->GetWorldPosition();
    data.rotate = GetGameObject()->transform_->GetRotation();  

    DirectX::XMFLOAT3 scaleV = GetGameObject().get()->transform_->GetScale();
    data.scale.x = scaleV.x * rigidScale;
    data.scale.y = scaleV.y * rigidScale;
    data.scale.z = scaleV.z * rigidScale;

    //���f��
    !useResourcePath.empty()
    ? data.model = ResourceManager::Instance().GetModelResource(useResourcePath.c_str())
    : data.model = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource_shared();

    rigidActor = PhysXLib::Instance().GenerateCollider(data);
}
