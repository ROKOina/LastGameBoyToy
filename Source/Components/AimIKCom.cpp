#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"
#include "Character/CharacterCom.h"

//�R���X�g���N�^
AimIKCom::AimIKCom(const char* ainbonename)
{
    //���O���R�s�[
    copyname = ainbonename;
}

// �J�n����
void AimIKCom::Start()
{
    //ik������bone��T��
    SearchAimNode(copyname);
}

//�X�V����
void AimIKCom::Update(float elapsedTime)
{
    //aimik�̌v�Z
    AimIK();
}

//�v�Z
void AimIKCom::AimIK()
{
    // �Q�[���I�u�W�F�N�g�̃����_���[�R���|�[�l���g���烂�f�����擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPS�J�����̑O�������̃��[���h��Ԃł̃^�[�Q�b�g�ʒu���擾
    if (!GameObjectManager::Instance().Find("cameraPostPlayer"))
    {
        return;
    }
    DirectX::XMFLOAT3 target;
    auto& chara = GetGameObject()->AddComponent<CharacterCom>();
    int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetID();
    if (playerNetID == chara->GetNetID())
        target = GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->GetFront();
    else
        target = GetGameObject()->AddComponent<CharacterCom>()->GetFpsCameraDir();

    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);

    // �v���C���[�̃��[���h�g�����X�t�H�[���̋t�s����擾
    DirectX::XMMATRIX playerTransformInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()));

    for (size_t neckBoneIndex : AimBone)
    {
        // ���f������G�C���{�[���m�[�h���擾
        Model::Node& aimbone = model->GetNodes()[neckBoneIndex];

        // �G�C���{�[���̃��[���h��Ԃł̈ʒu���擾
        DirectX::XMFLOAT3 aimPosition = { aimbone.worldTransform._41, aimbone.worldTransform._42, aimbone.worldTransform._43 };

        // �^�[�Q�b�g�ʒu���v���C���[�̃��[�J����Ԃɕϊ�
        DirectX::XMStoreFloat3(&target, DirectX::XMVector4Transform(targetVec, playerTransformInv));

        // �G�C���{�[������^�[�Q�b�g�ւ̃��[�J����Ԃł̃x�N�g�����v�Z
        DirectX::XMFLOAT3 toTarget = { target.x - aimPosition.x, target.y - aimPosition.y, target.z - aimPosition.z };
        DirectX::XMVECTOR toTargetVec = DirectX::XMLoadFloat3(&toTarget);

        // ���[�J����Ԃł̃A�b�v�x�N�g�����`
        DirectX::XMFLOAT3 up = { 0, 0, 1 };
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

        // �G�C���{�[���̃O���[�o���g�����X�t�H�[���̋t�s����擾
        DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&aimbone.worldTransform));

        // toTarget��up�x�N�g�����G�C���{�[���̃��[�J����Ԃɕϊ�
        toTargetVec = DirectX::XMVector3TransformNormal(toTargetVec, inverseGlobalTransform);
        upVec = DirectX::XMVector3TransformNormal(upVec, inverseGlobalTransform);

        // ��]����up�x�N�g����toTarget�x�N�g���̊O�ςƂ��Čv�Z
        DirectX::XMVECTOR axis = DirectX::XMVector3Cross(upVec, toTargetVec);

        // up�x�N�g����toTarget�x�N�g���̊Ԃ̉�]�p���v�Z
        float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(upVec, toTargetVec));

        // ��]�p�𐧌�
        angle = (std::min)(angle, DirectX::XMConvertToRadians(60.0f));

        // �J�����̌����ɂ���ĉ�]�������C��
        DirectX::XMVECTOR cameraForward = DirectX::XMLoadFloat3(&target); // �����ŃJ�����̑O���x�N�g�����g�p
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cameraForward, targetVec)) > 0)
        {
            angle = -angle;
        }

        // �v�Z�������Ɗp�x�ŉ�]�s����쐬
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(axis), angle);

        // ���݂̉�]�ƖڕW��]���擾
        DirectX::XMVECTOR targetQuat = DirectX::XMQuaternionRotationMatrix(rotation);

        //�v�Z������]���G�C���{�[���ɓK�p
        DirectX::XMStoreFloat(&aimbone.rotate.x, targetQuat);
    }
}

//ik������bone��T��
void AimIKCom::SearchAimNode(const char* ainbonename)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    for (size_t nodeIndex = 0; nodeIndex < model->GetNodes().size(); ++nodeIndex)
    {
        const Model::Node& node = model->GetNodes().at(nodeIndex);

        if (strstr(node.name, ainbonename) == node.name)
        {
            AimBone.push_back(static_cast<int>(nodeIndex));
        }
    }
}