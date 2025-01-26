#pragma once
#include "Scene\Scene.h"
#include "Component\System\GameObject.h"
#include "DeliveryResultData.h"

class SceneResult : public Scene
{
public:

    SceneResult() {};
    ~SceneResult()override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneResult"; };

private:

    //UIやモデルを生成する関数
    void MakeResultUI(GameObj canvas);
    void MakeResultModel();

    //イベントカメラの更新更新処理
    void EventCamera(float elapsedTime);

private:

    enum Chara
    {
        INAZO = 0,
        FARAHC = 1,
        SANTORAT = 2,
        MATHYA = 3
    };

    DelivertResultData::ResultData resultDatas[4];
    bool isMyWin = false;
    std::weak_ptr<GameObject> resultUI[4];
    std::weak_ptr<GameObject> resultModel[2];

    float timer = 1.0f;
    float limittimer = 0.0f;
};