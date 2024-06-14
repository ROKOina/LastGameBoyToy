#pragma once
#include "CharacterCom.h"

class TestCharacterCom : public CharacterCom
{
    void Start() override;

    void MainAttack() override;
    void SubSkill() override;

private:
    bool dashFlag = false;
};

