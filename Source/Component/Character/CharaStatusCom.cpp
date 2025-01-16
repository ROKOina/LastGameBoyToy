#include "CharaStatusCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"

// 更新処理
void CharaStatusCom::Update(float elapsedTime)
{
    if (hitPoint <= 0)
    {
        isDeath = true;
    }

    frameDamage = 0.0f;

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
void CharaStatusCom::AddDamagePoint(float value, int playerID)
{
    if (!IsInvincible())
    {
        if (hitPoint > 0)
        {
            hitPoint += value;
            frameDamage += value;

            // ダメージを受けたら無敵時間をリセット
            currentInvincibleTime = invincibleTime;

            //死亡時プレイヤーID保存
            if (playerID >= 0)
            {
                if (hitPoint <= 0)
                {
                    //キルした相手を保存
                    StaticSendDataManager::Instance().GetDeathID(playerID) = true;
                }
            }
        }
    }
}

void CharaStatusCom::ReSpawn(int HP)
{
    //復活後のHP
    hitPoint = HP;

    //デスフラグを撤回
    isDeath = false;
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragFloat("HP", &hitPoint);
    ImGui::DragFloat("Invincible Time", &invincibleTime, 0.1f, 0.0f, 10.0f);
    ImGui::Text("Current Invincible Time: %.2f", currentInvincibleTime);
}