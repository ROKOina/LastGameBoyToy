#include <imgui.h>
#include "TransformCom.h"
#include <cmath>

// 開始処理
void TransformCom::Start()
{

}

// 更新処理
void TransformCom::Update(float elapsedTime)
{
}

// GUI描画
void TransformCom::OnGUI()
{
	//ローカルポジション
	ImGui::DragFloat3("Position", &localPosition_.x, 0.1f);

	//ワールドポジション
	DirectX::XMFLOAT3 worldPosition = worldPosition_;
	if (ImGui::DragFloat3("WorldPosition", &worldPosition.x, 0.1f))
	{
		SetWorldPosition(worldPosition);
	}

	//クォータニオン
	ImGui::DragFloat4("Rotation", &rotation_.x, 0.1f);

	//オイラー角
	DirectX::XMFLOAT3 euler = eulerRotation_;
	if (ImGui::DragFloat3("EulerRotato", &euler.x)) {
		SetEulerRotation(euler);
	}

	ImGui::DragFloat3("Scale", &scale_.x, 0.1f);
}

//行列更新
void TransformCom::UpdateTransform()
{
	// ワールド行列の更新
	DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&rotation_.dxFloat4);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(Q);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(localPosition_.x, localPosition_.y, localPosition_.z);

	DirectX::XMStoreFloat4x4(&localTransform_, S * R * T);
	DirectX::XMMATRIX L = DirectX::XMLoadFloat4x4(&parentTransform_);
	DirectX::XMMATRIX W = S * R * T * L;

	DirectX::XMStoreFloat4x4(&worldTransform_, W);

	right = { worldTransform_._11,worldTransform_._12,worldTransform_._13 };
	up = { worldTransform_._21,worldTransform_._22,worldTransform_._23 };
	forward_ = { worldTransform_._31,worldTransform_._32,worldTransform_._33 };

	worldPosition_ = { worldTransform_._41,worldTransform_._42,worldTransform_._43 };
}

//指定地点を向く
void TransformCom::LookAtTransform(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	//位置、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&worldPosition_);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, DirectX::XMVectorSubtract(Focus, Eye));
	
	rotation_ = QuaternionStruct::LookRotation(direction, up);

	UpdateTransform();
}

//指定のUpに合わせる
void TransformCom::SetUpTransform(const DirectX::XMFLOAT3& up)
{
	DirectX::XMFLOAT3 front = GetWorldFront();
	DirectX::XMFLOAT3 right;
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&front);
	DirectX::XMVECTOR Right;

	//upを基準に
	Right = DirectX::XMVector3Cross(Up, Front);
	Front = DirectX::XMVector3Cross(Right, Up);
	
	DirectX::XMStoreFloat3(&DirectX::XMFLOAT3(up), Up);
	DirectX::XMStoreFloat3(&front, Front);
	DirectX::XMStoreFloat3(&right, Right);

	//upを基準に行列を作る
	DirectX::XMFLOAT4X4 matrixUp = {
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		front.x, front.y, front.z, 0,
		0, 0, 0, 1 };
	DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&matrixUp);

	//回転を適用
	DirectX::XMStoreFloat4(&rotation_.dxFloat4, DirectX::XMQuaternionRotationMatrix(M));

	UpdateTransform();
}

void TransformCom::Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed)
{
	// 1フレームあたりの旋回速度
	float speed = turnSpeed * elapsedTime;

	DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&directionXZ);
	Direction = DirectX::XMVector3Normalize(Direction);

	DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&this->forward_);

	// 回転する任意軸を作成
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Forward, Direction);
	if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
	{
		return;
	}

	// ベクトル同士の角度の差を求める
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Forward, Direction));
	// スピードを調整 ( 向きが近くなるほど遅くなる )
	speed = min(1.0f - dot, speed);

	//if(speed )

	// 回転処理
	DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
	DirectX::XMStoreFloat4(&rotation_.dxFloat4, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&rotation_.dxFloat4), Turn));
}
