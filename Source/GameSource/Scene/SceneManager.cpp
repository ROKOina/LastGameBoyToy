#include "SceneManager.h"

#include "Components/System/GameObject.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Graphics/Graphics.h"
#include "Input\Input.h"

#include "SceneDebugGame.h"
#include "SceneGame.h"
#include "SceneIKTest.h"

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

        //シーン初期化処理
        if (!currentScene_->IsReady())currentScene_->Initialize();
        nextScene_ = nullptr;
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

    //危険かも
    GameObjectManager::Instance().AllRemove();
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

    //入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //カメラ方向とスティックの入力値によって進行方向を計算する
    const DirectX::XMFLOAT3& cameraRight = cameraCom->GetRight();
    const DirectX::XMFLOAT3& cameraFront = cameraCom->GetFront();

    //移動ベクトルはXZ平面

    //カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        //単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    //スティックの水平入力値をカメラ右方向に反映し、
    //スティックの垂直入力値をカメラ前方向に反映し、
    //進行ベクトルを計算する
    DirectX::XMFLOAT3 vec = {};
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    vec.y = 0.0f;

    return vec;
}