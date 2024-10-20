#include "CharaStatusCom.h"

// 更新処理
void CharaStatusCom::Update(float elapsedTime)
{
    //無敵時間更新
    UpdateInvincibleTime(elapsedTime);
}

//GUI
void CharaStatusCom::OnGUI()
{
    ImGui::DragInt("HP", &hitPoint);
    ImGui::DragFloat("INVISIBLETIMER", &invincibletimer);
    ImGui::Checkbox("hit", &hit);
}

//無敵時間更新
void CharaStatusCom::UpdateInvincibleTime(float elapsedTime)
{
    if (invincibletimer > 0.0f)
    {
        invincibletimer -= elapsedTime;
    }
}

//ダメージと無敵時間の更新
void CharaStatusCom::AddDamageAndInvincibleTime(int damage)
{
    // ダメージ前のHPを保持
    int previousHitPoint = hitPoint;

    ////健康状態変更なし
    //if (damage == 0)
    //{
    //    hit = true;
    //}

    ////死亡してる時も変えない
    //if (damage <= 0)
    //{
    //    hit = true;
    //}

    ////無敵時間中も変えない
    //if (invincibletimer > 0.0f)
    //{
    //    hit = true;
    //}

    // ダメージ適用
    //if (hit)
    {
        hitPoint += damage;
    }

    // HPが変動した場合、hitフラグをtrueに設定
    if (hitPoint != previousHitPoint)
    {
        hit = true;
        //invincibletimer = 1.0f; // 無敵時間をリセット
    }
}