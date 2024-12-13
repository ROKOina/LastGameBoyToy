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

    //初期設定
    void Start()override {};
    //更新処理
    void Update(float elapsedTime)override ;
    //IMGUI
    void OnGUI()override {};
    //名前設定
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