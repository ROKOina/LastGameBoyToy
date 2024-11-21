#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Component\System\GameObject.h"

//LGBT�V�[��
class SceneLGBT :public Scene
{
public:
    SceneLGBT() {};
    ~SceneLGBT()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override {};

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneLGBT"; }

private:

    //�V�[���J��
    void SceneTransition(float elapsedTime);

private:
    float dissolvetime = 0.0f;
    float easingtime = 0.0f;
};