#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\System\GameObject.h"

#include "PVPGameSystem/PVPGameSystem.h"

//���U���g�V�[��
class ScenePVP :public Scene
{
public:
    ScenePVP() {};
    ~ScenePVP()override {};

    // ������
    void Initialize()override;

    //�������P�i���r�[�I���j
    void InitializeLobbySelect();
    //�������Q�i���r�[�j
    void InitializeLobby();
    //�������R�i�L�����Z���N�g�j
    void InitializeCharaSelect();
    //�������S�iPVP�j
    void InitializePVP();

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    //���O�擾
    std::string GetName() const override { return "ScenePVP"; };

private:

    //�I�u�W�F�N�g�����֐�
    void NewObject();

    //�Q�[���V�X�e���X�V
    void GameSystemUpdate(float elapsedTime);

private:

    std::unique_ptr<BasicsApplication> photonNet;

    bool isGame = false;
    bool isCharaSelect = false;
    bool isLobby = false;

    std::vector<std::weak_ptr<GameObject>> tempRemoveObj;   //��ʐ؂�ւ����ɍ폜����I�u�W�F�N�g

    //�Q�[���V�X�e��
    std::unique_ptr<PVPGameSystem> pvpGameSystem;
};
