#include "FootIKCom.h"
#include "Math\Collision.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\System\TransformCom.h"
#include "Component\System\RayCastManager.h"
#include "Graphics\Graphics.h"
#include <Math\Mathf.h>

#define Float4x4ToFloat3(transform) DirectX::XMFLOAT3(transform._41,transform._42,transform._43)

void FootIKCom::Start()
{
    //���m�[�h��o�^
    for (int i = 0; i < (int)LegNodes::Max; i++)
    {
        for (auto& node : GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetNodes())
        {
            for (auto& layer : node.layer)
                if (layer == i + 3)
                {
                    legNodes[i] = &node;
                    if (layer == 4)
                    {
                        DirectX::XMMATRIX kneesWorldTransfrom = DirectX::XMLoadFloat4x4(&node.worldTransform);
                        DirectX::XMMATRIX PoleLocalTransform = DirectX::XMMatrixTranslation(0, 1, 0);
                        DirectX::XMMATRIX PoleWorldTransform = DirectX::XMMatrixMultiply(PoleLocalTransform, kneesWorldTransfrom);
                        DirectX::XMStoreFloat4x4(&poleLocalTransform[(int)Legs::RIGHT], PoleLocalTransform);
                        DirectX::XMStoreFloat4x4(&poleWorldTransform[(int)Legs::RIGHT], PoleWorldTransform);
                    }
                    if (layer == 6)
                    {
                        DirectX::XMMATRIX kneesWorldTransfrom = DirectX::XMLoadFloat4x4(&node.worldTransform);
                        DirectX::XMMATRIX PoleLocalTransform = DirectX::XMMatrixTranslation(0, 1, 0);
                        DirectX::XMMATRIX PoleWorldTransform = DirectX::XMMatrixMultiply(PoleLocalTransform, kneesWorldTransfrom);
                        DirectX::XMStoreFloat4x4(&poleLocalTransform[(int)Legs::LEFT], PoleLocalTransform);
                        DirectX::XMStoreFloat4x4(&poleWorldTransform[(int)Legs::LEFT], PoleWorldTransform);
                    }
                }
        }
    }

    stageModel = GameObjectManager::Instance().Find("stage")->GetComponent<RendererCom>()->GetModel();
}

void FootIKCom::Update(float elassedTime)
{
    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

    // //�^�[�Q�b�g�|�W�V�������擾
    if (GetTargetPosition(Legs::RIGHT, targetPos[(int)Legs::RIGHT])) {
        MoveBone(Legs::RIGHT);
    }

    if (GetTargetPosition(Legs::LEFT, targetPos[(int)Legs::LEFT])) {
        MoveBone(Legs::LEFT);
    }
    debug->DrawSphere(Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform), 0.1f, { 1,0,1,1 });
    debug->DrawSphere(Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform), 0.1f, { 1,0,1,1 });

    debug->DrawSphere(targetPos[(int)Legs::RIGHT], 0.1f, { 1,0,0,1 });
    debug->DrawSphere(targetPos[(int)Legs::LEFT], 0.1f, { 0,0,1,1 });
}

//�^�[�Q�b�g�|�W�V�����擾
bool FootIKCom::GetTargetPosition(Legs leg, DirectX::XMFLOAT3& resultPos)
{
    //�f�o�b�N�`��
    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

    //���C�̎n�_�A�I�_
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;

    //�E���̃^�[�Q�b�g�|�W�V����
    if ((int)leg == (int)Legs::RIGHT)
    {
        //���񂩂烌�C���΂�
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);
        start.y += 1.5f;
        //���񂩂�̃x�N�g��
        end = { start.x,start.y - 3.0f,start.z };
    }
    //�����̃^�[�Q�b�g�|�W�V����
    else
    {
        //���񂩂烌�C���΂�
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
        start.y += 1.5f;
        //���񂩂�̃x�N�g��
        end = { start.x,start.y - 3.0f,start.z };
    }

    auto line = Graphics::Instance().GetLineRenderer();
    if ((int)leg == (int)Legs::RIGHT) {
        line->AddVertex(start, { 0,1,1,1 });
        line->AddVertex(end, { 0,1,1,1 });
    }
    else {
        line->AddVertex(start, { 1,1,0,1 });
        line->AddVertex(end, { 1,1,0,1 });
    }

    bool result = RayCastManager::Instance().RayCast(
        start,
        end,
        resultPos);

    debug->DrawSphere(start, 0.08f, { 1,0,1,1 });

    if (!result) {
        //targetPos[(int)leg] = end;
    }

    //���C�L���X�g�̌��ʂ�Ԃ�
    return result;
}

