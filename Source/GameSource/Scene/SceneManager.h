#pragma once

#include "Scene.h"

class GameObject;
class CameraCom;

//シーンマネージャー
class SceneManager
{
private:
    SceneManager();
    ~SceneManager();

public:
    //唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(float elapsedTime);

    //シーンクリア
    void Clear();

    //シーン切り替え
    void ChangeScene(Scene* scene);
    //シーン切り替え(遅延)
    void ChangeSceneDelay(Scene* scene, float time);

    //imgui
    void ImGui();

    // シーン取得
    Scene* GetScene() { return currentScene_; }
    Scene* GetNextScene() { return nextScene_; }

    //カメラ
    void SetActiveCamera(std::shared_ptr<GameObject> camera) { activeCamera = camera; }
    std::shared_ptr<GameObject> GetActiveCamera() { return activeCamera.lock(); }

    // カメラからコントローラーの進行方向取得
    DirectX::XMFLOAT3 InputVec(std::shared_ptr<GameObject> obj) const;

    std::shared_ptr<GameObject> GetParticleObj()
    {
        return changeParticle_;
    }

    bool GetParticleUpdate() { return isParticleUpdate_; }
    void SetParticleUpdate(bool particleUpdate) { isParticleUpdate_ = particleUpdate; }

    bool GetTransitionFlag() { return transitionFlag; }

private:
    Scene* currentScene_ = nullptr;
    Scene* nextScene_ = nullptr;

    bool isParticleUpdate_ = true;
    std::shared_ptr<GameObject> changeParticle_;
    std::shared_ptr<GameObject> changeParticleCamera_;

    //カメラアクティブ制御
    std::weak_ptr<GameObject> activeCamera;

    //遷移処理用
    bool transitionAllRemoveFlag = false;   //遷移時にオブジェクトを消す時にtrueになる

    //遷移遅延用変数
    bool transitionFlag = false;
    bool exposetransiotnflag = true;
    float transitionTime = 0;
    float transitionTimer = 0;
    Scene* transitionScene_ = nullptr;
};