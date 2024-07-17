#include "FootIKCom.h"
#include "GameSource\Math\Collision.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\System\RayCastManager.h"]
#include "Graphics\Graphics.h"

#define Float4x4ToFloat3(transform) DirectX::XMFLOAT3(transform._41,transform._42,transform._43)

void FootIKCom::Start()
{
    //���m�[�h��o�^
    for (int i = 0; i < (int)LegNodes::Max; i++)
    {
        for (auto& node : GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetNodes())
        {
            if (node.layer == i + 3)
            {
                legNodes[i] = &node;
            }
        }
    }
    stageModel = GameObjectManager::Instance().Find("stage")->GetComponent<RendererCom>()->GetModel();
}

void FootIKCom::Update(float elassedTime)
{
    //Sleep(10 * 60);
    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

    // //�^�[�Q�b�g�|�W�V�������擾
     if (GetTargetPosition(Legs::RIGHT, targetPos[(int)Legs::RIGHT])) {
         MoveBone(Legs::RIGHT);
     }
     
     if (GetTargetPosition(Legs::LEFT, targetPos[(int)Legs::LEFT])) {
         MoveBone(Legs::LEFT);
     }
   
    debug->DrawSphere(targetPos[(int)Legs::RIGHT], 0.05f, { 1,0,0,1 });
    debug->DrawSphere(targetPos[(int)Legs::LEFT], 0.05f, { 0,0,1,1 });
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
        //�G���烌�C���΂�
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        //�G���瑫��̃x�N�g��
        end = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform) - Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);

    }
    //�����̃^�[�Q�b�g�|�W�V����
    else
    {
        //�G���烌�C���΂�
        start = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        //�G���瑫��̃x�N�g��
        end = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform) - Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
    }

    //���K��
    end = Mathf::Normalize(end);

    //�I�t�Z�b�g�����C��L�΂�
    float offset = 0.7f;
    end *= offset;

    end += start;

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

    debug->DrawSphere(start, 0.05f, { 1,0,1,1 });

    if (!result) {
      
        //targetPos[(int)leg] = end;
    }

    //���C�L���X�g�̌��ʂ�Ԃ�
    return result;
}

