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

    void SubAttackDown() override;
    void SubSkill() override;

    void UltSkill() override;

    void ResetShootTimer() { shootTimer = 0; }

    int* GetRCounter() { return &attackUltCounter; }
    int GetRMaxCount() { return attackUltCountMax; }

    //�U���E���g�擾
    void SetAttackUltRayObj(std::shared_ptr<GameObject> obj) { attackUltRayObj = obj; }

private:
    //���ˊԊu
    float shootTime = 0.6f;
    float shootTimer = 0;

    bool attackInputSave = false;   //��s����

    //�A�^�b�NULT
    int attackUltCountMax = 5;  //�E���g��łĂ鐔
    int attackUltCounter;
    std::weak_ptr<GameObject> attackUltRayObj;  //�E���g���C
};
