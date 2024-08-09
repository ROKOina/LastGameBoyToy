#pragma once
#include "CharacterCom.h"

class HaveAllAttackCharaCom : public CharacterCom
{
public:
    // ���O�擾
    const char* GetName() const override { return "HaveAllAttackChara"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override;

    void MainAttackPushing() override;

    void SubAttackPushing() override;

    void SubSkill() override;

    void SpaceSkill() override;

    void LeftShiftSkill() override;


private:

};

