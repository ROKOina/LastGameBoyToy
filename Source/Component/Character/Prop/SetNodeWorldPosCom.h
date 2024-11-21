#pragma once

#include "../../System\Component.h"

class SetNodeWorldPosCom : public Component
{
public:
    SetNodeWorldPosCom() {};
    ~SetNodeWorldPosCom() override {};

    // ���O�擾
    const char* GetName() const override { return "SetNodeWorldPos"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

private:
    std::weak_ptr<GameObject> obj;
};
