#include <imgui.h>
#include "TransformCom.h"
#include <cmath>

// �J�n����
void TransformCom::Start()
{

}

// �X�V����
void TransformCom::Update(float elapsedTime)
{
}

// GUI�`��
void TransformCom::OnGUI()
{
	//���[�J���|�W�V����
	ImGui::DragFloat3("Position", &localPosition_.x, 0.1f);

	//���[���h�|�W�V����
	DirectX::XMFLOAT3 worldPosition = worldPosition_;
	if (ImGui::DragFloat3("WorldPosition", &worldPosition.x, 0.1f))
	{
		SetWorldPosition(worldPosition);
	}

	//�N�H�[�^�j�I��
	ImGui::DragFloat4("Rotation", &rotation_.x, 0.1f);

	//�I�C���[�p
	DirectX::XMFLOAT3 euler = eulerRotation_;
	if (ImGui::DragFloat3("EulerRotato", &euler.x)) {
		SetEulerRotation(euler);
	}

	ImGui::DragFloat3("Scale", &scale_.x, 0.1f);
}

//�s��X�V
void TransformCom::UpdateTransform()
{
	// ���[���h�s��̍X�V
	DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&rotation_.dxFloat4);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(Q);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(localPosition_.x, localPosition_.y, localPosition_.z);

	DirectX::XMStoreFloat4x4(&localTransform_, S * R * T);
	DirectX::XMMATRIX L = DirectX::XMLoadFloat4x4(&parentTransform_);
	DirectX::XMMATRIX W = S * R * T * L;

	DirectX::XMStoreFloat4x4(&worldTransform_, W);

	worldPosition_ = { worldTransform_._41,worldTransform_._42,worldTransform_._43 };
}

//�w�����������
void TransformCom::LookAtTransform(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	//�ʒu�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&worldPosition_);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, DirectX::XMVectorSubtract(Focus, Eye));
	
	rotation_ = QuaternionStruct::LookRotation(direction, up);

	UpdateTransform();
}

//�w���Up�ɍ��킹��
void TransformCom::SetUpTransform(const DirectX::XMFLOAT3& up)
{
	DirectX::XMFLOAT3 front = GetWorldFront();
	DirectX::XMFLOAT3 right;
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&front);
	DirectX::XMVECTOR Right;

	//up�����
	Right = DirectX::XMVector3Cross(Up, Front);
	Front = DirectX::XMVector3Cross(Right, Up);
	
	DirectX::XMStoreFloat3(&DirectX::XMFLOAT3(up), Up);
	DirectX::XMStoreFloat3(&front, Front);
	DirectX::XMStoreFloat3(&right, Right);

	//up����ɍs������
	DirectX::XMFLOAT4X4 matrixUp = {
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		front.x, front.y, front.z, 0,
		0, 0, 0, 1 };
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&matrixUp);

	//��]��K�p
	DirectX::XMStoreFloat4(&rotation_.dxFloat4, DirectX::XMQuaternionRotationMatrix(M));

	UpdateTransform();
}