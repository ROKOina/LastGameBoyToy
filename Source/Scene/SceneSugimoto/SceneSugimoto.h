#pragma once

#include "../Scene.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Graphics/Light/Light.h"

#include "Netwark/Photon/BasicsApplication.h"

// ゲームシーン
class SceneSugimoto :public Scene
{
public:
    SceneSugimoto() {}
    ~SceneSugimoto()override;

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneSugimoto"; };

private:
    // 各プレイヤーの入力情報を、それぞれのキャラクターに送る
    void SetUserInputs();

    // プレイヤー( PCの持ち主 )の入力情報
    void SetPlayerInput();

    // 他のプレイヤー( オンライン )の入力情報
    void SetOnlineInput();

    //ディレイ方式用入力
    void DelayOnlineInput();

    //UIのゲームオブジェクト生成
    void CreateUiObject();

    //PVEゲームからリザルトに遷移
    void TransitionPVEFromResult();

private:
    Light* mainDirectionalLight = nullptr;

private:

    std::shared_ptr<TransformCom>t;
};