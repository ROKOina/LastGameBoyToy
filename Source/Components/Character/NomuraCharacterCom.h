#pragma once
#include "CharacterCom.h"

class NomuraCharacterCom :public CharacterCom
{
public:
    //名前取得
    const char* GetName()const override { return "NomuraCharacter"; }

    void Start() override;

    void Update(float elapsedTime)override;

    void OnGUI() override;

    void MainAttack()override;

    void SubSkill()override;

    //void SpaceSkill()override;


private:

    //ダッシュ
    bool dashFlag = false;

};