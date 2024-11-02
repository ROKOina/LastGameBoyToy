#include "HitProcessCom.h"

#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"

#include "Netwark/Photon/StaticSendDataManager.h"

void HitProcessCom::Update(float elapsedTime)
{
    isHit = false;
    isHitNonChara = false;

    //ƒqƒbƒgŠÔŠu
    hitIntervalTimer += elapsedTime;
    if (hitIntervalTimer < hitIntervalTime)return;

    //ƒqƒbƒg‚ð‘—M
    std::shared_ptr<Collider> col = GetGameObject()->GetComponent<Collider>();
    if (!col)return;
    std::shared_ptr<CharacterCom> parentChara = myObj.lock()->GetComponent<CharacterCom>();
    if (!parentChara)return;

    for (auto& hit : col->OnHitGameObject())
    {
        auto& stats = hit.gameObject.lock()->GetComponent<CharaStatusCom>();
        if (!stats)continue;

        auto& hitChara = hit.gameObject.lock()->GetComponent<CharacterCom>();
        if (!hitChara)
        {
            isHitNonChara = true;
            nonCharaObj = hit.gameObject;

            //–³“GŽžŠÔ‚ÌŠÔ‚Íƒqƒbƒg‚ð—^‚¦‚È‚¢
            if(!stats->IsInvincible())
                parentChara->SetIsHitAttack(true);

            //ƒ_ƒ[ƒW‚ð—^‚¦‚é
            stats->AddDamagePoint(-value);

            continue;
        }

        parentChara->SetIsHitAttack(true);

        //ƒqƒbƒgˆ—
        HitProcess(parentChara->GetNetID(), hitChara->GetNetID());

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