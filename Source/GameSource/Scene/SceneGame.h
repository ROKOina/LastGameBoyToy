#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
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

private:
    // �e�v���C���[�̓��͏����A���ꂼ��̃L�����N�^�[�ɑ���
    void SetUserInputs();

    // �v���C���[( PC�̎����� )�̓��͏��
    void SetPlayerInput();

    // ���̃v���C���[( �I�����C�� )�̓��͏��
    void SetOnlineInput();

    //�f�B���C�����p����
    void DelayOnlineInput();
private:
    std::unique_ptr<NetwarkPost> n;
    std::unique_ptr<BasicsApplication> photonNet;
    Light* mainDirectionalLight = nullptr;
};