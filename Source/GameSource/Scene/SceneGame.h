#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Components/NodeCollsionCom.h"

#include "Netwark/Photon/BasicsApplication.h"

// �Q�[���V�[��
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //�G�t�F�N�g����
    void EffectNew();

    //���O�擾
    std::string GetName() const override { return "SceneGame"; };

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
    std::unique_ptr<NetwarkPost> n;
    std::unique_ptr<BasicsApplication> photonNet;
    Light* mainDirectionalLight = nullptr;

private:

    std::shared_ptr<TransformCom>t;
};