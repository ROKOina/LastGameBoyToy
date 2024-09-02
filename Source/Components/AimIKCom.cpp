#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

//�R���X�g���N�^
AimIKCom::AimIKCom(const char* aimbonename)
{
    //���O���R�s�[
    copyname = aimbonename;
}

// �J�n����
void AimIKCom::Start()
{
    //ik������bone��T��
    SearchAimNode(copyname);
}

//imgui
void AimIKCom::OnGUI()
{
    ImGui::DragFloat("Dot", &dot);
}

//�v�Z
void AimIKCom::AimIK()
{
    // �����_���[�R���|�[�l���g���烂�f�����擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPS�J��������^�[�Q�b�g�ʒu���擾
    auto cameraObj = GameObjectManager::Instance().Find("cameraPostPlayer");
    if (!cameraObj)
    {
        return;
    }
    DirectX::XMFLOAT3 cameraPosition = cameraObj->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 cameraForward = cameraObj->transform_->GetWorldFront();

    // �^�[�Q�b�g�ʒu���J�����̑O���Ɉ�苗�������i�񂾈ʒu�ɐݒ�
    DirectX::XMFLOAT3 targetPosition = {
        cameraPosition.x + cameraForward.x * 100.0f, // ������10�ɐݒ�i�����\�j
        cameraPosition.y + cameraForward.y * 100.0f,
        cameraPosition.z + cameraForward.z * 100.0f
    };

    // �v���C���[�̋t�s����擾���ă^�[�Q�b�g�ʒu�����[�J����Ԃɕϊ�
    DirectX::XMMATRIX playerTransformInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()));
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
    targetVec = DirectX::XMVector3Transform(targetVec, playerTransformInv);

    for (size_t neckBoneIndex : AimBone)
    {
        Model::Node& aimbone = model->GetNodes()[neckBoneIndex];

        // �G�C���{�[���̃��[���h��Ԉʒu���擾
        DirectX::XMFLOAT3 aimPosition = { aimbone.worldTransform._41, aimbone.worldTransform._42, aimbone.worldTransform._43 };

        // �^�[�Q�b�g�ʒu����G�C���{�[���ւ̃��[�J����Ԃł̃x�N�g�����v�Z
        DirectX::XMFLOAT3 toTarget = { targetPosition.x - aimPosition.x, targetPosition.y - aimPosition.y, targetPosition.z - aimPosition.z };
        DirectX::XMVECTOR toTargetVec = DirectX::XMLoadFloat3(&toTarget);

        // ���[�J����Ԃł̃A�b�v�x�N�g�����`
        DirectX::XMFLOAT3 up = { 0.0f, 0.0f, 1.0f };
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

        // toTarget�x�N�g����up�x�N�g�����G�C���{�[���̃��[�J����Ԃɕϊ�
        DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&aimbone.worldTransform));
        toTargetVec = DirectX::XMVector3TransformNormal(toTargetVec, inverseGlobalTransform);
        upVec = DirectX::XMVector3TransformNormal(upVec, inverseGlobalTransform);

        // ��]�����v�Z
        DirectX::XMVECTOR axis = DirectX::XMVector3Cross(upVec, toTargetVec);
        axis = DirectX::XMVector3Normalize(axis);

        // ��]�p�x���v�Z
        float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(upVec, toTargetVec));

        // ��]�p�𐧌��i�ő�20�x�j
        angle = (std::min)(angle, DirectX::XMConvertToRadians(30.0f));

        // �J�����̌����Ɋ�Â��ĉ�]�𒲐�
        DirectX::XMVECTOR cameraForwardVec = DirectX::XMLoadFloat3(&cameraForward);
        dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(cameraForwardVec), DirectX::XMVector3Normalize(targetVec)));

        if (dot < 0.0f)
        {
            angle = -angle;
        }

        // ��]�s����쐬
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axis, angle);

        // ���݂̉�]���Ԃ��ēK�p
        DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&aimbone.rotate);
        DirectX::XMVECTOR targetQuat = DirectX::XMQuaternionRotationMatrix(rotation);
        DirectX::XMVECTOR newQuat = DirectX::XMQuaternionSlerp(currentQuat, targetQuat, 0.2f);

        // �V������]��K�p
        DirectX::XMStoreFloat4(&aimbone.rotate, newQuat);
    }
}

//ik������bone��T��
void AimIKCom::SearchAimNode(const char* aimbonename)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    for (size_t nodeIndex = 0; nodeIndex < model->GetNodes().size(); ++nodeIndex)
    {
        const Model::Node& node = model->GetNodes().at(nodeIndex);

        if (strstr(node.name, aimbonename) == node.name)
        {
            AimBone.push_back(static_cast<int>(nodeIndex));
        }
    }
}