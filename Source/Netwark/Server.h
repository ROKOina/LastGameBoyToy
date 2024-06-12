#pragma once

#include <ws2tcpip.h>
#include "NetwarkPost.h"

class NetServer : public NetwarkPost
{
public:
    __fastcall NetServer() {}
    __fastcall ~NetServer();

    void __fastcall Initialize() override;

    void __fastcall Update() override;

    void ImGui() override;

private:
    struct sockaddr_in multicastAddr;
};