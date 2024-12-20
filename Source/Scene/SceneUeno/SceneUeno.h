#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"

//���V�[��
class SceneUeno :public Scene
{
public:
    SceneUeno() {};
    ~SceneUeno()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneUeno"; };

private:

    //�I�u�W�F�N�g�����֐�
    void NewObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;
};
