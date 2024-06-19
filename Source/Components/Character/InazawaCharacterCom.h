#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{    
public:
    // ���O�擾
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void MainAttack() override;

    void MainSkill() override;
    void SubSkill() override;

};

