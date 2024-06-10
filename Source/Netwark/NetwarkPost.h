#pragma once

#include <string>

class NetwarkPost
{
public:
    __fastcall NetwarkPost() {}
    __fastcall ~NetwarkPost();

    virtual void __fastcall Initialize() = 0;

    virtual void __fastcall Update() = 0;

    virtual void ImGui() = 0;

protected:
    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData = "ABC";
};