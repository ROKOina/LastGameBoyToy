#include "CharaStatusCom.h"

// �X�V����
void CharaStatusCom::Update(float elapsedTime)
{
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragInt("HP", &hitPoint);
}