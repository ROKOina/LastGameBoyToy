#pragma once

#include <ws2tcpip.h>
#include "NetwarkPost.h"


class NetClient : public NetwarkPost
{
public:
    __fastcall NetClient(std::string ipv4Adress, int id) :ipv4Adress(ipv4Adress) { this->id = id; }
    __fastcall ~NetClient() override;

    void __fastcall Initialize() override;

    void __fastcall Update() override;

    void ImGui() override;

private:
    std::string ipv4Adress;
    struct sockaddr_in addr = {0};
};