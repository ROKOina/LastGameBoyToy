#include "FrustumCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Scene/SceneManager.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Camera/FPSCameraCom.h"
#include "Component/Camera/EventCameraCom.h"
#include "Graphics\Graphics.h"

//������
void FrustumCom::Start()
{
}

//�X�V����
void FrustumCom::Update(float elapsedTime)
{
    //������v�Z
    CalcurateFrustum();

    //�`�攻��
    DrawJudgement();
}

//GUI�`��
void FrustumCom::OnGUI()
{
    ImGui::Checkbox("Draw", &check);

    for (int i = 0; i < 4; i++)
    {
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            nearP[i], 0.1f, { 1,0,0,1 });
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            farP[i], 10.0f, { 1,1,0,1 });
    }
    ImGui::InputFloat3("nearP0", &nearP[0].x);
    ImGui::InputFloat3("nearP1", &nearP[1].x);
    ImGui::InputFloat3("nearP2", &nearP[2].x);
    ImGui::InputFloat3("nearP3", &nearP[3].x);

    ImGui::InputFloat3("farP0", &farP[0].x);
    ImGui::InputFloat3("farP1", &farP[1].x);
    ImGui::InputFloat3("farP2", &farP[2].x);
    ImGui::InputFloat3("farP3", &farP[3].x);
}

//�`�攻��
void FrustumCom::DrawJudgement()
{
    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();

    pos += GetGameObject()->GetComponent<RendererCom>()->GetBoundsMin() + GetGameObject()->GetComponent<RendererCom>()->GetBounds();

    if (IntersectFrustumVsAABB(pos, GetGameObject()->GetComponent<RendererCom>()->GetBounds()))
    {
        GetGameObject()->GetComponent<RendererCom>()->SetEnabled(true);

        check = true;
    }
    else
    {
        GetGameObject()->GetComponent<RendererCom>()->SetEnabled(false);

        check = false;
    }
}

//�������AABB�̓����蔻��v�Z
bool FrustumCom::IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3 radius)
{
    //�����������������ĂȂ���
    int collisionState = 0;

    for (int i = 0; i < 6; i++)
    {
        //�C�p���ʂ̖@���̐�����p����AABB��8���_�̒�����ŋߓ_�ƍŉ��_�����߂�
        //�ŒZ�_����������AABB�̒��S�Ƃ���
        DirectX::XMFLOAT3 negaPos = aabbPosition;//�ŋߓ_
        DirectX::XMFLOAT3 posiPos = aabbPosition;//�ŉ��_
        //�@��N�̐������v���X�Ȃ�A�ŉ��_�̍��W�ɔ��a�����Z����

        //�ŉ��_
        if (plane[i].normal.x > 0.0f)
        {
            posiPos.x += radius.x;
            negaPos.x -= radius.x;
        }
        else
        {
            posiPos.x -= radius.x;
            negaPos.x += radius.x;
        }
        if (plane[i].normal.y > 0.0f)
        {
            posiPos.y += radius.y;
            negaPos.y -= radius.y;
        }
        else
        {
            posiPos.y -= radius.y;
            negaPos.y += radius.y;
        }
        if (plane[i].normal.z > 0.0f)
        {
            posiPos.z += radius.z;
            negaPos.z -= radius.z;
        }
        else
        {
            posiPos.z -= radius.z;
            negaPos.z += radius.z;
        }

        //  �O���ƕ�����Ώ�����break���m�肳����
        //  ������Ԃł���΁A�X�e�[�^�X��ύX���Ă��玟�̕��ʂƂ̃`�F�b�N�ɑ�����
        //  �����ł���΁A���̂܂܎��̕��ʂƂ̃`�F�b�N�ɑ�����

        float negaN = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&negaPos)));
        float posiN = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&posiPos)));
        if (plane[i].dist < negaN && plane[i].dist < posiN)
        {
            collisionState = 0;
            continue;
        }
        else if (plane[i].dist < negaN != plane[i].dist < posiN)
        {
            collisionState = 1;
        }
        else
        {
            collisionState = 2;
            break;
        }
    }
    if (collisionState == 2)
    {
        return false;
    }
    else return true;
}

