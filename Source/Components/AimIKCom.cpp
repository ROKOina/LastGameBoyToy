#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

#include "Character/CharacterCom.h"

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
}

//�v�Z
void AimIKCom::AimIK()
{
    // �����_���[�R���|�[�l���g���烂�f�����擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPS�J��������^�[�Q�b�g�ʒu���擾
    DirectX::XMFLOAT3 cameraForward = GetGameObject()->GetComponent<CharacterCom>()->GetFpsCameraDir();

    DirectX::XMFLOAT3 playerForward = GetGameObject()->transform_->GetWorldFront();
    DirectX::XMFLOAT3 playerRight = GetGameObject()->transform_->GetWorldRight();

    float dot = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(DirectX::XMLoadFloat3(&cameraForward), DirectX::XMLoadFloat3(&playerForward)));
    DirectX::XMFLOAT3 cross;
    DirectX::XMStoreFloat3(&cross, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&cameraForward), DirectX::XMLoadFloat3(&playerForward)));
    cross.y = 0.1f;

    DirectX::XMVECTOR Cross = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cross));
    DirectX::XMVECTOR Right = DirectX::XMLoadFloat3(&playerRight);

    if (DirectX::XMVector3Dot(Cross, Right).m128_f32[0] > 0)dot *= -1;

    Model::Node& aimbone = model->GetNodes()[AimBone[0]];

    // �V������]��K�p        
    DirectX::XMVECTOR ROT = DirectX::XMQuaternionRotationRollPitchYaw(dot, 0, 0);

    // ���݂̉�]���Ԃ��ēK�p
    DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&aimbone.rotate);
    DirectX::XMVECTOR newQuat = DirectX::XMQuaternionSlerp(currentQuat, ROT, 0.2f);
    DirectX::XMStoreFloat4(&aimbone.rotate, newQuat);
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