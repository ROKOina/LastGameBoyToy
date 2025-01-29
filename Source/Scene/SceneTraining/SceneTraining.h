#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"

#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

//トレーニングシーン
class SceneTraining :public Scene
{
public:
    SceneTraining() {};
    ~SceneTraining() override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneTraining"; };

    void PlayerRespawn();

    GameObj audioObj;
private:
    std::unique_ptr<BasicsApplication> photonNet;

    DirectX::XMFLOAT3 SpawnPos = { 0,8.0f,0 };
};
