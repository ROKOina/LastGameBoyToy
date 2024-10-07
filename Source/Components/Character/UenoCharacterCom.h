#pragma once
#include "CharacterCom.h"
#include "Components/GPUParticle.h"

class UenoCharacterCom : public CharacterCom
{
public:
    UenoCharacterCom() {};
    ~UenoCharacterCom() {}

    // ���O�擾
    const char* GetName() const override { return "UenoCharacter"; }

    //������
    void Start() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //imgui
    void OnGUI() override;

    //���N���b�N
    void MainAttackPushing() override;
};