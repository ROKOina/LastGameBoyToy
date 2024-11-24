#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"

//リザルトシーン
class ScenePVP :public Scene
{
public:
    ScenePVP() {};
    ~ScenePVP()override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "ScenePVP"; };

    //UIのゲームオブジェクト生成
    void CreateUiObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;
};
