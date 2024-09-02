#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

//コンストラクタ
AimIKCom::AimIKCom(const char* aimbonename)
{
    //名前をコピー
    copyname = aimbonename;
}

// 開始処理
void AimIKCom::Start()
{
    //ikさせるboneを探す
    SearchAimNode(copyname);
}

//imgui
void AimIKCom::OnGUI()
{
    ImGui::DragFloat("Dot", &dot);
}

//計算
void AimIKCom::AimIK()
{
    // レンダラーコンポーネントからモデルを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPSカメラからターゲット位置を取得
    auto cameraObj = GameObjectManager::Instance().Find("cameraPostPlayer");
    if (!cameraObj)
    {
        return;
    }
    DirectX::XMFLOAT3 cameraPosition = cameraObj->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 cameraForward = cameraObj->transform_->GetWorldFront();

    // ターゲット位置をカメラの前方に一定距離だけ進んだ位置に設定
    DirectX::XMFLOAT3 targetPosition = {
        cameraPosition.x + cameraForward.x * 100.0f, // 距離を10に設定（調整可能）
        cameraPosition.y + cameraForward.y * 100.0f,
        cameraPosition.z + cameraForward.z * 100.0f
    };

    // プレイヤーの逆行列を取得してターゲット位置をローカル空間に変換
    DirectX::XMMATRIX playerTransformInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()));
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPosition);
    targetVec = DirectX::XMVector3Transform(targetVec, playerTransformInv);

    for (size_t neckBoneIndex : AimBone)
    {
        Model::Node& aimbone = model->GetNodes()[neckBoneIndex];

        // エイムボーンのワールド空間位置を取得
        DirectX::XMFLOAT3 aimPosition = { aimbone.worldTransform._41, aimbone.worldTransform._42, aimbone.worldTransform._43 };

        // ターゲット位置からエイムボーンへのローカル空間でのベクトルを計算
        DirectX::XMFLOAT3 toTarget = { targetPosition.x - aimPosition.x, targetPosition.y - aimPosition.y, targetPosition.z - aimPosition.z };
        DirectX::XMVECTOR toTargetVec = DirectX::XMLoadFloat3(&toTarget);

        // ローカル空間でのアップベクトルを定義
        DirectX::XMFLOAT3 up = { 0.0f, 0.0f, 1.0f };
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

        // toTargetベクトルとupベクトルをエイムボーンのローカル空間に変換
        DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&aimbone.worldTransform));
        toTargetVec = DirectX::XMVector3TransformNormal(toTargetVec, inverseGlobalTransform);
        upVec = DirectX::XMVector3TransformNormal(upVec, inverseGlobalTransform);

        // 回転軸を計算
        DirectX::XMVECTOR axis = DirectX::XMVector3Cross(upVec, toTargetVec);
        axis = DirectX::XMVector3Normalize(axis);

        // 回転角度を計算
        float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(upVec, toTargetVec));

        // 回転角を制限（最大20度）
        angle = (std::min)(angle, DirectX::XMConvertToRadians(30.0f));

        // カメラの向きに基づいて回転を調整
        DirectX::XMVECTOR cameraForwardVec = DirectX::XMLoadFloat3(&cameraForward);
        dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(cameraForwardVec), DirectX::XMVector3Normalize(targetVec)));

        if (dot < 0.0f)
        {
            angle = -angle;
        }

        // 回転行列を作成
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axis, angle);

        // 現在の回転を補間して適用
        DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&aimbone.rotate);
        DirectX::XMVECTOR targetQuat = DirectX::XMQuaternionRotationMatrix(rotation);
        DirectX::XMVECTOR newQuat = DirectX::XMQuaternionSlerp(currentQuat, targetQuat, 0.2f);

        // 新しい回転を適用
        DirectX::XMStoreFloat4(&aimbone.rotate, newQuat);
    }
}

//ikさせるboneを探す
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