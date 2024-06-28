#pragma once
#include "CharacterCom.h"

class NomuraCharacterCom :public CharacterCom
{
public:
    //���O�擾
    const char* GetName()const override { return "NomuraCharacter"; }

    void Start() override;

    void Update(float elapsedTime)override;

    void OnGUI() override;

    void MainAttack()override;

    void SubSkill()override;

    //void SpaceSkill()override;


private:

    //�_�b�V��
    bool dashFlag = false;

};