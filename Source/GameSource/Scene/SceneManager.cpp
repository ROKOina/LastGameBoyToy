#include "SceneManager.h"

#include "Components/System/GameObject.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Graphics/Graphics.h"
#include "Input\Input.h"

#include "SceneDebugGame.h"
#include "SceneGame.h"
#include "SceneIKTest.h"
#include <GameSource/Math/Mathf.h>
#include "ScenePVE/ScenePVE.h"
#include "ScenePVP/ScenePVP.h"
#include "SceneTitle/SceneTitle.h"
#include "SceneResult/SceneResult.h"
#include "SceneTraining/SceneTraining.h"
#include "Graphics\Shaders\PostEffect.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

//更新処理
void SceneManager::Update(float elapsedTime)
{
    if (transitionAllRemoveFlag)
    {
        transitionAllRemoveFlag = false;
        GameObjectManager::Instance().AllRemove();
        GameObjectManager::Instance().Update(elapsedTime);
    }

    if (nextScene_ != nullptr)
    {
        //古いシーンを終了処理
        Clear();

        //新しいシーンを設定
        currentScene_ = nextScene_;

        //シーン初期化処理
        if (!currentScene_->IsReady())currentScene_->Initialize();
        nextScene_ = nullptr;
    }

    if (currentScene_ != nullptr)
    {
        currentScene_->Update(elapsedTime);
    }

    //遅延遷移
    if (transitionFlag)
    {
        transitionTimer += elapsedTime;
        exposetransiotnflag = false;
        if (transitionTime < transitionTimer)
        {
            ChangeScene(transitionScene_);
            transitionFlag = false;
        }
    }

    //ここで画面暗転しているよ！！by上野
    PostEffect::Instance().ParameterMove(elapsedTime / 1.3f, 1.4f, exposetransiotnflag, PostEffect::PostEffectParameter::Exposure);
}

//描画処理
void SceneManager::Render(float elapsedTime)
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Render(elapsedTime);
        exposetransiotnflag = true;
    }

    //imgui
    ImGui();
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

    transitionAllRemoveFlag = true;
}

void SceneManager::ChangeSceneDelay(Scene* scene, float time)
{
    transitionFlag = true;
    transitionScene_ = scene;
    transitionTime = time;
    transitionTimer = 0;
}

//imgui
void SceneManager::ImGui()
{
    const Scene* scene = GetScene();
    if (scene != nullptr)
    {
        ImGui::Begin("Scene");
        if (ImGui::CollapsingHeader((char*)u8"シーン", ImGuiTreeNodeFlags_DefaultOpen))
        {
            constexpr const char* scene_names[] =
            {
                "SceneGame",
                "SceneDebugGame",
                "SceneIKTest",
                "SceneStageSelect",
                "SceneTitle",
                "SceneResult",
                "ScenePVE",
                "ScenePVP",
                "SceneTraining"
            };
            int scene_id = 0;
            int last_index = sizeof(scene_names) / sizeof(const char*);
            for (int i = 0; i < _countof(scene_names); i++)
            {
                if (scene->GetName() == scene_names[i])scene_id = i;
            }
            if (ImGui::Combo("Scene", &scene_id, scene_names, last_index, _countof(scene_names) - 1))
            {
                Scene* scenes[] =
                {
                    // Game scenes
                    new SceneGame,
                    new SceneDebugGame,
                    new SceneIKTest,
                    new SceneTitle,
                    new SceneResult,
                    new ScenePVE,
                    new ScenePVP,
                    new SceneTraining
                };
                ChangeScene((scenes[scene_id]));
                for (int i = 0; i < _countof(scenes); i++)
                {
                    if (i != scene_id)delete	scenes[i];
                }
            }
        }
        ImGui::End();
    }
}

DirectX::XMFLOAT3 SceneManager::InputVec() const
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();
    CameraCom* cameraCom = cameraObj->GetComponent<CameraCom>().get();

    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();

    //ステックのXY取得
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& rightVec = cameraCom->GetRight();
    const DirectX::XMFLOAT3& forwardVec = cameraCom->GetFront();

    // 移動ベクトルはXZ平面に水平なベクトルになるようにする
    // 右方向ベクトルをXZ単位ベクトルに変換
    DirectX::XMFLOAT2 rightXZ = { rightVec.x,rightVec.z };
    rightXZ = Mathf::Normalize(rightXZ);

    // 前方向ベクトルをXZ単位ベクトルに変換
    DirectX::XMFLOAT2 forwardXZ = { forwardVec.x,forwardVec.z };
    forwardXZ = Mathf::Normalize(forwardXZ);

    // スティックの水平入力値を右方向に反映し、
    // スティックの垂直入力値を前方向に反映し、
    // 進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = forwardXZ.x * ay + rightXZ.x * ax;
    vec.y = 0;
    vec.z = forwardXZ.y * ay + rightXZ.y * ax;

    return vec;
}