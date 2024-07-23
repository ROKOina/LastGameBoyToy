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

    bufRing.release();
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

        //�_���[�W���X�V
        for (int i = 0; i < SceneDebugGame::MAX_PLAYER_NUM; ++i)
        {
            std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find(("player" + std::to_string(i)).c_str());
            if (player.use_count() == 0) continue;

            player->GetComponent<CharacterCom>()->AddHitPoint(client.damageData[i]);
        }
    }
}

bool NetwarkPost::IsSynchroFrame()
{
    for (auto& c : clientDatas)
    {
        if (c.id == id)continue;

        if (nowFrame - c.nowFrame > 3)
        {
            return false;
        }
    }

    return true;
}

