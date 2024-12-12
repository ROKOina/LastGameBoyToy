#pragma once
#include "Component/System/Component.h"


class RespawnCom :public Component
{
public:
    enum class GameMode
    {
        PVE,
        DeathMatch,
    };

public:
    RespawnCom() {};
    ~RespawnCom() {};

    //‰Šúİ’è
    void Start()override {};
    //XVˆ—
    void Update(float elapsedTime)override ;
    //IMGUI
    void OnGUI()override {};
    //–¼‘Oİ’è
    const char* GetName() const override { return "Respawn"; }

    std::vector<DirectX::XMFLOAT3>& GetRespawnPoses() { return respawnPoses; }
    void AddRespawnPoses(DirectX::XMFLOAT3 pos) { respawnPoses.emplace_back(pos); }
    GameMode GetGameMode() { return gameMode; }
    void SetGameMode(GameMode mode) { gameMode = mode; }

    bool GetIsRespawn() { return isRespawn; }
    void SetIsRespawn(bool flag) { isRespawn = flag; }

private:
    GameMode gameMode = GameMode::PVE;

    std::vector<DirectX::XMFLOAT3> respawnPoses = {};
    float playerDeathHeight = -50;
    bool isRespawn = false;


};