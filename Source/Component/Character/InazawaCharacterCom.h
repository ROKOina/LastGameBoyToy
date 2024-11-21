#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{
public:
    // 名前取得
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
    //ジャンプダッシュ
    bool isDashJump = false;
    float airTimer = 0.0f;

    //発射間隔
    float shootTime = 0.6f;
    float shootTimer = 0;

    bool attackInputSave = false;   //先行入力
};
