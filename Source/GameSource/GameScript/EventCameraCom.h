#pragma once

#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

//�C�x���g�J����
class EventCameraCom : public CameraCom
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    EventCameraCom();
    ~EventCameraCom() {}

    // ���O�擾
    const char* GetName() const override { return "EventCamera"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

private:
    std::string focusObjName;
    std::weak_ptr<GameObject> cameraCopy;
};