void FootIKCom::MoveBone(Legs leg)
{
    DirectX::XMFLOAT3  waistBonePosition;   //��
    DirectX::XMFLOAT3  knessBonePosition;   //�G
    DirectX::XMFLOAT3  ankletBonePosition;  //����

    //�e�{�[���̃��[���h���W���擾
    //�E��
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_WAIST]->worldTransform);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);

    }
    //����
    else
    {
        waistBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_WAIST]->worldTransform);
        knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
    }

    //�x�N�g�����Z�o
    DirectX::XMFLOAT3 waistKnessVec = knessBonePosition - waistBonePosition;   //������G�̃x�N�g��
    DirectX::XMFLOAT3 knessAnkleVec = ankletBonePosition - knessBonePosition;  //�G���瑫��̃x�N�g��
    DirectX::XMFLOAT3 waistTargetVec;  //������^�[�Q�b�g�̃x�N�g��
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistTargetVec = targetPos[(int)Legs::RIGHT] - waistBonePosition;
    }
    else
    {
        waistTargetVec = targetPos[(int)Legs::LEFT] - waistBonePosition;
    }

    //���������߂�
    float waistKnessLength = Mathf::Length(waistKnessVec);
    float knessAnkleLength = Mathf::Length(knessAnkleVec);
    float waistTargetLength = Mathf::Length(waistTargetVec);

    float addLength = waistKnessLength + knessAnkleLength;

    //��[�{�[�����W���^�[�Q�b�g���W�ɋ߂Â��悤�ɍ��{�{�[���ƒ��ԃ{�[������]���䂷��
    {
        auto rotateBone = [](Model::Node& bone, const DirectX::XMFLOAT3& Direction1, const DirectX::XMFLOAT3& Direction2)
            {
                //��]���Z�o
                DirectX::XMFLOAT3 WorldAixs = Mathf::Cross(Direction1, Direction2);
                if (Mathf::Equal(WorldAixs, { 0,0,0 }))
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
                DirectX::XMVECTOR LocalAxis = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&WorldAixs), InverseParentWorldTransform);
                LocalAxis = DirectX::XMVector3Normalize(LocalAxis);

                //�p�x���Z�o
                float dot = Mathf::Dot(Direction1, Direction2);
                if (dot == 0)
                {
                    return;
                }
                float angle = acosf(dot);

               //��]�N�H�[�^�j�I���Z�o
               DirectX::XMVECTOR LocalRotationAxis = DirectX::XMQuaternionRotationAxis(LocalAxis, angle);
               DirectX::XMVECTOR LocalRotation = DirectX::XMLoadFloat4(&bone.rotate);
               LocalRotation = DirectX::XMQuaternionMultiply(LocalRotation, LocalRotationAxis);
               DirectX::XMStoreFloat4(&bone.rotate, LocalRotation);
                
            };

        //�e�x�N�g����P�ʃx�N�g����
        DirectX::XMFLOAT3 waistTargetDirection = Mathf::Normalize(waistTargetVec);
        DirectX::XMFLOAT3 knessAnkleDirection = Mathf::Normalize(knessAnkleVec);
        DirectX::XMFLOAT3 waistKnessDirection = Mathf::Normalize(waistKnessVec);

        //�^�[�Q�b�g�����ɉ�]������
        //���̉�]
        if ((int)leg == (int)Legs::RIGHT)
        {
           rotateBone(*legNodes[(int)LegNodes::RIGHT_WAIST], waistKnessDirection, waistTargetDirection);
        }
        else
        {
           rotateBone(*legNodes[(int)LegNodes::LEFT_WAIST], waistKnessDirection, waistTargetDirection);
        }

        if (addLength <= waistTargetLength)
        {
           //�^�[�Q�b�g�����ɉ�]������
           //�G�̉�]
           if ((int)leg == (int)Legs::RIGHT)
           {
               UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
               DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
               DirectX::XMFLOAT3 kneestTargetDirection = Mathf::Normalize(kneesTargetVec);
               rotateBone(*legNodes[(int)LegNodes::RIGHT_KNEES], knessAnkleDirection, kneestTargetDirection);
           }
           else
           {
               UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_WAIST]);
               DirectX::XMFLOAT3 kneesTargetVec = targetPos[(int)Legs::LEFT] - knessBonePosition;
               DirectX::XMFLOAT3 kneestTargetDirection = Mathf::Normalize(kneesTargetVec);
               rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, kneestTargetDirection);
           }
        }

        if (addLength > waistTargetLength)
        {
         //�O�̃x�N�g���Ŗʐς��Z�o
         float s = (addLength + waistTargetLength) / 2;
         s = sqrtf(s * (s - waistKnessLength) * (s - knessAnkleLength) * (s - waistTargetLength));
         //�Z�o�����ʐςō������Z�o
         float t = s * 2 / waistKnessLength;
         //���p�O�p�`�̎Εӂƍ�������p�x���Z�o
         float root = asinf(t / waistTargetLength);
        }

        if ((int)leg == (int)Legs::RIGHT)
        {
          UpdateWorldTransform(legNodes[(int)LegNodes::RIGHT_WAIST]);
          knessBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
          ankletBonePosition = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);
          
          DirectX::XMFLOAT3 knessTargetVec = targetPos[(int)Legs::RIGHT] - knessBonePosition;
          DirectX::XMFLOAT3 knessTargetDirection = Mathf::Normalize(knessTargetVec);
          knessAnkleVec = ankletBonePosition - knessBonePosition;
          knessAnkleDirection = Mathf::Normalize(knessAnkleDirection);
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
           knessAnkleDirection = Mathf::Normalize(knessAnkleDirection);
           rotateBone(*legNodes[(int)LegNodes::LEFT_KNEES], knessAnkleDirection, knessTargetDirection);
           UpdateWorldTransform(legNodes[(int)LegNodes::LEFT_ANKLE]);
        }
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
