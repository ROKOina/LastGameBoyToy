#include "SceneManager.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Character\CharacterCom.h"
#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "SceneLobby\SceneLobby.h"
#include "ScenePVE/ScenePVE.h"
#include "ScenePVP/ScenePVP.h"
#include "SceneTitle/SceneTitle.h"
#include "SceneResult/SceneResult.h"
#include "SceneTraining/SceneTraining.h"
#include "Scene\SceneLGBT\SceneLGBT.h"
#include "Scene\SceneNakanisi\SceneNakanisi.h"
#include "SceneUeno\SceneUeno.h"

#include "Setting/Setting.h"

SceneManager::SceneManager()
{
    //設定画面UIオブジェクト生成
    ss = std::make_shared<SettingScreen>();
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
        if (!currentScene_->IsReady())
        {
            currentScene_->Initialize();
            ss->CreateSettingUiObject();
            ss->SetViewSetting(false);
        }
        nextScene_ = nullptr;
    }

    if (currentScene_ != nullptr)
    {
        currentScene_->Update(elapsedTime);

        //設定画面更新
        ss->SettingScreenUpdate(elapsedTime);
    }

    //遅延遷移
    if (transitionFlag)
    {
        transitionTimer += elapsedTime;
        if (transitionTime < transitionTimer)
        {
            ChangeScene(transitionScene_);
            transitionFlag = false;
        }
    }
}

//描画処理
void SceneManager::Render(float elapsedTime)
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Render(elapsedTime);
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
#ifdef _DEBUG
    const Scene* scene = GetScene();
    if (scene != nullptr)
    {
        ImGui::Begin("Scene");
        if (ImGui::CollapsingHeader((char*)u8"シーン", ImGuiTreeNodeFlags_DefaultOpen))
        {
            constexpr const char* scene_names[] =
            {
                "SceneLGBT",
                "SceneTitle",
                "ScenePVE",
                "ScenePVP",
                "SceneTraining",
                "SceneLobby",
                "SceneResult",
                "SceneNakanisi",
                "SceneUeno",
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
                    new SceneLGBT,
                    new SceneTitle,
                    new ScenePVE,
                    new ScenePVP,
                    new SceneTraining,
                    new SceneLobby,
                    new SceneResult,
                    new SceneNakanisi,
                    new SceneUeno,
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
#endif
}

DirectX::XMFLOAT3 SceneManager::InputVec(std::shared_ptr<GameObject> obj) const
{
    std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();
    CameraCom* cameraCom = cameraObj->GetComponent<CameraCom>().get();

    //ステックのXY取得
    DirectX::XMFLOAT2 leftStick = obj->GetComponent<CharacterCom>()->GetLeftStick();
    float ax = leftStick.x;
    float ay = leftStick.y;

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