#pragma once

#include "CharacterCom.h"

class TestCharacterCom : public CharacterCom
{
public:
    void Start() override;

    void MainAttackDown() override;
    void SubSkill() override;

    void SetGunFireCollision(GameObj obj) { gunFireCollision = obj; }
    GameObj GetGunFireCollision() { return gunFireCollision; }

private:
    bool dashFlag = false;
    bool useWeponFlag = false;

    GameObj gunFireCollision;
};

