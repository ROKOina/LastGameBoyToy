#pragma once

#include "CharacterCom.h"
#include <algorithm>
#include <vector>
#include <memory>

class FarahCom : public CharacterCom
{
public:

    // �X�̒e�ۏ���\���\����
    struct FarahBullet
    {
        float bombertimer = 0.0f;          // �����^�C�}�[
        bool bomberflag = false;           // �����t���O
        bool played = false;
        float rotation = 0;
        std::shared_ptr<GameObject> obj;   // �e�ۃI�u�W�F�N�g�ւ̃|�C���^
    };

public:
    // ���O�擾
    const char* GetName() const override { return "FarahCom"; }

    // ������
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI
    void OnGUI() override;

    // �E�N���b�N�P����������
    void SubAttackDown() override;

    // �X�y�[�X�X�L��������
    void SpaceSkillPushing(float elapsedTime) override;

    // E�X�L��
    void SubSkill() override;

    // ���C���U��
    void MainAttackDown() override;

    // ULT
    void UltSkill() override;

    //�����[�h�i�e���炷�����͊e���̃L�����ł���
    void Reload()override;

private:

    // �E���g�X�V
    void UltUpdate(float elapsedTime);

    // �e�̑łԊu�ƃ}�Y���t���b�V��
    void ShotSecond();

    // �E���g�N�[���_�E���Ǘ�
    void HandleCooldown(float elapsedTime);

    // �u�[�X�g�t���O�Ǘ�
    void HandleBoostFlag();

    // �W�����v����
    void ApplyJumpForce();

    // �N�[���_�E���ݒ�
    void SetCooldown(float time);

    // �_�b�V���Q�[�W����
    void AddDashGauge(float amount);

    // �E���g���Z�b�g
    void ResetUlt();

    // �n�ʂŔ�������
    void GroundBomber(float elapsedTime);

    // �e�ۍ폜
    void RemoveBullet(const std::shared_ptr<GameObject>& obj);

    // �S�e�폜
    void ClearAllBullets();

public:

    // �Q�[�W�������x�ݒ�
    void SetDashGaugeMins(const float& dashgaugemin_) { dashgaugemin = dashgaugemin_; }

    // �e�ې���
    void AddBullet(const std::shared_ptr<GameObject>& obj);

private:
    float cooldownTimer = 0.0f;  // �N�[���^�C���̎c�莞�ԁi�b�j
    float ulttimer = 0.0f;       // �E���g����
    float dashgaugemin = 4.0f;   // �_�b�V���Q�[�W�̍ŏ��l
    std::vector<FarahBullet> bullets;  // �e�ۃ��X�g
};