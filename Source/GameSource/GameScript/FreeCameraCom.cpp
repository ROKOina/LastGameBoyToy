#include "FreeCameraCom.h"

#include "Components/TransformCom.h"
#include "SystemStruct/QuaternionStruct.h"
#include "Graphics/Graphics.h"
#include <windows.h>
#include "Input/Input.h"

//コンストラクタ
FreeCameraCom::FreeCameraCom() : CameraCom::CameraCom(45, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    focuslapelate = 1.0f;
    eyelaperate = 0.0f;
}

// 開始処理
void FreeCameraCom::Start()
{
    DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();
    DirectX::XMFLOAT3 eye;
    eye.x = focusPos.x - front.x * distance;
    eye.y = focusPos.y - front.y * distance;
    eye.z = focusPos.z - front.z * distance;
    GetGameObject()->transform_->SetWorldPosition(eye);

    //基底クラスのカメラを呼ぶ
    CameraCom::Start();

    isActiveCamera = true;
}

// 更新処理
void FreeCameraCom::Update(float elapsedTime)
{
    if (isActiveCamera)
    {
        POINT cursor;
        ::GetCursorPos(&cursor);

        oldCursor = newCursor;
        newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

        float moveX = (newCursor.x - oldCursor.x) * 0.02f;
        float moveY = (newCursor.y - oldCursor.y) * 0.02f;

        // Altキー
        if (::GetAsyncKeyState(VK_MENU) & 0x8000)
        {
            //回転
            if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
            {
                DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();

                euler.y += moveX * 8.0f;
                euler.x += moveY * 8.0f;
                GetGameObject()->transform_->SetEulerRotation(euler);

                GetGameObject()->transform_->UpdateTransform();
            }
            else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                // 平行移動
                DirectX::XMFLOAT3 right = GetRight();
                DirectX::XMFLOAT3 up = GetUp();

                float x = moveX * 2;
                float y = moveY * 2;

                focusPos.x -= right.x * x;
                focusPos.y -= right.y * x;
                focusPos.z -= right.z * x;

                focusPos.x += up.x * y;
                focusPos.y += up.y * y;
                focusPos.z += up.z * y;
            }
            //else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            //{

            //    // ズーム
            //    distance += (-moveY - moveX) * distance * 0.1f;
            //    if (distance < 0.1f)
            //        distance = 0.1f;
            //}
            if (Input::Instance().GetMouse().GetWheel()!= 0)
            {
                // ズーム
                distance -= static_cast<float>(Input::Instance().GetMouse().GetWheel()) * distance * 0.001f;
            }
            DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();

            DirectX::XMFLOAT3 eye;
            eye.x = focusPos.x - front.x * distance;
            eye.y = focusPos.y - front.y * distance;
            eye.z = focusPos.z - front.z * distance;

            GetGameObject()->transform_->SetWorldPosition(eye);
        }
    }

    //基底クラスのカメラを呼ぶ
    CameraCom::Update(elapsedTime);
}

// GUI描画
void FreeCameraCom::OnGUI()
{
    //基底クラスのカメラを呼ぶ
    CameraCom::OnGUI();
}