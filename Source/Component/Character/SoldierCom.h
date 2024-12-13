#pragma once

#include "CharacterCom.h"

class SoldierCom : public CharacterCom
{
public:

    // ���O�擾
    const char* GetName() const override { return "SoldierCom"; }

    // ������
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �E�N���b�N�P����������
    void SubAttackDown() override;

    // E�X�L��
    void SubSkill() override;

    // ���C���U��
    void MainAttackDown() override;

    // ULT
    void UltSkill() override;

    //�����[�h�i�e���炷�����͊e���̃L�����ł���
    void Reload()override;

    // GUI
    void OnGUI() override;
};