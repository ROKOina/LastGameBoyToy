#pragma once
#include <windows.h>
#include <DirectXMath.h>

//�J�����R���g���[���[
class CameraController
{
public:
    CameraController();

    //�X�V����
    void Update();

private:
    DirectX::XMFLOAT3  targetFocus = { 0,0,0 };
    DirectX::XMFLOAT3  targetEye = { 0,0,0 };
    DirectX::XMFLOAT3  currentFocus = { 0,0,0 };
    DirectX::XMFLOAT3  currentEye = { 0,0,0 };
    DirectX::XMFLOAT3  angle = { 0,0,0 };
    float              distance = 10;
    float              ROLL_SPEED = DirectX::XMConvertToRadians(90);
    float              range = 10.0f;
    float              MAX_ANGLE_X = DirectX::XMConvertToRadians(45);
    float              MIN_ANGLE_X = DirectX::XMConvertToRadians(-45);
};
