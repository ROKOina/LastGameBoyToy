#pragma once

#include "NetwarkPost.h"

class NetServer : public NetwarkPost
{
public:
    __fastcall NetServer() {}
    __fastcall ~NetServer();

    void __fastcall Initialize() override;

    void __fastcall Update() override;

    void ImGui() override;
};