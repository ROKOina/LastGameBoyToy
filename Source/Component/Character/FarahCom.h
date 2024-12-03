#pragma once

#include "CharacterCom.h"

class FarahCom : public CharacterCom
{
public:
    // ���O�擾
    const char* GetName() const override { return "FarahCom"; }

    //������
    void Start() override;

    //�X�V����
    void Update(float elapsedTime) override;

    //gui
    void OnGUI() override;

    //�E�N���b�N�P����������
    void SubAttackDown() override;

    //�X�y�[�X�X�L��������
    void SpaceSkillPushing(float elapsedTime) override;

    //E�X�L��
    void SubSkill() override;

    //���C���̍U��
    void MainAttackDown() override;

    //ULT
    void UltSkill()override;

private:

    //fps�p�̘r�A�j���[�V����
    void FPSArmAnimation();
};