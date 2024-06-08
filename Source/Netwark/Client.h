#pragma once

//#include <winsock2.h>
//#include <memory>
#include <string>

class NetClient
{
public:
    NetClient(){}
    ~NetClient();

    void __fastcall Initialize();

    void __fastcall Update();

    void ImGui();

private:
    void __fastcall RecvThread();

    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData="ABC";
};