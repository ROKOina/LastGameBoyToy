#pragma once
#include "../System\Component.h"
#include ".//"

class AudioCom : public Component
{
public:
    AudioCom() {};
    ~AudioCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Audio"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:
};
