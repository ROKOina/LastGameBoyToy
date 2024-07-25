#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Netwark/Photon/BasicsApplication.h"


// ゲームシーン
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

private:
    // 各プレイヤーの入力情報を、それぞれのキャラクターに送る
    void SetUserInputs();

    // プレイヤー( PCの持ち主 )の入力情報
    void SetPlayerInput();

    // 他のプレイヤー( オンライン )の入力情報
    void SetOnlineInput();

    //ディレイ方式用入力
    void DelayOnlineInput();
private:
    std::unique_ptr<NetwarkPost> n;
    std::unique_ptr<BasicsApplication> photonNet;
    Light* mainDirectionalLight = nullptr;
};