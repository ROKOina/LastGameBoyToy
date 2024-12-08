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

    void UltSkill() override;

    void ResetShootTimer() { shootTimer = 0; }

    int* GetRCounter() { return &attackUltCounter; }
    int GetRMaxCount() { return attackUltCountMax; }

    //攻撃ウルト取得
    void SetAttackUltRayObj(std::shared_ptr<GameObject> obj) { attackUltRayObj = obj; }

private:
    //発射間隔
    float shootTime = 0.6f;
    float shootTimer = 0;

    bool attackInputSave = false;   //先行入力

    //アタックULT
    int attackUltCountMax = 5;  //ウルトを打てる数
    int attackUltCounter;
    std::weak_ptr<GameObject> attackUltRayObj;  //ウルトレイ
};
