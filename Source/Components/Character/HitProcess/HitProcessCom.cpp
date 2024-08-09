#include "HitProcessCom.h"

#include "Components\ColliderCom.h"
#include "Components\Character\CharacterCom.h"

#include "Netwark/Photon/StaticSendDataManager.h"

void HitProcessCom::Update(float elapsedTime)
{
    //ƒqƒbƒgŠÔŠu
    hitIntervalTimer += elapsedTime;
    if (hitIntervalTimer < hitIntervalTime)return;

    //ƒqƒbƒg‚ð‘—M
    std::shared_ptr<Collider> col = GetGameObject()->GetComponent<Collider>();
    std::shared_ptr<CharacterCom> parentChara = GetGameObject()->GetParent()->GetComponent<CharacterCom>();
    if (!col)return;
    if (!parentChara)return;

    for (auto& hit : col->OnHitGameObject())
    {
        auto& hitChara = hit.gameObject.lock()->GetComponent<CharacterCom>();
        if (!hitChara)continue;

        //ƒqƒbƒgˆ—
        HitProcess(parentChara->GetNetID(), hitChara->GetNetID());

        hitIntervalTimer = 0;
    }
}

void HitProcessCom::OnGUI()
{
    ImGui::DragFloat("hitIntervalTime", &hitIntervalTime, 0.1f, 0, 5);
    float hitT = hitIntervalTimer;
    ImGui::DragFloat("hitIntervalTimer", &hitT);

    ImGui::DragInt("value", &value, 1, 0, 100);
}

void HitProcessCom::HitProcess(int myID, int hitID)
{
    switch (hitType)
    {
    case HitProcessCom::HIT_TYPE::DAMAGE:
        StaticSendDataManager::Instance().SetSendDamage(myID, hitID, value);
        break;
    case HitProcessCom::HIT_TYPE::HEAL:
        StaticSendDataManager::Instance().SetSendHead(myID, hitID, value);
        break;
    default:
        break;
    }
}
