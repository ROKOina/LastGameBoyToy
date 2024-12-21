#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"

//���U���g�V�[��
class SceneNakanisi :public Scene
{
public:
    SceneNakanisi() {};
    ~SceneNakanisi()override {};

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneNakanisi"; };

private:

    //�I�u�W�F�N�g�����֐�
    void NewObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;
};
