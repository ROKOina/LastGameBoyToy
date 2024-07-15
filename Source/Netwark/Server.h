#pragma once

#include <ws2tcpip.h>
#include "NetwarkPost.h"

class NetServer : public NetwarkPost
{
public:
    __fastcall NetServer() { id = 0; }
    __fastcall ~NetServer() override;

    void __fastcall Initialize() override;

    void __fastcall Update() override;

    void ImGui() override;

private:
    //óM
    void Receive();
    //‘—M
    void Send();

    struct sockaddr_in multicastAddr;

};