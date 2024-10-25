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
        if (!currentScene_->IsReady())currentScene_->Initialize();
        nextScene_ = nullptr;
    }

    if (currentScene_ != nullptr)
    {
        currentScene_->Update(elapsedTime);
    }

    //�x���J��
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

    //�����ŉ�ʈÓ]���Ă����I�Iby���
    PostEffect::Instance().ParameterMove(elapsedTime / 1.3f, 1.4f, exposetransiotnflag, PostEffect::PostEffectParameter::Exposure);
}

//�`�揈��
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
    const Scene* scene = GetScene();
    if (scene != nullptr)
    {
        ImGui::Begin("Scene");
        if (ImGui::CollapsingHeader((char*)u8"�V�[��", ImGuiTreeNodeFlags_DefaultOpen))
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

    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();

    //�X�e�b�N��XY�擾
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

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