#pragma once

#include "Component\Sprite/Sprite.h"
#include "Scene/Scene.h"
#include <thread>

//���[�f�B���O�V�[��
class SceneLoading :public Scene
{
public:
    SceneLoading(Scene* nextScene) :nextScene_(nextScene) {}
    ~SceneLoading() {}

    //������
    void Initialize()override;

    //�I����
    void Finalize()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //�`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneLoading"; };

private:
    //���[�f�B���O�X���b�h
    static void LoadingThread(SceneLoading* scene);

private:
    Sprite* sprite_ = nullptr;

    Scene* nextScene_ = nullptr;
    std::thread* thread_ = nullptr;
};