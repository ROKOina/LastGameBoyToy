#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{    
public:
    // –¼‘OŽæ“¾
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void MainAttack() override;

    void MainSkill() override;
    void SubSkill() override;

};

