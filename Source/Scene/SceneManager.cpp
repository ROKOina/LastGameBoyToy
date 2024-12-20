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
    //�ݒ���UI�I�u�W�F�N�g����
    ss = std::make_shared<SettingScreen>();
}

SceneManager::~SceneManager()
{
}

//�X�V����
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
        //�Â��V�[�����I������
        Clear();

        //�V�����V�[����ݒ�
        currentScene_ = nextScene_;

        //�V�[������������
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

        //�ݒ��ʍX�V
        ss->SettingScreenUpdate(elapsedTime);
    }

    //�x���J��
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

//�`�揈��
void SceneManager::Render(float elapsedTime)
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Render(elapsedTime);
    }

    //imgui
    ImGui();
}

//�V�[���N���A
void SceneManager::Clear()
{
    if (currentScene_ != nullptr)
    {
        currentScene_->Finalize();
        delete currentScene_;
        currentScene_ = nullptr;
    }
}

//�V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    //�V�����V�[����ݒ�
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
        if (ImGui::CollapsingHeader((char*)u8"�V�[��", ImGuiTreeNodeFlags_DefaultOpen))
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

    //�X�e�b�N��XY�擾
    DirectX::XMFLOAT2 leftStick = obj->GetComponent<CharacterCom>()->GetLeftStick();
    float ax = leftStick.x;
    float ay = leftStick.y;

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    const DirectX::XMFLOAT3& rightVec = cameraCom->GetRight();
    const DirectX::XMFLOAT3& forwardVec = cameraCom->GetFront();

    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
    // �E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    DirectX::XMFLOAT2 rightXZ = { rightVec.x,rightVec.z };
    rightXZ = Mathf::Normalize(rightXZ);

    // �O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    DirectX::XMFLOAT2 forwardXZ = { forwardVec.x,forwardVec.z };
    forwardXZ = Mathf::Normalize(forwardXZ);

    // �X�e�B�b�N�̐������͒l���E�����ɔ��f���A
    // �X�e�B�b�N�̐������͒l��O�����ɔ��f���A
    // �i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = forwardXZ.x * ay + rightXZ.x * ax;
    vec.y = 0;
    vec.z = forwardXZ.y * ay + rightXZ.y * ax;

    return vec;
}