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
    //�^�[�Q�b�g�|�W�V�������擾
    targetPos[(int)Legs::RIGHT] = GetTargetPosition(Legs::RIGHT);
    targetPos[(int)Legs::LEFT] = GetTargetPosition(Legs::LEFT);

    DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();
    debug->DrawSphere(targetPos[(int)Legs::LEFT], 0.05f, { 1,0,0,1 });
    debug->DrawSphere(targetPos[(int)Legs::RIGHT], 0.05f, { 1,0,0,1 });

}

//�^�[�Q�b�g�|�W�V�����擾
DirectX::XMFLOAT3 FootIKCom::GetTargetPosition(Legs leg)
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
    float offset = 100;
    end *= offset;

    DirectX::XMFLOAT3 hitPos;
    RayCastManager::Instance().RayCast(
        start,
        end,
        hitPos);

    debug->DrawSphere(start, 0.05f, { 1,0,0,1 });


    //���C�L���X�g�̌��ʂ�Ԃ�
    return hitPos;
}

void FootIKCom::MoveBone(Legs leg)
{
    DirectX::XMFLOAT3  waistBone;   //��
    DirectX::XMFLOAT3  knessBone;   //�G
    DirectX::XMFLOAT3  ankletBone;  //����

    //�e�{�[���̃��[���h���W���擾
    //�E��
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_WAIST]->worldTransform);
        knessBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_KNEES]->worldTransform);
        ankletBone = Float4x4ToFloat3(legNodes[(int)LegNodes::RIGHT_ANKLE]->worldTransform);
       
    }
    //����
    else
    {
        waistBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_WAIST]->worldTransform);
        knessBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_KNEES]->worldTransform);
        ankletBone = Float4x4ToFloat3(legNodes[(int)LegNodes::LEFT_ANKLE]->worldTransform);
    }
    
    //�x�N�g�����Z�o
    DirectX::XMFLOAT3 waistKnessVec  = knessBone  - waistBone;   //������G�̃x�N�g��
    DirectX::XMFLOAT3 knessaAnkleVec = ankletBone - knessBone;  //�G���瑫��̃x�N�g��
    DirectX::XMFLOAT3 waistTargetVec;  //������^�[�Q�b�g�̃x�N�g��
    if ((int)leg == (int)Legs::RIGHT)
    {
        waistTargetVec = targetPos[(int)Legs::RIGHT] - waistBone;
    }
    else
    {
        waistTargetVec = targetPos[(int)Legs::LEFT] - waistBone;
    }

    //���������߂�
    float waistKnessLength  = Mathf::Length(waistKnessVec);
    float knessaAnkleLength = Mathf::Length(knessaAnkleVec);
    float waistTargetLength = Mathf::Length(waistTargetVec);



    
    float addLength = waistKnessLength + knessaAnkleLength;
    //��[�{�[�����W���^�[�Q�b�g���W�ɋ߂Â��悤�ɍ��{�{�[���ƒ��ԃ{�[������]���䂷��
    {
        auto rotateBone = [](Model::Node& bone, const DirectX::XMFLOAT3& Direction1, const DirectX::XMFLOAT3& Direction2)
            {
                //��]���Z�o
                DirectX::XMFLOAT3 WorldAixs = Mathf::Cross(Direction1,Direction2);
                if (Mathf::Equal(WorldAixs, {0,0,0})) return;
                
       

                //��]�������[�J����ԕϊ�
                DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&bone.worldTransform);
                DirectX::XMMATRIX InverseParentWorldTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
                DirectX::XMVECTOR LocalAxis = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&WorldAixs), InverseParentWorldTransform);
                LocalAxis = DirectX::XMVector3Normalize(LocalAxis);

                //�p�x���Z�o
                float dot = Mathf::Dot(Direction1, Direction2);
                float angle = acosf(dot);

                //��]�N�H�[�^�j�I���Z�o
                DirectX::XMVECTOR LocalRotationAxis = DirectX::XMQuaternionRotationAxis(LocalAxis, angle);
                //����������������localtransform����rotate���Ȃ��Ⴂ���Ȃ������H
                DirectX::XMVECTOR LocalRotation = DirectX::XMLoadFloat4(&bone.rotate);
                LocalRotation = DirectX::XMQuaternionMultiply(LocalRotation, LocalRotationAxis);
                DirectX::XMStoreFloat4(&bone.rotate, LocalRotation);
            };
        //�e�x�N�g����P�ʃx�N�g����
        DirectX::XMFLOAT3 waistTargetDirection = Mathf::Normalize(waistTargetVec);
        DirectX::XMFLOAT3 knessaAnkleDirection = Mathf::Normalize(knessaAnkleVec);
        DirectX::XMFLOAT3 waistKnessDirection  = Mathf::Normalize(waistKnessVec);

        //�^�[�Q�b�g�����ɉ�]������
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
            if ((int)leg == (int)Legs::RIGHT)
            {
                legNodes[(int)LegNodes::RIGHT_WAIST]->
            }
            
        }
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