//������v�Z
void FrustumCom::CalcurateFrustum()
{
    Graphics& graphics = Graphics::Instance();

    DirectX::XMFLOAT4X4 cameraview = {};

    // FPS�J���������ŗǂ�
    if (const auto& cameraObject = GameObjectManager::Instance().Find("cameraPostPlayer"))
    {
        if (const auto& fpscamera = cameraObject->GetComponent<CameraCom>())
        {
            cameraview = fpscamera->GetView();
        }
    }

    //�r���[�v���W�F�N�V�����s����擾����
    DirectX::XMMATRIX matrix = {};
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&cameraview);
    DirectX::XMMATRIX projMat = DirectX::XMMatrixPerspectiveFovLH(70, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.01f, 1000.0f);

    matrix = viewMat * projMat;

    //�r���[�v���W�F�N�V�����s��̋t�s��
    DirectX::XMMATRIX inv_matrix = DirectX::XMMatrixInverse(nullptr, matrix);

    //�r���[�v���W�F�N�V�������̒��_�Z�o�p�ʒu�x�N�g��
    DirectX::XMVECTOR verts[8] =
    {
        //near plane corners
        {-1,-1,0},//[0]:����
        {1,-1,0},//[1]:�E��
        {1,1,0},//[2]:����
        {-1,1,0},//[3]:�E��

        //far plane cornesrs.
        {-1,-1,1},//[4]:����
        {1,-1,1},//[5]:�E��
        {1,1,1},//[6]:����
        {-1,1,1}//[7]:�E��
    };

    //�r���[�v���W�F�N�V�����s��̋t�s���p����,�e���_���Z�o����
    for (int i = 0; i < 4; ++i)
    {
        DirectX::XMStoreFloat3(&nearP[i], DirectX::XMVector3TransformCoord(verts[i], inv_matrix));
    }
    for (int i = 0; i < 4; ++i)
    {
        DirectX::XMStoreFloat3(&farP[i], DirectX::XMVector3TransformCoord(verts[i + 4], inv_matrix));
    }

    //������i�t���X�^���j���\������U���ʂ��Z�o����
    // 0:������, 1:�E����, 2:������, 3:�㑤��, 4:������,5:��O����

    DirectX::XMFLOAT4X4 matrix4X4 = {};
    DirectX::XMStoreFloat4x4(&matrix4X4, matrix);
    //�S�Ă̖ʂ̖@���͓����������悤�ɐݒ肷�邱��

    //������
    plane[0].normal.x = matrix4X4._14 + matrix4X4._11;
    plane[0].normal.y = matrix4X4._24 + matrix4X4._21;
    plane[0].normal.z = matrix4X4._34 + matrix4X4._31;
    DirectX::XMStoreFloat3(&plane[0].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[0].normal)));
    plane[0].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[0].normal), DirectX::XMLoadFloat3(&farP[0])));

    //�E����
    plane[1].normal.x = matrix4X4._14 - matrix4X4._11;
    plane[1].normal.y = matrix4X4._24 - matrix4X4._21;
    plane[1].normal.z = matrix4X4._34 - matrix4X4._31;
    DirectX::XMStoreFloat3(&plane[1].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[1].normal)));
    plane[1].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[1].normal), DirectX::XMLoadFloat3(&farP[1])));

    //������
    plane[2].normal.x = matrix4X4._14 + matrix4X4._12;
    plane[2].normal.y = matrix4X4._24 + matrix4X4._22;
    plane[2].normal.z = matrix4X4._34 + matrix4X4._32;
    DirectX::XMStoreFloat3(&plane[2].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[2].normal)));
    plane[2].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[2].normal), DirectX::XMLoadFloat3(&farP[1])));

    //�㑤��
    plane[3].normal.x = matrix4X4._14 - matrix4X4._12;
    plane[3].normal.y = matrix4X4._24 - matrix4X4._22;
    plane[3].normal.z = matrix4X4._34 - matrix4X4._32;
    DirectX::XMStoreFloat3(&plane[3].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[3].normal)));
    plane[3].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[3].normal), DirectX::XMLoadFloat3(&farP[2])));

    //������
    plane[4].normal.x = matrix4X4._14 - matrix4X4._13;
    plane[4].normal.y = matrix4X4._24 - matrix4X4._23;
    plane[4].normal.z = matrix4X4._34 - matrix4X4._33;
    DirectX::XMStoreFloat3(&plane[4].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[4].normal)));
    plane[4].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[4].normal), DirectX::XMLoadFloat3(&farP[2])));

    //��O����
    plane[5].normal.x = matrix4X4._14 + matrix4X4._13;
    plane[5].normal.y = matrix4X4._24 + matrix4X4._23;
    plane[5].normal.z = matrix4X4._34 + matrix4X4._33;
    DirectX::XMStoreFloat3(&plane[5].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[5].normal)));
    plane[5].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[5].normal), DirectX::XMLoadFloat3(&nearP[0])));

    //�e���E��outLineNorm��ʂ̖@���̊O�ς��狁�߂Đ��K��
    //�������E��
    DirectX::XMStoreFloat3(&outLineNorm[0], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[0].normal), DirectX::XMLoadFloat3(&plane[2].normal))));
    //�E�����E��
    DirectX::XMStoreFloat3(&outLineNorm[1], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[2].normal), DirectX::XMLoadFloat3(&plane[1].normal))));
    //�E�㋫�E��
    DirectX::XMStoreFloat3(&outLineNorm[2], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[1].normal), DirectX::XMLoadFloat3(&plane[3].normal))));
    //���㋫�E��
    DirectX::XMStoreFloat3(&outLineNorm[3], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[3].normal), DirectX::XMLoadFloat3(&plane[0].normal))));
}