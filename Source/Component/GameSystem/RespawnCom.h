#pragma once
#include "Component/System/Component.h"
#include "PVPGameSystem/PVPGameSystem.h"

struct RespawnData
{
    GameObject* gameObj;
    float respawnTime = 0;
};

class RespawnCom :public Component
{
public:
    RespawnCom() {};
    ~RespawnCom() {};

    //èâä˙ê›íË
    void Start()override {};
    //çXêVèàóù
    void Update(float elapsedTime)override ;
    //IMGUI
    void OnGUI()override {};
    //ñºëOê›íË
    const char* GetName() const override { return "Respawn"; }

    std::vector<DirectX::XMFLOAT3>& GetRespawnPoses() { return respawnPoses; }
    void AddRespawnPoses(DirectX::XMFLOAT3 pos) { respawnPoses.emplace_back(pos); }
    PVPGameSystem::GAME_MODE GetGameMode() { return gameMode; }
    void SetGameMode(PVPGameSystem::GAME_MODE mode) { gameMode = mode; }

    void AddRespawnData(std::shared_ptr<GameObject> obj) 
    { 
        RespawnData* res = new RespawnData; 
        res->gameObj = obj.get();
        respawnDatas.emplace_back(res);
    }

    bool GetIsRespawn() { return isRespawn; }
    void SetIsRespawn(bool flag) { isRespawn = flag; }

private:
    PVPGameSystem::GAME_MODE gameMode;

    std::vector<DirectX::XMFLOAT3> respawnPoses = {};
    float playerDeathHeight = -50;
    float respawnTimer = 0.0f;
    bool isRespawn = false;
    bool fallEvent = false;//óéâ∫ÇµÇΩÇ©Ç«Ç§Ç©

    std::vector<RespawnData*> respawnDatas;
    std::vector<RespawnData*> endDatas;
};