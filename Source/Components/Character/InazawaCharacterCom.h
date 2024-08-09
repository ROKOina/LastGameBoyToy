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

    void SubSkill() override;

    void SpaceSkill() override;


    bool GetUseSkillE() { return useSkillE; }

private:
    //ジャンプダッシュ
    bool isDashJump = false;
    float airTimer = 0.0f;

    //Eスキル
    bool useSkillE = false;
    float eCoolTime = 8.0f;
    float eCoolTimer = 0;

};

