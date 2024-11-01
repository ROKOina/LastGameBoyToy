#include "FreeCameraCom.h"

#include "Components/TransformCom.h"
#include "SystemStruct/QuaternionStruct.h"
#include "Graphics/Graphics.h"
#include <windows.h>
#include "Input/Input.h"

//�R���X�g���N�^
FreeCameraCom::FreeCameraCom() : CameraCom::CameraCom(45, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    focuslapelate = 1.0f;
    eyelaperate = 0.0f;
}

// �J�n����
void FreeCameraCom::Start()
{
    DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();
    DirectX::XMFLOAT3 eye;
    eye.x = focusPos.x - front.x * distance;
    eye.y = focusPos.y - front.y * distance;
    eye.z = focusPos.z - front.z * distance;
    GetGameObject()->transform_->SetWorldPosition(eye);

    //���N���X�̃J�������Ă�
    CameraCom::Start();

    //ActiveCameraChange();
}

// �X�V����
void FreeCameraCom::Update(float elapsedTime)
{
    if (isActiveCamera)
    {
        Mouse& mouse = Input::Instance().GetMouse();
        POINT cursor;
        ::GetCursorPos(&cursor);

        DirectX::XMFLOAT2 newCursor(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
        float moveX = (newCursor.x - oldCursor.x) * 0.02f;
        float moveY = (newCursor.y - oldCursor.y) * 0.02f;

        oldCursor = newCursor;

        DirectX::XMFLOAT3 right = GetRight();
        DirectX::XMFLOAT3 up = GetUp();

        if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            // ��]����
            DirectX::XMFLOAT3 euler = GetGameObject()->transform_->GetEulerRotation();
            euler.y += moveX * 8.0f;
            euler.x += moveY * 8.0f;
            GetGameObject()->transform_->SetEulerRotation(euler);
            GetGameObject()->transform_->UpdateTransform();
        }
        else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
        {
            // ���s�ړ�
            float x = moveX * 2;
            float y = moveY * 2;
            focusPos.x -= right.x * x;
            focusPos.y -= right.y * x;
            focusPos.z -= right.z * x;
            focusPos.x += up.x * y;
            focusPos.y += up.y * y;
            focusPos.z += up.z * y;
        }

        if (mouse.GetWheel() != 0)
        {
            ImGuiIO& io = ImGui::GetIO();

            // ImGui��Ƀ}�E�X�J�[�\��������ꍇ�͏������Ȃ�
            if (io.WantCaptureMouse) return;

            // �Y�[��
            distance -= static_cast<float>(mouse.GetWheel()) * distance * 0.001f;
        }

        DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();
        DirectX::XMFLOAT3 eye;
        eye.x = focusPos.x - front.x * distance;
        eye.y = focusPos.y - front.y * distance;
        eye.z = focusPos.z - front.z * distance;
        GetGameObject()->transform_->SetWorldPosition(eye);
    }

    // ���N���X�̃J�������Ă�
    CameraCom::Update(elapsedTime);
}

// GUI�`��
void FreeCameraCom::OnGUI()
{
    //���N���X�̃J�������Ă�
    CameraCom::OnGUI();
}