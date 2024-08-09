#include "Components\System\GameObject.h"
#include "Components\Character\TestCharacterCom.h"

#include "StaticSendDataManager.h"  //インクルード一番下に

void StaticSendDataManager::SetSendDamage(int myID, int sendID, int damage)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetID() == myID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.id = sendID;
    data.damage = damage;
    sendData->Enqueue(data);
}
