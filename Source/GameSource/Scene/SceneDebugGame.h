#pragma once

#include <memory>
#include "Scene.h"
#include "GameSource\Sprite\SpriteObject.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Components\System\StateMachine.h"
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

class SceneState_Login : public State<SceneDebugGame>
{
    void Enter() override;
    void Update() override;
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

private:
    // 各プレイヤーの入力情報を、それぞれのキャラクターに送る
    void SetUserInputs();

    // プレイヤー( PCの持ち主 )の入力情報
    void SetPlayerInput();

    // 他のプレイヤー( オンライン )の入力情報
    void SetOnlineInput();

public:
    static constexpr int MAX_PLAYER_NUM = 6;


    std::array<std::weak_ptr<GameObject>, MAX_PLAYER_NUM> GetPlayers() { return players; }

private:
    std::unique_ptr<NetwarkPost> n;
    Light* mainDirectionalLight = nullptr;

    SpriteObject* sprite = nullptr;

    StateMachine<SceneActionState, SCENE_ACT> stateMachine;
    std::array<std::weak_ptr<GameObject>,MAX_PLAYER_NUM> players;
};


