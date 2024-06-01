#include "CameraController.h"
#include "Input/Input.h"
#include "Camera.h"
#include <imgui.h>

//�R���X�g���N�^
CameraController::CameraController()
{
    //�����ݒ�
    angle.x = 0.280f;
    angle.y = -0.010f;
    targetFocus.x = 0.184f;
    targetFocus.y = 56.708f;
    targetFocus.z = 300.950f;
}

// �X�V����
void CameraController::Update()
{
    //�}�E�X�ƃp�b�h�̏����擾
    Mouse& mouse = Input::Instance().GetMouse();
    GamePad& pad = Input::Instance().GetGamePad();

    //�}�E�X�ړ�����
    float moveX = (mouse.GetPositionX() - mouse.GetOldPositionX()) * 0.02f;
    float moveY = (mouse.GetPositionY() - mouse.GetOldPositionY()) * 0.02f;

    Camera& camera = Camera::Instance();

    // �����s��𐶐�
    DirectX::XMMATRIX V;
    {
        DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
        // �}�E�X����
        {
            // �E�N���b�N
            if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            {
                // ��]����
                {
                    // Y����]
                    angle.y += moveX * 0.5f;
                    if (angle.y > DirectX::XM_PI)
                        angle.y -= DirectX::XM_2PI;
                    else if (angle.y < -DirectX::XM_PI)
                        angle.y += DirectX::XM_2PI;

                    // X����]
                    angle.x += moveY * 0.5f;
                    if (angle.x > DirectX::XMConvertToRadians(89.9f))
                        angle.x = DirectX::XMConvertToRadians(89.9f);
                    else if (angle.x < -DirectX::XMConvertToRadians(89.9f))
                        angle.x = -DirectX::XMConvertToRadians(89.9f);
                }

                // �ړ�����
                {
                    V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&targetEye),
                        DirectX::XMLoadFloat3(&targetFocus),
                        up);
                    DirectX::XMFLOAT4X4 W;
                    DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
                    // ���s�ړ�
                    float s = distance * 0.035f;
                    // �V�t�g�ō����ړ�
                    if (GetAsyncKeyState(VK_SHIFT))s = 1;
                    float x = -pad.GetAxisLX() * s;
                    float y = pad.GetAxisLY() * s;
                    // ������
                    targetFocus.x -= W._11 * x;
                    targetFocus.y -= W._12 * x;
                    targetFocus.z -= W._13 * x;
                    // �O����
                    targetFocus.x += W._31 * y;
                    targetFocus.y += W._32 * y;
                    targetFocus.z += W._33 * y;
                }
            }
            // �z�C�[���N���b�N
            else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&targetEye),
                    DirectX::XMLoadFloat3(&targetFocus),
                    up);
                DirectX::XMFLOAT4X4 W;
                DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
                // ���s�ړ�
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

        // �J�����Ɏ��_�𒍎��_��ݒ�
        Camera::Instance().SetLookAt(targetEye, targetFocus, { 0, 1, 0 });
    }

    if (mouse.GetWheel() != 0)
    {
        // �Y�[��
        distance -= static_cast<float>(mouse.GetWheel()) * distance * 0.001f;
    }
}