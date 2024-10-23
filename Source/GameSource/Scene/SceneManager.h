#pragma once

#include "Scene.h"

class GameObject;
class CameraCom;

//�V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager();
    ~SceneManager();

public:
    //�B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(float elapsedTime);

    //�V�[���N���A
    void Clear();

    //�V�[���؂�ւ�
    void ChangeScene(Scene* scene);
    //�V�[���؂�ւ�(�x��)
    void ChangeSceneDelay(Scene* scene,float time);

    //imgui
    void ImGui();

    // �V�[���擾
    Scene* GetScene() { return currentScene_; }
    Scene* GetNextScene() { return nextScene_; }

    //�J����
    void AddCameraActiveCount() { cameraActiveCount++; }
    const bool& GetCameraChange() const { return isChangeCamera; }
    void SetActiveCamera(std::shared_ptr<GameObject> camera) { activeCamera = camera; }
    std::shared_ptr<GameObject> GetActiveCamera() { return activeCamera.lock(); }

    // �J��������R���g���[���[�̐i�s�����擾
    DirectX::XMFLOAT3 InputVec() const;

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

    //�J�����A�N�e�B�u����
    std::weak_ptr<GameObject> activeCamera;
    int cameraActiveCount = 0;
    bool isChangeCamera = false;

    //�J�ڏ����p
    bool transitionAllRemoveFlag = false;   //�J�ڎ��ɃI�u�W�F�N�g����������true�ɂȂ�

    //�J�ڒx���p�ϐ�
    bool transitionFlag = false;
    float transitionTime = 0;
    float transitionTimer = 0;
    Scene* transitionScene_ = nullptr;
};