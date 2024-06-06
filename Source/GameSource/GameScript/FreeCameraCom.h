#pragma once

#include <DirectXMath.h>

#include "Components/System/Component.h"

#include "Components/CameraCom.h"

//�t���[�J����
class FreeCameraCom : public CameraCom
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    FreeCameraCom() {}
    ~FreeCameraCom() {}

    // ���O�擾
    const char* GetName() const override { return "FreeCamera"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    //FreeCamera�N���X
public:


private:
    DirectX::XMFLOAT3 focusPos = { 0,0,0 };
    float distance = 10;

    DirectX::XMFLOAT2	oldCursor;
    DirectX::XMFLOAT2	newCursor;
};