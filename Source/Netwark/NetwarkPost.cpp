#include "NetwarkPost.h"

#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Graphics/Graphics.h"

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
        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        //������
        if (!clientObj)
        {
            clientObj = GameObjectManager::Instance().Create();
            clientObj->SetName(name.c_str());
            clientObj->transform_->SetScale(DirectX::XMFLOAT3(0.3f, 0.3f, 0.3f));
            std::shared_ptr<RendererCom> r = clientObj->AddComponent<RendererCom>(SHADERMODE::BLACK, BLENDSTATE::ALPHA);
            r->LoadModel("Data/Ball/t.mdl");
        }

        //�ʒu�X�V
        clientObj->transform_->SetWorldPosition(client.pos);
    }
}

