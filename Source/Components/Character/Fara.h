#pragma once

#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class Fara : public CharacterCom
{
public:
    Fara() {};
    ~Fara() {}

    // ���O�擾
    const char* GetName() const override { return "Fara"; }

    //������
    void Start() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;
};