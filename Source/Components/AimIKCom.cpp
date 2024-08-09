#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"
#include "Character/CharacterCom.h"

//コンストラクタ
AimIKCom::AimIKCom(const char* ainbonename)
{
    //名前をコピー
    copyname = ainbonename;
}

// 開始処理
void AimIKCom::Start()
{
    //ikさせるboneを探す
    SearchAimNode(copyname);
}

//更新処理
void AimIKCom::Update(float elapsedTime)
{
    //aimikの計算
    AimIK();
}

//計算
void AimIKCom::AimIK()
{
    // ゲームオブジェクトのレンダラーコンポーネントからモデルを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPSカメラの前方方向のワールド空間でのターゲット位置を取得
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

    // プレイヤーのワールドトランスフォームの逆行列を取得
    DirectX::XMMATRIX playerTransformInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()));

    for (size_t neckBoneIndex : AimBone)
    {
        // モデルからエイムボーンノードを取得
        Model::Node& aimbone = model->GetNodes()[neckBoneIndex];

        // エイムボーンのワールド空間での位置を取得
        DirectX::XMFLOAT3 aimPosition = { aimbone.worldTransform._41, aimbone.worldTransform._42, aimbone.worldTransform._43 };

        // ターゲット位置をプレイヤーのローカル空間に変換
        DirectX::XMStoreFloat3(&target, DirectX::XMVector4Transform(targetVec, playerTransformInv));

        // エイムボーンからターゲットへのローカル空間でのベクトルを計算
        DirectX::XMFLOAT3 toTarget = { target.x - aimPosition.x, target.y - aimPosition.y, target.z - aimPosition.z };
        DirectX::XMVECTOR toTargetVec = DirectX::XMLoadFloat3(&toTarget);

        // ローカル空間でのアップベクトルを定義
        DirectX::XMFLOAT3 up = { 0, 0, 1 };
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

        // エイムボーンのグローバルトランスフォームの逆行列を取得
        DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&aimbone.worldTransform));

        // toTargetとupベクトルをエイムボーンのローカル空間に変換
        toTargetVec = DirectX::XMVector3TransformNormal(toTargetVec, inverseGlobalTransform);
        upVec = DirectX::XMVector3TransformNormal(upVec, inverseGlobalTransform);

        // 回転軸をupベクトルとtoTargetベクトルの外積として計算
        DirectX::XMVECTOR axis = DirectX::XMVector3Cross(upVec, toTargetVec);

        // upベクトルとtoTargetベクトルの間の回転角を計算
        float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(upVec, toTargetVec));

        // 回転角を制限
        angle = (std::min)(angle, DirectX::XMConvertToRadians(60.0f));

        // カメラの向きによって回転方向を修正
        DirectX::XMVECTOR cameraForward = DirectX::XMLoadFloat3(&target); // ここでカメラの前方ベクトルを使用
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cameraForward, targetVec)) > 0)
        {
            angle = -angle;
        }

        // 計算した軸と角度で回転行列を作成
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(axis), angle);

        // 現在の回転と目標回転を取得
        DirectX::XMVECTOR targetQuat = DirectX::XMQuaternionRotationMatrix(rotation);

        //計算した回転をエイムボーンに適用
        DirectX::XMStoreFloat(&aimbone.rotate.x, targetQuat);
    }
}

//ikさせるboneを探す
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