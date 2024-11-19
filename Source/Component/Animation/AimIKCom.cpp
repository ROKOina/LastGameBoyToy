#include "AimIKCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Character\CharacterCom.h"

//�R���X�g���N�^
AimIKCom::AimIKCom(const char* playeraimbonename, const char* enemyaimbone)
{
    if (playeraimbonename)
    {
        //���O���R�s�[
        playercopyname = playeraimbonename;
    }
    else if (enemyaimbone)
    {
        enemycopyname = enemyaimbone;
    }
}

// �J�n����
void AimIKCom::Start()
{
    //ik������bone��T��
    if (playercopyname)
    {
        SearchAimNode(playercopyname);
    }
    else if (enemycopyname)
    {
        SearchAimNode(enemycopyname);
    }
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

    if (playercopyname)
    {
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
    else if (enemycopyname)
    {
        float dot = 0.0f;
        DirectX::XMFLOAT3 playerpos = {};

        //�{�X�̑O�����̃x�N�g��
        DirectX::XMFLOAT3 enemyfront = { GetGameObject()->transform_->GetWorldFront() };

        //���ς��Z�o
        auto& player = GameObjectManager::Instance().Find("player");
        dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&enemyfront)), DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&player->transform_->GetWorldPosition()), DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldPosition())))));
        playerpos = { player->transform_->GetWorldPosition().x,player->transform_->GetWorldPosition().y + offsetYpos,player->transform_->GetWorldPosition().z };

        //�t�s��𐶐�
        DirectX::XMFLOAT3 inversepos;
        DirectX::XMStoreFloat3(&inversepos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&playerpos), DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()))));

        //�����ō��W��߂��Ă���
        //inversepos.x *= -1;

        //�����A���������r���[�s����쐬
        //�r���[���t�s�񉻂��A���[���h���W�ɖ߂�
        //�s��[���N�H�[�^�j�I���ɕϊ�
        if (dot >= 0.0f)
        {
            DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&neckpos), DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixLookAtLH({ 0,0,0 }, DirectX::XMLoadFloat3(&inversepos), DirectX::XMVector3Normalize({ 0,1,0 })))), 0.1f);
            DirectX::XMStoreFloat4(&neckpos, R);
        }

        // ��̃{�[���𐧌䂷��
        Model::Node& aimbone = model->GetNodes()[AimBone[0]];
        aimbone.rotate = neckpos;
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