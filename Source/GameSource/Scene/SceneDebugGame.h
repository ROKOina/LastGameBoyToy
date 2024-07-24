#pragma once

#include <memory>
#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include <array>

//前方宣言
class SceneDebugGame;

enum class SCENE_ACT
{
    LOGIN,
    CHARACTER_SLECT,
    BATTLE,
};



// ゲームシーン
class SceneDebugGame :public Scene
{
public:
    SceneDebugGame() {}
    ~SceneDebugGame()override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    void LoginInitialize();
    void LoginUpdate(float elapsedTime);
    void LoginRender(float elapsedTime);

    void GameInitialize();
    void GameUpdate(float elapsedTime);
    void GameRender(float elapsedTime);

    // 各プレイヤーの入力情報を、それぞれのキャラクターに送る
    void SetUserInputs();

    // プレイヤー( PCの持ち主 )の入力情報
    void SetPlayerInput();

    // 他のプレイヤー( オンライン )の入力情報
    void SetOnlineInput();

public:
    static constexpr int MAX_PLAYER_NUM = 6;

    std::array<std::weak_ptr<GameObject>, MAX_PLAYER_NUM>& GetPlayers() { return players; }
    std::unique_ptr<NetwarkPost>& GetNetWarkPost() { return n; }
    Light* GetMainDirectionalLight() { return mainDirectionalLight; }

private:
    std::unique_ptr<NetwarkPost> n;
    Light* mainDirectionalLight = nullptr;

    bool isLogin = true;
    std::array<std::weak_ptr<GameObject>,MAX_PLAYER_NUM> players;
};
