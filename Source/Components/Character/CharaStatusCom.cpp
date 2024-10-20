#include "CharaStatusCom.h"

// 更新処理
void CharaStatusCom::Update(float elapsedTime)
{
    // 無敵時間の減少処理
    if (currentInvincibleTime > 0.0f)
    {
        currentInvincibleTime -= elapsedTime;
        if (currentInvincibleTime < 0.0f)
        {
            currentInvincibleTime = 0.0f; // 無敵時間が0を下回らないようにする
        }
    }
}

// HPの減少処理（無敵時間を考慮）
void CharaStatusCom::AddDamagePoint(float value)
{
    if (!IsInvincible())
    {
        hitPoint += value;

        // ダメージを受けたら無敵時間をリセット
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