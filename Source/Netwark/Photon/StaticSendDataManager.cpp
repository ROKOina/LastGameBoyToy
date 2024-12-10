#include "Component\System\GameObject.h"
#include <Component\Character\CharacterCom.h>

#include "StaticSendDataManager.h"  //インクルード一番下に

void StaticSendDataManager::SetSendDamage(int myID, int sendPlayerID, float damage)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetPlayerID() == sendPlayerID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 0;
    data.playerID = sendPlayerID;
    data.valueI = int(damage);
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendHeal(int myID, int sendPlayerID, float heal)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetPlayerID() == sendPlayerID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 1;
    data.playerID = sendPlayerID;
    data.valueI = int(heal);
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendStan(int myID, int sendPlayerID, float stanSec)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetPlayerID() == sendPlayerID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 2;
    data.playerID = sendPlayerID;
    data.valueF = stanSec;
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendKnockback(int myID, int sendPlayerID, DirectX::XMFLOAT3 knockbackVec)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetPlayerID() == sendPlayerID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 3;
    data.playerID = sendPlayerID;
    data.valueF3 = knockbackVec;
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendMovePos(int myID, int sendPlayerID, DirectX::XMFLOAT3 movePos)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetPlayerID() == sendPlayerID)return;   //自分に送られている場合はreturn

    NetSendData data;
    data.sendType = 4;
    data.playerID = sendPlayerID;
    data.valueF3 = movePos;
    sendGameData->Enqueue(data);
}