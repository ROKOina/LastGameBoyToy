#include "FrustumCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Scene/SceneManager.h"
#include "Component/Camera/CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>

// 更新処理
void FrustumCom::Update(float elapsedTime)
{
    // 視錐台計算
    CalculateFrustum();

    // 描画判定
    PerformDrawJudgement();
}

// GUI描画
void FrustumCom::OnGUI()
{
}

// 描画判定
void FrustumCom::PerformDrawJudgement()
{
    // オブジェクトの AABB 情報を取得
    const auto& renderer = GetGameObject()->GetComponent<RendererCom>();
    if (!renderer) return;

    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 bounds = renderer->GetBounds();

    //バウンディングボックス描画
    Graphics::Instance().GetDebugRenderer()->DrawBox(pos, bounds, { 0,1,0,1 });

    //描画するかを判定する
    if (IntersectFrustumVsAABB(pos, bounds))
    {
        renderer->SetEnabled(true);
    }
    else
    {
        renderer->SetEnabled(false);
    }
}

// 視錐台と AABB の当たり判定計算
bool FrustumCom::IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3& radius)
{
    for (int i = 0; i < 6; ++i)
    {
        // 最近点と最遠点を計算
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

        // 平面と最近点・最遠点の距離を計算
        float negaDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&negaPos)));
        float posiDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[i].normal), DirectX::XMLoadFloat3(&posiPos)));

        if (negaDist + plane[i].dist < 0.0f)
        {
            return false;  // 完全に外部
        }
    }
    return true;  // AABB は視錐台内または交差している
}

// 視錐台計算
void FrustumCom::CalculateFrustum() {
    Graphics& graphics = Graphics::Instance();

    // ビュー行列を取得
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

    // プロジェクション行列
    DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(fov),  // FOV
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),  // アスペクト比
        0.01f,  // ニアクリップ
        1000.0f // ファークリップ
    );

    // ビュー×プロジェクション行列
    DirectX::XMMATRIX viewProjMatrix = DirectX::XMLoadFloat4x4(&viewMatrix) * projMatrix;

    // 各平面の法線と距離を計算
    const DirectX::XMFLOAT4X4 matrix = [&]() {
        DirectX::XMFLOAT4X4 m;
        DirectX::XMStoreFloat4x4(&m, viewProjMatrix);
        return m;
        }();

    // 左平面
    CalculatePlane(matrix._14 + matrix._11, matrix._24 + matrix._21, matrix._34 + matrix._31, matrix._44 + matrix._41, 0);
    // 右平面
    CalculatePlane(matrix._14 - matrix._11, matrix._24 - matrix._21, matrix._34 - matrix._31, matrix._44 - matrix._41, 1);
    // 下平面
    CalculatePlane(matrix._14 + matrix._12, matrix._24 + matrix._22, matrix._34 + matrix._32, matrix._44 + matrix._42, 2);
    // 上平面
    CalculatePlane(matrix._14 - matrix._12, matrix._24 - matrix._22, matrix._34 - matrix._32, matrix._44 - matrix._42, 3);
    // 奥平面
    CalculatePlane(matrix._14 - matrix._13, matrix._24 - matrix._23, matrix._34 - matrix._33, matrix._44 - matrix._43, 4);
    // 手前平面
    CalculatePlane(matrix._14 + matrix._13, matrix._24 + matrix._23, matrix._34 + matrix._33, matrix._44 + matrix._43, 5);
}

// 平面計算補助関数
void FrustumCom::CalculatePlane(float a, float b, float c, float d, int index)
{
    DirectX::XMFLOAT3 normal = { a, b, c };
    DirectX::XMStoreFloat3(&plane[index].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal)));
    plane[index].dist = d / DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&normal)));
}