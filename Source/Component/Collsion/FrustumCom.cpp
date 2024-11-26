#include "FrustumCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Scene/SceneManager.h"
#include "Component/Camera/CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>

// �X�V����
void FrustumCom::Update(float elapsedTime)
{
    // ������v�Z
    CalculateFrustum();

    // �`�攻��
    PerformDrawJudgement();
}

// GUI�`��
void FrustumCom::OnGUI()
{
}

// �`�攻��
void FrustumCom::PerformDrawJudgement()
{
    // �I�u�W�F�N�g�� AABB �����擾
    const auto& renderer = GetGameObject()->GetComponent<RendererCom>();
    if (!renderer) return;

    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 bounds = renderer->GetBounds();

    //�o�E���f�B���O�{�b�N�X�`��
    Graphics::Instance().GetDebugRenderer()->DrawBox(pos, bounds, { 0,1,0,1 });

    //�`�悷�邩�𔻒肷��
    if (IntersectFrustumVsAABB(pos, bounds))
    {
        renderer->SetEnabled(true);
    }
    else
    {
        renderer->SetEnabled(false);
    }
}

// ������� AABB �̓����蔻��v�Z
bool FrustumCom::IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3& radius)
{
    for (int i = 0; i < 6; ++i)
    {
        // �ŋߓ_�ƍŉ��_���v�Z
        DirectX::XMFLOAT3 negaPos = aabbPosition;
        DirectX::XMFLOAT3 posiPos = aabbPosition;

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

        // ���ʂƍŋߓ_�E�ŉ��_�̋������v�Z
        float negaDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&negaPos)));
        float posiDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&posiPos)));

        if (negaDist + plane[i].dist < 0.0f)
        {
            return false;  // ���S�ɊO��
        }
    }
    return true;  // AABB �͎�������܂��͌������Ă���
}

// ������v�Z
void FrustumCom::CalculateFrustum() {
    Graphics& graphics = Graphics::Instance();

    // �r���[�s����擾
    DirectX::XMFLOAT4X4 viewMatrix = {};
    float fov = {};
    if (const auto& cameraObject = GameObjectManager::Instance().Find("cameraPostPlayer"))
    {
        if (const auto& fpsCamera = cameraObject->GetComponent<CameraCom>())
        {
            viewMatrix = fpsCamera->GetView();
            fov = fpsCamera->GetFov();
        }
    }
    else
    {
        DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixIdentity());
        fov = 1000;
    }

    // �v���W�F�N�V�����s��
    DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(fov),  // FOV
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),  // �A�X�y�N�g��
        0.01f,  // �j�A�N���b�v
        1000.0f // �t�@�[�N���b�v
    );

    // �r���[�~�v���W�F�N�V�����s��
    DirectX::XMMATRIX viewProjMatrix = DirectX::XMLoadFloat4x4(&viewMatrix) * projMatrix;

    // �e���ʂ̖@���Ƌ������v�Z
    const DirectX::XMFLOAT4X4 matrix = [&]() {
        DirectX::XMFLOAT4X4 m;
        DirectX::XMStoreFloat4x4(&m, viewProjMatrix);
        return m;
        }();

    // ������
    CalculatePlane(matrix._14 + matrix._11, matrix._24 + matrix._21, matrix._34 + matrix._31, matrix._44 + matrix._41, 0);
    // �E����
    CalculatePlane(matrix._14 - matrix._11, matrix._24 - matrix._21, matrix._34 - matrix._31, matrix._44 - matrix._41, 1);
    // ������
    CalculatePlane(matrix._14 + matrix._12, matrix._24 + matrix._22, matrix._34 + matrix._32, matrix._44 + matrix._42, 2);
    // �㕽��
    CalculatePlane(matrix._14 - matrix._12, matrix._24 - matrix._22, matrix._34 - matrix._32, matrix._44 - matrix._42, 3);
    // ������
    CalculatePlane(matrix._14 - matrix._13, matrix._24 - matrix._23, matrix._34 - matrix._33, matrix._44 - matrix._43, 4);
    // ��O����
    CalculatePlane(matrix._14 + matrix._13, matrix._24 + matrix._23, matrix._34 + matrix._33, matrix._44 + matrix._43, 5);
}

// ���ʌv�Z�⏕�֐�
void FrustumCom::CalculatePlane(float a, float b, float c, float d, int index)
{
    DirectX::XMFLOAT3 normal = { a, b, c };
    DirectX::XMStoreFloat3(&plane[index].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal)));
    plane[index].dist = d / DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&normal)));
}