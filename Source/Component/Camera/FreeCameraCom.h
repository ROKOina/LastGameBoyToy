#pragma once

#include <DirectXMath.h>

#include "CameraCom.h"

//�t���[�J����
class FreeCameraCom : public CameraCom
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    FreeCameraCom();
    ~FreeCameraCom() {}

    // ���O�擾
    const char* GetName() const override { return "FreeCamera"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    void SetFocusPos(DirectX::XMFLOAT3 pos) { focusPos = pos; }

    void SetDistance(float dis) { distance = dis; }

    //FreeCamera�N���X
public:

private:
    DirectX::XMFLOAT3 focusPos = { 0,0,0 };
    float distance = 10;

    DirectX::XMFLOAT2	oldCursor = {};
    DirectX::XMFLOAT2	newCursor = {};
};