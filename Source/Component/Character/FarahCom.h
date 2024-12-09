#pragma once

#include "CharacterCom.h"
#include <algorithm>

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

    //fps�p�̘r�A�j���[�V����
    void FPSArmAnimation();

public:

    //�E���g�X�V
    void UltUpdate(float elapsedTime);

public:

    //�Q�[�W���鑬�x�Z�b�g
    void SetDashuGaugeMins(const float& dashgaugemin_) { dashgaugemin = dashgaugemin_; }

private:
    float cooldownTimer = 0.0f;  // �N�[���^�C���̎c�莞�ԁi�b�j
    float dashgaugemin = 4.0f;
    float ulttimer = 0.0f;
};