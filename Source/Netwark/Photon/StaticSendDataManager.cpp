#include "Components\System\GameObject.h"
#include "Components\Character\TestCharacterCom.h"

#include "StaticSendDataManager.h"  //インクルード一番下に

void StaticSendDataManager::SetSendDamage(int myID, int sendID, int damage)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetID() == sendID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 0;
    data.id = sendID;
    data.value = damage;
    sendData->Enqueue(data);
}

void StaticSendDataManager::SetSendHead(int myID, int sendID, int damage)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetID() == sendID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 1;
    data.id = sendID;
    data.value = damage;
    sendData->Enqueue(data);

}
