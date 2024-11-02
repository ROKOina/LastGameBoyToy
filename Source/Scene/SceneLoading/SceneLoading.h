#pragma once

#include "Component\Sprite/Sprite.h"
#include "Scene/Scene.h"
#include <thread>

//ローディングシーン
class SceneLoading :public Scene
{
public:
    SceneLoading(Scene* nextScene) :nextScene_(nextScene) {}
    ~SceneLoading() {}

    //初期化
    void Initialize()override;

    //終了化
    void Finalize()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneLoading"; };

private:
    //ローディングスレッド
    static void LoadingThread(SceneLoading* scene);

private:
    Sprite* sprite_ = nullptr;

    Scene* nextScene_ = nullptr;
    std::thread* thread_ = nullptr;
};