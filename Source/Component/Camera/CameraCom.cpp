#include "CameraCom.h"

#include "Component\System\TransformCom.h"
#include "Graphics/Graphics.h"
#include "Math/Mathf.h"
#include "Scene/SceneManager.h"
#include <imgui.h>
#include <cmath>

//コンストラクタで値を代入
CameraCom::CameraCom(float fovY, float aspect, float nearZ, float farZ)
{
    //パースペクティブ設定
    SetPerspectiveFov(DirectX::XMConvertToRadians(fovY), aspect, nearZ, farZ);
}

// 開始処理
void CameraCom::Start()
{
}

// 更新処理
void CameraCom::Update(float elapsedTime)
{
    //カメラシェイク
    if (shakeSec_ > 0)
    {
        shakeSec_ -= elapsedTime;

        DirectX::XMFLOAT3 upDir = GetGameObject()->transform_->GetWorldUp();
        DirectX::XMFLOAT3 rightDir = GetGameObject()->transform_->GetWorldRight();

        float random = Mathf::RandomRange(-1, 1) * shakePower_;
        upDir = { upDir.x * random,upDir.y * random,upDir.z * random };
        random = Mathf::RandomRange(-1, 1) * shakePower_;
        rightDir = { rightDir.x * random,rightDir.y * random,rightDir.z * random };

        shakePos_ = { upDir.x + rightDir.x,upDir.y + rightDir.y,upDir.z + rightDir.z };
    }
    else
    {
        shakePos_ = { 0,0,0 };
    }

    //LookAt関数使っていないなら更新する
    if (!isLookAt_)
    {
        //カメラのフォワードをフォーカスする
        DirectX::XMFLOAT3 wPos = GetGameObject()->transform_->GetWorldPosition();

        //前方向のベクトルを取得
        DirectX::XMFLOAT3 forwardNormalVec = GetGameObject()->transform_->GetWorldFront();

        //eyeを線形補完して滑らかに制御する
        DirectX::XMStoreFloat3(&wPos, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&eye_), DirectX::XMLoadFloat3(&wPos), 1.0f - eyelaperate));

        //focusを線形補完して滑らかに制御する
        DirectX::XMFLOAT3 forwardPoint = { forwardNormalVec * 2 + wPos + shakePos_ };
        DirectX::XMStoreFloat3(&focus_, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&forwardPoint), DirectX::XMLoadFloat3(&focus_), 1.0f - focuslapelate));

        SetLookAt(focus_, GetGameObject()->transform_->GetWorldUp());
    }

    isLookAt_ = false;

    //アクティブカメラ変更処理
    ChangeActiveProcess();
}

// GUI描画
void CameraCom::OnGUI()
{
    ImGui::DragFloat3("Focus", &focus_.x);
    ImGui::DragFloat3("Eye", &eye_.x);
    ImGui::DragFloat("focuslapelate", &focuslapelate, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("eyelaperate", &eyelaperate, 0.01f, 0.0f, 1.0f);

    ImGui::DragFloat3("shakePos_", &shakePos_.x);
    ImGui::DragFloat("shakeSec", &shakeSec_);
    ImGui::DragFloat("shakePower", &shakePower_);

    bool active = isActiveCamera;
    ImGui::Checkbox("isActive", &active);
    ImGui::Checkbox("isUiCreate", &isUiCreate);
    if (ImGui::Button("Active"))
    {
        ActiveCameraChange();
    }
}

//指定方向を向く
void CameraCom::SetLookAt(const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    //視点、注視点、上方向からビュー行列を作成
    DirectX::XMFLOAT3 cameraPos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&cameraPos);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

    //同じ座標の場合、少しずらす
    if (focus.x == cameraPos.x && focus.y == cameraPos.y && focus.z == cameraPos.z)
    {
        cameraPos.y += 0.0001f;
        Eye = DirectX::XMLoadFloat3(&cameraPos);
    }

    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&view_, View);

    //ビューを逆行列化し、ワールド座標に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    if (!std::isfinite(world._11))return;

    //カメラの方向を取り出す
    this->right_.x = world._11;
    this->right_.y = world._12;
    this->right_.z = world._13;

    this->up_.x = world._21;
    this->up_.y = world._22;
    this->up_.z = world._23;

    this->front_.x = world._31;
    this->front_.y = world._32;
    this->front_.z = world._33;

    GetGameObject()->transform_->SetWorldTransform(world);

    //視点、注視点を保存
    this->focus_ = focus;
    this->eye_ = cameraPos;

    isLookAt_ = true;
}

//パースペクティブ設定
void CameraCom::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    //画角、画面比率、クリップ距離からプロジェクション行列を作成
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);	//プロジェクション行列作成
    DirectX::XMStoreFloat4x4(&projection_, Projection);	//rcに渡す
    scope_ = DirectX::XMFLOAT2(nearZ, farZ);
}

void CameraCom::ActiveCameraChange()
{
    SceneManager::Instance().SetActiveCamera(GetGameObject());
}

//アクティブカメラ変更処理
void CameraCom::ChangeActiveProcess()
{
    isActiveCamera = false;

    auto& activeCamera = SceneManager::Instance().GetActiveCamera();
    if (!activeCamera)return;

    if (activeCamera->GetName() == GetGameObject()->GetName())
    {
        isActiveCamera = true;
    }
}