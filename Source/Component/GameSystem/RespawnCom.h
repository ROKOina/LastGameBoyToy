#pragma once
#include "Component/System/Component.h"


class RespawnCom :public Component
{
public:
    RespawnCom() {};
    ~RespawnCom() {};

    //‰Šúİ’è
    void Start()override {};
    //XVˆ—
    void Update(float elapsedTime)override {};
    //IMGUI
    void OnGUI()override {};
    //–¼‘Oİ’è
    const char* GetName() const override { return "Respawn"; }

    void Respawn();

private:
    DirectX::XMFLOAT3 respawnPos = {};
    bool respawnFrame = false;
};