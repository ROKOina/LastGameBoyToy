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

    //�J����
    void AddCameraActiveCount() { cameraActiveCount++; }
    const bool& GetCameraChange() const { return isChangeCamera; }
    void SetActiveCamera(std::shared_ptr<GameObject> camera) { activeCamera = camera; }
    std::shared_ptr<GameObject> GetActiveCamera() { return activeCamera.lock(); }

    std::shared_ptr<GameObject> GetParticleObj()
    {
        return changeParticle_;
    }

    bool GetParticleUpdate() { return isParticleUpdate_; }
    void SetParticleUpdate(bool particleUpdate) { isParticleUpdate_ = particleUpdate; }

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
};