#pragma once
#include "System\Component.h"

class PostureControl : public Component
{
public:
    PostureControl();
    ~PostureControl();

    // ���O�擾
    const char* GetName() const override { return "PostureControl"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;
};