void FootIKCom::MoveBone(Legs leg)
{
    // �{�[���̃��[���h���W���擾
    DirectX::XMFLOAT3 waistBonePosition = Float4x4ToFloat3(legNodes[(int)(leg == Legs::RIGHT ? LegNodes::RIGHT_WAIST : LegNodes::LEFT_WAIST)]->worldTransform);
    DirectX::XMFLOAT3 knessBonePosition = Float4x4ToFloat3(legNodes[(int)(leg == Legs::RIGHT ? LegNodes::RIGHT_KNEES : LegNodes::LEFT_KNEES)]->worldTransform);
    DirectX::XMFLOAT3 ankletBonePosition = Float4x4ToFloat3(legNodes[(int)(leg == Legs::RIGHT ? LegNodes::RIGHT_ANKLE : LegNodes::LEFT_ANKLE)]->worldTransform);

    // �x�N�g�����Z�o
    DirectX::XMFLOAT3 waistKnessVec = knessBonePosition - waistBonePosition;
    DirectX::XMFLOAT3 knessAnkleVec = ankletBonePosition - knessBonePosition;
    DirectX::XMFLOAT3 waistTargetVec = targetPos[(int)leg] - waistBonePosition;

    // ���������߂�
    float waistKnessLength = Mathf::Length(waistKnessVec);
    float knessAnkleLength = Mathf::Length(knessAnkleVec);
    float waistTargetLength = Mathf::Length(waistTargetVec);

    // �������[���̏ꍇ�͏����𒆒f
    if (waistKnessLength == 0 || knessAnkleLength == 0 || waistTargetLength == 0) {
        return;
    }

    float addLength = waistKnessLength + knessAnkleLength;

    // �x�N�g����P�ʃx�N�g����
    DirectX::XMFLOAT3 waistTargetDirection = Mathf::Normalize(waistTargetVec);
    DirectX::XMFLOAT3 knessAnkleDirection = Mathf::Normalize(knessAnkleVec);
    DirectX::XMFLOAT3 waistKnessDirection = Mathf::Normalize(waistKnessVec);

    auto rotateBone = [](Model::Node& bone, const DirectX::XMFLOAT3& Direction1, const DirectX::XMFLOAT3& Direction2)
        {
            //��]���Z�o
            DirectX::XMFLOAT3 WorldAxis = Mathf::Cross(Direction1, Direction2);
            if (Mathf::Equal(WorldAxis, { 0, 0, 0 }))
            {
                return;
            }
            if (Mathf::Equal(Direction1, Direction2))
            {
                return;
            }

            //��]�������[�J����ԕϊ�
            DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&bone.worldTransform);
            DirectX::XMMATRIX InverseParentWorldTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
            DirectX::XMVECTOR LocalAxis = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&WorldAxis), InverseParentWorldTransform);
            LocalAxis = DirectX::XMVector3Normalize(LocalAxis);

            //�p�x���Z�o
            float dot = Mathf::Dot(Direction1, Direction2);
            if (dot > 1.0f) dot = 1.0f;
            if (dot < -1.0f) dot = -1.0f;
            float angle = acosf(dot);

            //��]�N�H�[�^�j�I���Z�o
            DirectX::XMVECTOR LocalRotationAxis = DirectX::XMQuaternionRotationAxis(LocalAxis, angle);
            DirectX::XMVECTOR LocalRotation = DirectX::XMLoadFloat4(&bone.rotate);
            LocalRotation = DirectX::XMQuaternionMultiply(LocalRotation, LocalRotationAxis);
            DirectX::XMStoreFloat4(&bone.rotate, LocalRotation);
        };

    // �^�[�Q�b�g�����ɉ�]������
    if (leg == Legs::RIGHT)
    {
        rotateBone(*legNodes[(int)LegNodes::RIGHT_WAIST], waistKnessDirection, waistTargetDirection);
    }
    else
    {
        rotateBone(*legNodes[(int)LegNodes::LEFT_WAIST], waistKnessDirection, waistTargetDirection);
    }

    if (addLength <= waistTargetLength)
    {
        // �^�[�Q�b�g�����ɉ�]������
        if (leg == Legs::RIGHT)
        {
            UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
            DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
            DirectX::XMFLOAT3 kneesTargetDirection = Mathf::Normalize(kneesTargetVec);
            rotateBone(*legNodes[(int)LegNodes::RIGHT_KNEES], knessAnkleDirection, kneesTargetDirection);
        }
        else
        {
            UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_WAIST]);
            DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::LEFT] - knessBonePosition;
            DirectX::XMFLOAT3 kneesTargetDirection = Mathf::Normalize(kneesTargetVec);
            rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, kneesTargetDirection);
        }
    }

    if (addLength > waistTargetLength)
    {
        // �O�̃x�N�g���Ŗʐς��Z�o
        float s = (addLength + waistTargetLength) / 2;
        float areaSquared = s * (s - waistKnessLength) * (s - knessAnkleLength) * (s - waistTargetLength);
        if (areaSquared < 0) areaSquared = 0; // ���̒l���[���ɂ���
        s = sqrtf(areaSquared);
        // �Z�o�����ʐςō������Z�o
        float t = s * 2 / waistKnessLength;
        // ���p�O�p�`�̎Εӂƍ�������p�x���Z�o
        if (waistTargetLength > 0)
        {
            float root = asinf(t / waistTargetLength);
        }
    }

    // �{�[���̃��[���h���W���X�V
    if (leg == Legs::RIGHT)
    {
        UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);

        DirectX::XMFLOAT3 knessTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
        DirectX::XMFLOAT3 knessTargetDirection = Mathf::Normalize(knessTargetVec);
        knessAnkleVec = ankletBonePosition - knessBonePosition;
        knessAnkleDirection = Mathf::Normalize(knessAnkleVec);
        rotateBone(*legNodes[(int)LegNodes::RIGHT_KNEES], knessAnkleDirection, knessTargetDirection);
        UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_ANKLE]);
    }
    else
    {
        UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_WAIST]);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);

        DirectX::XMFLOAT3 knessTargetVec = targetPos[(int)Legs::LEFT] - knessBonePosition;
        DirectX::XMFLOAT3 knessTargetDirection = Mathf::Normalize(knessTargetVec);
        knessAnkleVec = ankletBonePosition - knessBonePosition;
        knessAnkleDirection = Mathf::Normalize(knessAnkleVec);
        rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, knessTargetDirection);
        UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_ANKLE]);
    }
}

