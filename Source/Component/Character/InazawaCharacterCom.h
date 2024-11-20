#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{
public:
    // ���O�擾
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override;

    void MainAttackDown() override;

    void SubAttackDown() override;
    void SubSkill() override;

    void SpaceSkill() override;

    void UltSkill() override;

    void ResetShootTimer() { shootTimer = 0; }

private:
    void FPSArmAnimation();

private:
    //�W�����v�_�b�V��
    bool isDashJump = false;
    float airTimer = 0.0f;

    //���ˊԊu
    float shootTime = 0.6f;
    float shootTimer = 0;

    bool attackInputSave = false;   //��s����
};
