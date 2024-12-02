#include "HitProcessCom.h"

#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Sprite\Sprite.h"
#include "Netwark/Photon/StaticSendDataManager.h"

void HitProcessCom::Update(float elapsedTime)
{
    isHit = false;
    isHitNonChara = false;

    //ヒット間隔
    hitIntervalTimer += elapsedTime;
    if (hitIntervalTimer < hitIntervalTime)return;

    //ヒットを送信
    std::shared_ptr<Collider> col = GetGameObject()->GetComponent<Collider>();
    if (!col)return;
    std::shared_ptr<CharacterCom> chara = myObj.lock()->GetComponent<CharacterCom>();
    if (!chara)return;

    for (auto& hit : col->OnHitGameObject())
    {
        //ウルトゲージ溜める
        float ultGauge = *chara->GetUltGauge();
        ultGauge += 5;
        chara->SetUltGauge(ultGauge);

        auto& stats = hit.gameObject.lock()->GetComponent<CharaStatusCom>();
        if (!stats)continue;

        //キャラクター以外の場合
        auto& hitChara = hit.gameObject.lock()->GetComponent<CharacterCom>();
        if (!hitChara)
        {
            isHitNonChara = true;
            nonCharaObj = hit.gameObject;

            //無敵時間の間はヒットを与えない
            if (!stats->IsInvincible())
                chara->SetIsHitAttack(true);

            //ダメージを与える
            stats->AddDamagePoint(-value);

            continue;
        }

        chara->SetIsHitAttack(true);

        //ヒット処理
        HitProcess(chara->GetNetID(), hitChara->GetNetID());

        hitIntervalTimer = 0;

        isHit = true;
    }
}

void HitProcessCom::OnGUI()
{
    ImGui::DragFloat("hitIntervalTime", &hitIntervalTime, 0.1f, 0, 5);
    float hitT = hitIntervalTimer;
    ImGui::DragFloat("hitIntervalTimer", &hitT);

    ImGui::DragFloat("value", &value, 0.1f, 0, 100);
}

void HitProcessCom::HitProcess(int myID, int hitID)
{
    switch (hitType)
    {
    case HitProcessCom::HIT_TYPE::DAMAGE:
        StaticSendDataManager::Instance().SetSendDamage(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::HEAL:
        StaticSendDataManager::Instance().SetSendHeal(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::STAN:
        StaticSendDataManager::Instance().SetSendStan(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::KNOCKBACK:
        StaticSendDataManager::Instance().SetSendKnockback(myID, hitID, value3);
        break;
    default:
        break;
    }
}