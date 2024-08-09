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
    data.valueI = damage;
    sendData->Enqueue(data);
}

void StaticSendDataManager::SetSendHeal(int myID, int sendID, int heal)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetID() == sendID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 1;
    data.id = sendID;
    data.valueI = heal;
    sendData->Enqueue(data);

}

void StaticSendDataManager::SetSendStan(int myID, int sendID, float stanSec)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetID() == sendID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 2;
    data.id = sendID;
    data.valueF = stanSec;
    sendData->Enqueue(data);
}
