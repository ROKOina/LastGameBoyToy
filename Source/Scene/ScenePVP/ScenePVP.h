#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"

//���U���g�V�[��
class ScenePVP :public Scene
{
public:
    ScenePVP() {};
    ~ScenePVP()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "ScenePVP"; };

    //UI�̃Q�[���I�u�W�F�N�g����
    void CreateUiObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;
};
