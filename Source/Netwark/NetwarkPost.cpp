#include "NetwarkPost.h"

#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Graphics/Graphics.h"

#include "Component/Animation/AnimationCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/Character/InazawaCharacterCom.h"
#include "Scene\SceneGame\SceneGame.h"

__fastcall NetwarkPost::~NetwarkPost()
{
    // ソケット終了
    if (closesocket(multicastSock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    if (closesocket(sock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // WSA終了
    WSACleanup();
}

void NetwarkPost::RenderUpdate()
{
    //for (auto& client : clientDatas)
    //{
    //    if (client.id == id)continue;

    //    std::string name = "player" + std::to_string(client.id);
    //    std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

    //    //初期化
    //    if (!clientObj)
    //    {
    //        newLoginID.emplace_back(client.id);
    //        continue;
    //    }

    //    //位置更新
    //    clientObj->transform_->SetWorldPosition(client.pos);
    //    clientObj->transform_->SetRotation(client.rotato);
    //    clientObj->GetComponent<MovementCom>()->SetVelocity(client.velocity);
    //    clientObj->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(client.nonVelocity);
    //}
}

bool NetwarkPost::IsSynchroFrame(bool isServer)
{
    //for (auto& c : clientDatas)
    //{
    //    if (c.id == id)continue;

    //    if (!isServer)    //サーバー側は全てのクライアントと比較する
    //        if (c.id != 0)continue;

    //    if (nowFrame - c.nowFrame > 10)
    //    {
    //        return false;
    //    }

    //    if (!isServer)
    //        break;
    //}

    return true;
}