#pragma once
#include "Component/System/Component.h"


class RespawnCom :public Component
{
public:
    RespawnCom() {};
    ~RespawnCom() {};

    //�����ݒ�
    void Start()override {};
    //�X�V����
    void Update(float elapsedTime)override {};
    //IMGUI
    void OnGUI()override {};
    //���O�ݒ�
    const char* GetName() const override { return "Respawn"; }

    void Respawn();

private:
    DirectX::XMFLOAT3 respawnPos = {};
    bool respawnFrame = false;
};