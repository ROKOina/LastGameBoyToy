#include "CharaStatusCom.h"
#include "CharacterCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"

// 更新処理
void CharaStatusCom::Update(float elapsedTime)
{
    if (hitPoint <= 0)
    {
        isDeath = true;
    }

    // isDeathFrame の処理
    if ((isDeath && !wasDeath))
    {
        isDeathFrame = true;
        if (std::strcmp(GetGameObject()->GetName(), "player") == 0)
            StaticSendDataManager::Instance().SendMyDeath();    //ネットにデスを送信
    }

    // 現在の isDeath 状態を記録
    wasDeath = isDeath;

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

    //最後攻撃してきた敵を保存するタイマー
    lastDamageTimer -= elapsedTime;
    if (lastDamageTimer < 0)
    {
        lastDamageID = -1;
    }
}

// HPの減少処理（無敵時間を考慮）
void CharaStatusCom::AddDamagePoint(float value, int playerID)
{
    if (isDeath)return;

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

                int attackID = StaticSendDataManager::Instance().GetTeamNum(playerID);
                int myID = GetGameObject()->GetComponent<CharacterCom>()->GetNetCharaData().GetTeamID();

                if (attackID != myID) { //敵チーム場合は通る
                    //攻撃してきた敵を保存
                    lastDamageID = playerID;
                    lastDamageTimer = 20;
                }
            }
            //死亡時にキルをした相手をネットに送る
            if (hitPoint <= 0)
            {
                if (lastDamageID >= 0)
                {
                    //キルした相手を保存
                    StaticSendDataManager::Instance().GetDeathID(lastDamageID) = true;
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

    //攻撃相手保存をリセット
    lastDamageID = -1;
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragFloat("HP", &hitPoint);
    ImGui::DragFloat("Invincible Time", &invincibleTime, 0.1f, 0.0f, 10.0f);
    ImGui::Text("Current Invincible Time: %.2f", currentInvincibleTime);
    ImGui::Checkbox("isDeathFrame", &isDeathFrame);
    ImGui::InputInt("lastDamageID", &lastDamageID);
    ImGui::InputFloat("lastDamageTimer", &lastDamageTimer);
}