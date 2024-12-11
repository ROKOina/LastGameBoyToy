#pragma once
#include "Component/System/Component.h"


class RespawnCom :public Component
{
public:
    RespawnCom() {};
    ~RespawnCom() {};

    //初期設定
    void Start()override {};
    //更新処理
    void Update(float elapsedTime)override {};
    //IMGUI
    void OnGUI()override {};
    //名前設定
    const char* GetName() const override { return "Respawn"; }

    void Respawn();

private:
    DirectX::XMFLOAT3 respawnPos = {};
    bool respawnFrame = false;
};