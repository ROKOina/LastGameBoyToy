#pragma once
#include "Scene\Scene.h"
#include "Component\System\GameObject.h"

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
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneResult"; };

private:
    void MakeResultUI(GameObj canvas);

public:
    struct ResultData
    {
        int killNum = 0;
        int deathNum = 0;

        //必要な分足していく
        int charaID = -1;
        int playerID = -1;

        bool isWin = false;

        std::string playerName;
    };
    ResultData resultDatas[4];
    GameObj resultUI[4];
};
