#include "Component\System\GameObject.h"
#include <Component\Character\CharacterCom.h>

#include "StaticSendDataManager.h"  //�C���N���[�h��ԉ���

void StaticSendDataManager::SetSendDamage(int myID, int sendID, float damage)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetID() == sendID)return;   //�����ɑ����Ă���ꍇ��return

    NetSendData data;
    data.sendType = 0;
    data.id = sendID;
    data.valueI = int(damage);
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendHeal(int myID, int sendID, float heal)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetID() == sendID)return;   //�����ɑ����Ă���ꍇ��return

    NetSendData data;
    data.sendType = 1;
    data.id = sendID;
    data.valueI = int(heal);
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendStan(int myID, int sendID, float stanSec)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetID() == sendID)return;   //�����ɑ����Ă���ꍇ��return

    NetSendData data;
    data.sendType = 2;
    data.id = sendID;
    data.valueF = stanSec;
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendKnockback(int myID, int sendID, DirectX::XMFLOAT3 knockbackVec)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetID() == sendID)return;   //�����ɑ����Ă���ꍇ��return

    NetSendData data;
    data.sendType = 3;
    data.id = sendID;
    data.valueF3 = knockbackVec;
    sendGameData->Enqueue(data);
}

void StaticSendDataManager::SetSendMovePos(int myID, int sendID, DirectX::XMFLOAT3 movePos)
{
    auto& player = GameObjectManager::Instance().Find("player");
    auto& chara = player->GetComponent<CharacterCom>();

    if (chara->GetNetCharaData().GetNetID() == sendID)return;   //�����ɑ����Ă���ꍇ��return

    NetSendData data;
    data.sendType = 4;
    data.id = sendID;
    data.valueF3 = movePos;
    sendGameData->Enqueue(data);
}