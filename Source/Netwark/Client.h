#pragma once

//#include <winsock2.h>
#include <thread>
//#include <memory>


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
    std::unique_ptr<std::thread> thread;

    std::string sendData;
    bool isSend = false;
    std::string recvData="ABC";
};