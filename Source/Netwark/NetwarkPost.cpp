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

    bufRing.release();
}

void NetwarkPost::RenderUpdate()
{
    for (auto& client : clientDatas)
    {
        if (client.id == id)continue;

        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        //初期化
        if (!clientObj)
        {
            clientObj = GameObjectManager::Instance().Create();
            clientObj->SetName(name.c_str());
            clientObj->transform_->SetWorldPosition({ 0, 0, 0 });
            clientObj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
            std::shared_ptr<RendererCom> r = clientObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
            r->LoadModel("Data/OneCoin/robot.mdl");
            std::shared_ptr<AnimationCom> a = clientObj->AddComponent<AnimationCom>();
            std::shared_ptr<MovementCom> m = clientObj->AddComponent<MovementCom>();
            //std::shared_ptr<InazawaCharacterCom> c = clientObj->AddComponent<InazawaCharacterCom>();
        }

        //位置更新
        clientObj->transform_->SetWorldPosition(client.pos);
        clientObj->transform_->SetRotation(client.rotato);
        clientObj->GetComponent<MovementCom>()->SetVelocity(client.velocity);
        clientObj->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(client.nonVelocity);
    }
}

