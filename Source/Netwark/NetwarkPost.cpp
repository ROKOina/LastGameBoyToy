#include "NetwarkPost.h"

#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Graphics/Graphics.h"

#include "Components/AnimationCom.h"
#include "Components/MovementCom.h"
#include "Components/Character/InazawaCharacterCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "GameSource\Scene\SceneDebugGame.h"
__fastcall NetwarkPost::~NetwarkPost()
{
    // �\�P�b�g�I��
    if (closesocket(multicastSock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    if (closesocket(sock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // WSA�I��
    WSACleanup();

}

void NetwarkPost::RenderUpdate()
{
    for (auto& client : clientDatas)
    {
        if (client.id == id)continue;

        std::string name = "player" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        //������
        if (!clientObj)
        {
            newLoginID.emplace_back(client.id);
            continue;
        }

        //�ʒu�X�V
        clientObj->transform_->SetWorldPosition(client.pos);
        clientObj->transform_->SetRotation(client.rotato);
        clientObj->GetComponent<MovementCom>()->SetVelocity(client.velocity);
        clientObj->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(client.nonVelocity);
    }
}

bool NetwarkPost::IsSynchroFrame(bool isServer)
{
    for (auto& c : clientDatas)
    {
        if (c.id == id)continue;

        if (!isServer)    //�T�[�o�[���͑S�ẴN���C�A���g�Ɣ�r����
            if (c.id != 0)continue;

        if (nowFrame - c.nowFrame > 10)
        {
            return false;
        }

        if (!isServer)
            break;
    }

    return true;
}

