#include "FootIKCom.h"
#include "GameSource\Math\Collision.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"

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
                rotate[i] = { &node.translate };
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
}

//�^�[�Q�b�g�|�W�V�����擾
DirectX::XMFLOAT3 FootIKCom::GetTargetPosition(Legs leg)
{
    HitResult hitresult;

    //���C�̎n�_�A�I�_
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;

    //�E���̃^�[�Q�b�g�|�W�V����
    if ((int)leg == (int)Legs::RIGHT)
    {
        //�G���烌�C���΂�
        start = legNodes[(int)LegNodes::RIGHT_KNEES]->translate;
        //�G���瑫��̃x�N�g��
        end = legNodes[(int)LegNodes::RIGHT_ANKLE]->translate - legNodes[(int)LegNodes::RIGHT_KNEES]->translate;
    }
    //�����̃^�[�Q�b�g�|�W�V����
    else
    {
        //�G���烌�C���΂�
        start = legNodes[(int)LegNodes::LEFT_KNEES]->translate;
        //�G���瑫��̃x�N�g��
        end = legNodes[(int)LegNodes::LEFT_ANKLE]->translate - legNodes[(int)LegNodes::LEFT_KNEES]->translate;
    }

    //���K��
    end = Mathf::Normalize(end);

    //�I�t�Z�b�g�����C��L�΂�
    float offset = 100;
    end *= offset;

    // ���C�̈ʒu�����[���h��Ԃɕϊ�
    {
        DirectX::XMFLOAT4X4 world = GetGameObject()->transform_->GetWorldTransform();
        DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&start), DirectX::XMLoadFloat4x4(&world));
        DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&end), DirectX::XMLoadFloat4x4(&world));

        DirectX::XMStoreFloat3(&start, Start);
        DirectX::XMStoreFloat3(&end, End);
    }

    Collision::IntersectRayVsModel(
        start,
        end,
        stageModel,
        hitresult
    );

    //���C�L���X�g�̌��ʂ�Ԃ�
    return hitresult.position;
}

void FootIKCom::OnGUI()
{
    for (int i = 0; i < 6; i++)
    {
        std::string legnode = legNodes[i]->name;
        ImGui::Text(std::string("Nodes" + legnode).c_str());

        ImGui::DragFloat3("Rotate", &rotate[i]->x);
    }
    ImGui::DragFloat3("Right_Position", &targetPos[(int)Legs::RIGHT].x);
    ImGui::DragFloat3("left_Position", &targetPos[(int)Legs::LEFT].y);
}
