#include "SceneManager.h"

#include "Components/System/GameObject.h"
#include "Components\ParticleSystemCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Graphics/Graphics.h"

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
}


//更新処理
void SceneManager::Update(float elapsedTime)
{
    if (nextScene_ != nullptr)
    {
        //古いシーンを終了処理
        Clear();

        //新しいシーンを設定
        currentScene_ = nextScene_;
        nextScene_ = nullptr;

        //シーン初期化処理
        if (!currentScene_->IsReady())currentScene_->Initialize();
    }

    if (currentScene_ != nullptr)
    {
        currentScene_->Update(elapsedTime);

        //カメラチェンジ処理用
        isChangeCamera = false;
        if (cameraActiveCount >= 2)
        {
            isChangeCamera = true;
        }
        cameraActiveCount = 0;
    }
}

//描画処理
void SceneManager::Render(float elapsedTime)
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Render(elapsedTime);
    }
}

//シーンクリア
void SceneManager::Clear()
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Finalize();
        delete currentScene_;
        currentScene_ = nullptr;
    }
}

//シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
    //新しいシーンを設定
    nextScene_ = scene;
}

