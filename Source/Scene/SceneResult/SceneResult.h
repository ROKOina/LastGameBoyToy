#pragma once
#include "Scene\Scene.h"

class SceneResult : public Scene
{
public:
    SceneResult() {};
    ~SceneResult()override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override {};

    // 更新処理
    void Update(float elapsedTime)override {};

    // 描画処理
    void Render(float elapsedTime)override {};

    //名前取得
    std::string GetName() const override { return "SceneResult"; };

public:
    struct ResultData
    {
        int killNum = 0;
        int deathNum = 0;

        //必要な分足していく
    };
};
