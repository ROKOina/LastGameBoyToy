#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"

//上野シーン
class SceneUeno :public Scene
{
public:
    SceneUeno() {};
    ~SceneUeno()override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneUeno"; };

private:

    //オブジェクト生成関数
    void NewObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;
};
