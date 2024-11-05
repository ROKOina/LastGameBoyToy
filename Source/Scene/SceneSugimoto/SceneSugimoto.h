#pragma once

#include "../Scene.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Graphics/Light/Light.h"

#include "Netwark/Photon/BasicsApplication.h"

// �Q�[���V�[��
class SceneSugimoto :public Scene
{
public:
    SceneSugimoto() {}
    ~SceneSugimoto()override;

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "SceneSugimoto"; };

private:
    // �e�v���C���[�̓��͏����A���ꂼ��̃L�����N�^�[�ɑ���
    void SetUserInputs();

    // �v���C���[( PC�̎����� )�̓��͏��
    void SetPlayerInput();

    // ���̃v���C���[( �I�����C�� )�̓��͏��
    void SetOnlineInput();

    //�f�B���C�����p����
    void DelayOnlineInput();

    //UI�̃Q�[���I�u�W�F�N�g����
    void CreateUiObject();

    //PVE�Q�[�����烊�U���g�ɑJ��
    void TransitionPVEFromResult();

private:
    Light* mainDirectionalLight = nullptr;

private:

    std::shared_ptr<TransformCom>t;
};