#pragma once

#include <string>
#include "NetData.h"

class NetwarkPost
{
public:
    __fastcall NetwarkPost() {}
    virtual __fastcall ~NetwarkPost();

    virtual void __fastcall Initialize() = 0;

    virtual void __fastcall Update() = 0;

    virtual void ImGui() = 0;

    virtual void RenderUpdate();

public:
     std::vector<NetData>& GetNetDatas() { return clientDatas; }
    const int GetNetId() { return id; }

protected:
    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData = "ABC";
    std::vector<NetData> clientDatas;
    int id;


    unsigned int input;
    unsigned int inputUp;
    unsigned int inputDown;
};