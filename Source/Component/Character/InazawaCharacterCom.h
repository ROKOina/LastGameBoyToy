#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{
public:
    // –¼‘Oæ“¾
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override;

    void MainAttackDown() override;

    void SubAttackDown() override;
    void SubSkill() override;

    void UltSkill() override;

    void ResetShootTimer() { shootTimer = 0; }

private:
    void FPSArmAnimation();

private:
    //”­ËŠÔŠu
    float shootTime = 0.6f;
    float shootTimer = 0;

    bool attackInputSave = false;   //æs“ü—Í
};
