#include "CameraController.h"
#include "Input/Input.h"
#include "Camera.h"
#include <imgui.h>

//コンストラクタ
CameraController::CameraController()
{
    //初期設定
    angle.x = 0.280f;
    angle.y = -0.010f;
    targetFocus.x = 0.184f;
    targetFocus.y = 56.708f;
    targetFocus.z = 300.950f;
}

// 更新処理
void CameraController::Update()
{
    //マウスとパッドの情報を取得
    Mouse& mouse = Input::Instance().GetMouse();
    GamePad& pad = Input::Instance().GetGamePad();

    //マウス移動処理
    float moveX = (mouse.GetPositionX() - mouse.GetOldPositionX()) * 0.02f;
    float moveY = (mouse.GetPositionY() - mouse.GetOldPositionY()) * 0.02f;

    Camera& camera = Camera::Instance();

    // 視線行列を生成
    DirectX::XMMATRIX V;
    {
        DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
        // マウス操作
        {
            // 右クリック
            if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            {
                // 回転処理
                {
                    // Y軸回転
                    angle.y += moveX * 0.5f;
                    if (angle.y > DirectX::XM_PI)
                        angle.y -= DirectX::XM_2PI;
                    else if (angle.y < -DirectX::XM_PI)
                        angle.y += DirectX::XM_2PI;

                    // X軸回転
                    angle.x += moveY * 0.5f;
                    if (angle.x > DirectX::XMConvertToRadians(89.9f))
                        angle.x = DirectX::XMConvertToRadians(89.9f);
                    else if (angle.x < -DirectX::XMConvertToRadians(89.9f))
                        angle.x = -DirectX::XMConvertToRadians(89.9f);
                }

                // 移動処理
                {
                    V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&targetEye),
                        DirectX::XMLoadFloat3(&targetFocus),
                        up);
                    DirectX::XMFLOAT4X4 W;
                    DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
                    // 平行移動
                    float s = distance * 0.035f;
                    // シフトで高速移動
                    if (GetAsyncKeyState(VK_SHIFT))s = 1;
                    float x = -pad.GetAxisLX() * s;
                    float y = pad.GetAxisLY() * s;
                    // 横方向
                    targetFocus.x -= W._11 * x;
                    targetFocus.y -= W._12 * x;
                    targetFocus.z -= W._13 * x;
                    // 前方向
                    targetFocus.x += W._31 * y;
                    targetFocus.y += W._32 * y;
                    targetFocus.z += W._33 * y;
                }
            }
            // ホイールクリック
            else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&targetEye),
                    DirectX::XMLoadFloat3(&targetFocus),
                    up);
                DirectX::XMFLOAT4X4 W;
                DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
                // 平行移動
                float s = distance * 0.035f;
                float x = moveX * s;
                float y = moveY * s;
                targetFocus.x -= W._11 * x;
                targetFocus.y -= W._12 * x;
                targetFocus.z -= W._13 * x;

                targetFocus.x += W._21 * y;
                targetFocus.y += W._22 * y;
                targetFocus.z += W._23 * y;
            }
        }
        float sx = ::sinf(angle.x), cx = ::cosf(angle.x);
        float sy = ::sinf(angle.y), cy = ::cosf(angle.y);
        DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&targetFocus);
        DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
        DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
        Front = DirectX::XMVectorMultiply(Front, Distance);
        DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
        DirectX::XMStoreFloat3(&targetEye, Eye);

        // カメラに視点を注視点を設定
        Camera::Instance().SetLookAt(targetEye, targetFocus, { 0, 1, 0 });
    }

    if (mouse.GetWheel() != 0)
    {
        // ズーム
        distance -= static_cast<float>(mouse.GetWheel()) * distance * 0.001f;
    }
}