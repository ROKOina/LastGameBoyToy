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

    //�U���E���g�擾
    void SetAttackRayObj(std::shared_ptr<GameObject> obj) { attackray = obj; }

private:

    //�q�b�g�X�L�����������������̏���
    void HitObject();

    //�e���ɃG�t�F�N�g��t����
    void SetMuzzleFlash();

private:

    //�q�b�g�X�L�����̃I�u�W�F�N�g
    std::weak_ptr<GameObject> attackray;
};