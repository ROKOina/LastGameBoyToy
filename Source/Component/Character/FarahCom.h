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

private:

    //�E���g�X�V
    void UltUpdate(float elapsedTime);

    //�e�̑łԊu�ƃ}�[���t���b�V��
    void ShotSecond();

    //�E���g�N�[���_�E��
    void HandleCooldown(float elapsedTime);

    //�u�[�X�g�t���O
    void HandleBoostFlag();

    //�W�����v����
    void ApplyJumpForce();

    //�N�[���_�E���Z�b�g
    void SetCooldown(float time);

    //�_�b�V������
    void AddDashGauge(float amount);

    //�E���g���Z�b�g
    void ResetUlt();

    //�n�ʂɕt���Δ������鏈��
    void GroundBomber();

public:

    //�Q�[�W���鑬�x�Z�b�g
    void SetDashuGaugeMins(const float& dashgaugemin_) { dashgaugemin = dashgaugemin_; }

private:
    float cooldownTimer = 0.0f;  // �N�[���^�C���̎c�莞�ԁi�b�j
    float ulttimer = 0.0f;       //�@�E���g����
    float dashgaugemin = 4.0f;
};