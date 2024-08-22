#pragma once
#include "CharacterCom.h"

class InazawaCharacterCom : public CharacterCom
{    
public:
    // ���O�擾
    const char* GetName() const override { return "InazawaCharacter"; }

    void Start() override;

    void Update(float elapsedTime) override;

    void OnGUI() override;

    void MainAttackDown() override;

    void SubSkill() override;

    void SpaceSkill() override;


    bool GetUseSkillE() { return useSkillE; }

private:
    //�W�����v�_�b�V��
    bool isDashJump = false;
    float airTimer = 0.0f;

    //E�X�L��
    bool useSkillE = false;
    float eCoolTime = 8.0f;
    float eCoolTimer = 0;

};

