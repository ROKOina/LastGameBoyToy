#pragma once

//#include <winsock2.h>
#include <string>

class NetServer
{
public:
    __fastcall NetServer() {}
    __fastcall ~NetServer();

    void __fastcall Initialize();

    void __fastcall Update();

    void ImGui();

private:
    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData="ABC";
};