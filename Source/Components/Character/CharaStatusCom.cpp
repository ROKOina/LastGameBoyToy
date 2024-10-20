#include "CharaStatusCom.h"

// �X�V����
void CharaStatusCom::Update(float elapsedTime)
{
    //���G���ԍX�V
    UpdateInvincibleTime(elapsedTime);
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragInt("HP", &hitPoint);
    ImGui::DragFloat("INVISIBLETIMER", &invincibletimer);
    ImGui::Checkbox("hit", &hit);
}

//���G���ԍX�V
void CharaStatusCom::UpdateInvincibleTime(float elapsedTime)
{
    if (invincibletimer > 0.0f)
    {
        invincibletimer -= elapsedTime;
    }
}

//�_���[�W�Ɩ��G���Ԃ̍X�V
void CharaStatusCom::AddDamageAndInvincibleTime(int damage)
{
    // �_���[�W�O��HP��ێ�
    int previousHitPoint = hitPoint;

    ////���N��ԕύX�Ȃ�
    //if (damage == 0)
    //{
    //    hit = true;
    //}

    ////���S���Ă鎞���ς��Ȃ�
    //if (damage <= 0)
    //{
    //    hit = true;
    //}

    ////���G���Ԓ����ς��Ȃ�
    //if (invincibletimer > 0.0f)
    //{
    //    hit = true;
    //}

    // �_���[�W�K�p
    //if (hit)
    {
        hitPoint += damage;
    }

    // HP���ϓ������ꍇ�Ahit�t���O��true�ɐݒ�
    if (hitPoint != previousHitPoint)
    {
        hit = true;
        //invincibletimer = 1.0f; // ���G���Ԃ����Z�b�g
    }
}