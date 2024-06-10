#pragma once

#include "NetwarkPost.h"


class NetClient : public NetwarkPost
{
public:
    NetClient(std::string ipv4Adress) :ipv4Adress(ipv4Adress) {}
    ~NetClient();

    void __fastcall Initialize() override;

    void __fastcall Update() override;

    void ImGui() override;

private:
    std::string ipv4Adress;
};