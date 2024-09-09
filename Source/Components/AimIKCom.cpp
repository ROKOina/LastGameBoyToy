#include "AimIKCom.h"
#include "RendererCom.h"
#include "CameraCom.h"
#include "TransformCom.h"

#include "Character/CharacterCom.h"

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
}

//計算
void AimIKCom::AimIK()
{
    // レンダラーコンポーネントからモデルを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPSカメラからターゲット位置を取得
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

    // 新しい回転を適用        
    DirectX::XMVECTOR ROT = DirectX::XMQuaternionRotationRollPitchYaw(dot, 0, 0);

    // 現在の回転を補間して適用
    DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&aimbone.rotate);
    DirectX::XMVECTOR newQuat = DirectX::XMQuaternionSlerp(currentQuat, ROT, 0.2f);
    DirectX::XMStoreFloat4(&aimbone.rotate, newQuat);
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