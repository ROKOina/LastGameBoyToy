#include "FrustumCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Scene/SceneManager.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Camera/FPSCameraCom.h"
#include "Component/Camera/EventCameraCom.h"
#include "Graphics\Graphics.h"

//初期化
void FrustumCom::Start()
{
}

//更新処理
void FrustumCom::Update(float elapsedTime)
{
    //視錐台計算
    CalcurateFrustum();

    //描画判定
    DrawJudgement();
}

//GUI描画
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

//描画判定
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

//視錐台とAABBの当たり判定計算
bool FrustumCom::IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3 radius)
{
    //あたったかあたってないか
    int collisionState = 0;

    for (int i = 0; i < 6; i++)
    {
        //④角平面の法線の成分を用いてAABBの8頂点の中から最近点と最遠点を求める
        //最短点をいったんAABBの中心とする
        DirectX::XMFLOAT3 negaPos = aabbPosition;//最近点
        DirectX::XMFLOAT3 posiPos = aabbPosition;//最遠点
        //法線Nの成分がプラスなら、最遠点の座標に半径を加算する

        //最遠点
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

        //  外部と分かれば処理をbreakし確定させる
        //  交差状態であれば、ステータスを変更してから次の平面とのチェックに続ける
        //  内部であれば、そのまま次の平面とのチェックに続ける

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

//視錐台計算
void FrustumCom::CalcurateFrustum()
{
    Graphics& graphics = Graphics::Instance();

    DirectX::XMFLOAT4X4 cameraview = {};

    // FPSカメラだけで良い
    if (const auto& cameraObject = GameObjectManager::Instance().Find("cameraPostPlayer"))
    {
        if (const auto& fpscamera = cameraObject->GetComponent<CameraCom>())
        {
            cameraview = fpscamera->GetView();
        }
    }

    //ビュープロジェクション行列を取得する
    DirectX::XMMATRIX matrix = {};
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&cameraview);
    DirectX::XMMATRIX projMat = DirectX::XMMatrixPerspectiveFovLH(70, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.01f, 1000.0f);

    matrix = viewMat * projMat;

    //ビュープロジェクション行列の逆行列
    DirectX::XMMATRIX inv_matrix = DirectX::XMMatrixInverse(nullptr, matrix);

    //ビュープロジェクション内の頂点算出用位置ベクトル
    DirectX::XMVECTOR verts[8] =
    {
        //near plane corners
        {-1,-1,0},//[0]:左下
        {1,-1,0},//[1]:右下
        {1,1,0},//[2]:左上
        {-1,1,0},//[3]:右上

        //far plane cornesrs.
        {-1,-1,1},//[4]:左下
        {1,-1,1},//[5]:右下
        {1,1,1},//[6]:左上
        {-1,1,1}//[7]:右上
    };

    //ビュープロジェクション行列の逆行列を用いて,各頂点を算出する
    for (int i = 0; i < 4; ++i)
    {
        DirectX::XMStoreFloat3(&nearP[i], DirectX::XMVector3TransformCoord(verts[i], inv_matrix));
    }
    for (int i = 0; i < 4; ++i)
    {
        DirectX::XMStoreFloat3(&farP[i], DirectX::XMVector3TransformCoord(verts[i + 4], inv_matrix));
    }

    //視錐台（フラスタム）を構成する６平面を算出する
    // 0:左側面, 1:右側面, 2:下側面, 3:上側面, 4:奥側面,5:手前側面

    DirectX::XMFLOAT4X4 matrix4X4 = {};
    DirectX::XMStoreFloat4x4(&matrix4X4, matrix);
    //全ての面の法線は内側を向くように設定すること

    //左側面
    plane[0].normal.x = matrix4X4._14 + matrix4X4._11;
    plane[0].normal.y = matrix4X4._24 + matrix4X4._21;
    plane[0].normal.z = matrix4X4._34 + matrix4X4._31;
    DirectX::XMStoreFloat3(&plane[0].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[0].normal)));
    plane[0].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[0].normal), DirectX::XMLoadFloat3(&farP[0])));

    //右側面
    plane[1].normal.x = matrix4X4._14 - matrix4X4._11;
    plane[1].normal.y = matrix4X4._24 - matrix4X4._21;
    plane[1].normal.z = matrix4X4._34 - matrix4X4._31;
    DirectX::XMStoreFloat3(&plane[1].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[1].normal)));
    plane[1].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[1].normal), DirectX::XMLoadFloat3(&farP[1])));

    //下側面
    plane[2].normal.x = matrix4X4._14 + matrix4X4._12;
    plane[2].normal.y = matrix4X4._24 + matrix4X4._22;
    plane[2].normal.z = matrix4X4._34 + matrix4X4._32;
    DirectX::XMStoreFloat3(&plane[2].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[2].normal)));
    plane[2].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[2].normal), DirectX::XMLoadFloat3(&farP[1])));

    //上側面
    plane[3].normal.x = matrix4X4._14 - matrix4X4._12;
    plane[3].normal.y = matrix4X4._24 - matrix4X4._22;
    plane[3].normal.z = matrix4X4._34 - matrix4X4._32;
    DirectX::XMStoreFloat3(&plane[3].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[3].normal)));
    plane[3].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[3].normal), DirectX::XMLoadFloat3(&farP[2])));

    //奥側面
    plane[4].normal.x = matrix4X4._14 - matrix4X4._13;
    plane[4].normal.y = matrix4X4._24 - matrix4X4._23;
    plane[4].normal.z = matrix4X4._34 - matrix4X4._33;
    DirectX::XMStoreFloat3(&plane[4].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[4].normal)));
    plane[4].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[4].normal), DirectX::XMLoadFloat3(&farP[2])));

    //手前側面
    plane[5].normal.x = matrix4X4._14 + matrix4X4._13;
    plane[5].normal.y = matrix4X4._24 + matrix4X4._23;
    plane[5].normal.z = matrix4X4._34 + matrix4X4._33;
    DirectX::XMStoreFloat3(&plane[5].normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&plane[5].normal)));
    plane[5].dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&plane[5].normal), DirectX::XMLoadFloat3(&nearP[0])));

    //各境界線outLineNormを面の法線の外積から求めて正規化
    //左下境界線
    DirectX::XMStoreFloat3(&outLineNorm[0], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[0].normal), DirectX::XMLoadFloat3(&plane[2].normal))));
    //右下境界線
    DirectX::XMStoreFloat3(&outLineNorm[1], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[2].normal), DirectX::XMLoadFloat3(&plane[1].normal))));
    //右上境界線
    DirectX::XMStoreFloat3(&outLineNorm[2], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[1].normal), DirectX::XMLoadFloat3(&plane[3].normal))));
    //左上境界線
    DirectX::XMStoreFloat3(&outLineNorm[3], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&plane[3].normal), DirectX::XMLoadFloat3(&plane[0].normal))));
}