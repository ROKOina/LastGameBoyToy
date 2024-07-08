#pragma once

#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

//FPS�J����
class FPSCameraCom : public CameraCom
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    FPSCameraCom();
    ~FPSCameraCom() {}

    // ���O�擾
    const char* GetName() const override { return "FPSCamera"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;
};