void FootIKCom::UpdateWorldTransform(Model::Node* legNode)
{
    Model::Node* node = legNode;
    DirectX::XMMATRIX LocalRotationTransform = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node->rotate));
    DirectX::XMMATRIX LocalPositionTransform = DirectX::XMMatrixTranslation(node->translate.x, node->translate.y, node->translate.z);
    DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(LocalRotationTransform, LocalPositionTransform);
    if (node->parent != nullptr)
    {
        DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&node->parent->worldTransform);
        DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(LocalTransform, ParentWorldTransform);
        DirectX::XMStoreFloat4x4(&node->worldTransform, WorldTransform);
    }
    else
    {
        DirectX::XMStoreFloat4x4(&node->worldTransform, LocalTransform);
    }
}

void FootIKCom::OnGUI()
{
    for (int i = 0; i < 6; i++)
    {
        std::string legnode = legNodes[i]->name;
        ImGui::Text(std::string(legnode).c_str());
        DirectX::XMFLOAT3 pos = Float4x4ToFloat3(legNodes[i]->worldTransform);
        ImGui::DragFloat3("Rotate", &pos.x);
    }
    ImGui::Separator();
    ImGui::DragFloat3("Right_Position", &targetPos[(int)Legs::RIGHT].x);
    ImGui::DragFloat3("Left_Position", &targetPos[(int)Legs::LEFT].x);
}