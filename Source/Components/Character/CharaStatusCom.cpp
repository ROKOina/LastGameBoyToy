#include "CharaStatusCom.h"

// �X�V����
void CharaStatusCom::Update(float elapsedTime)
{
    // ���G���Ԃ̌�������
    if (currentInvincibleTime > 0.0f)
    {
        currentInvincibleTime -= elapsedTime;
        if (currentInvincibleTime < 0.0f)
        {
            currentInvincibleTime = 0.0f; // ���G���Ԃ�0�������Ȃ��悤�ɂ���
        }
    }
}

// HP�̌��������i���G���Ԃ��l���j
void CharaStatusCom::AddDamagePoint(float value)
{
    if (!IsInvincible())
    {
        hitPoint += value;

        // �_���[�W���󂯂��疳�G���Ԃ����Z�b�g
        currentInvincibleTime = invincibleTime;
    }
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragInt("HP", &hitPoint);
    ImGui::DragFloat("Invincible Time", &invincibleTime, 0.1f, 0.0f, 10.0f);
    ImGui::Text("Current Invincible Time: %.2f", currentInvincibleTime);
}