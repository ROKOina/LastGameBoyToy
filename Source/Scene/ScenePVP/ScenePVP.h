#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\System\GameObject.h"

#include "PVPGameSystem/PVPGameSystem.h"
// �O���錾
class CharaPicks;

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

    //��ʂɉ������X�V����
    void TransitionUpdate(float elapsedTime);

    //���r�[�I����
    void LobbySelectFontUpdate(float elapsedTime);  //fontUpdate
    //���r�[��
    void LobbyFontUpdate(float elapsedTime);    //fontUpdate
    //�L�����Z���N�g��
    void CharaSelectUpdate(float elapsedTime);
    //�Q�[������
    void GameUpdate(float elapsedTime);
    //�w�iUodate
    void LobbyBackSprUpdate(float elapsedTime);
private:

    std::shared_ptr<CharaPicks>          charaPicks;
    std::unique_ptr<BasicsApplication> photonNet;

    bool isGame = false;
    bool isCharaSelect = false;
    bool isLobby = false;
    int lobbyState = 0; //0:���r�[�I��   1:���r�[  2:�L�����I��  3:�Q�[����

    std::vector<std::weak_ptr<GameObject>> tempRemoveObj;   //��ʐ؂�ւ����ɍ폜����I�u�W�F�N�g

    //�Q�[���V�X�e��
    std::unique_ptr<PVPGameSystem> pvpGameSystem;

    struct LobbySelectFont
    {
        LobbySelectFont(int id, std::wstring str, DirectX::XMFLOAT2 pos, float scale, bool col, int state)
            :id(id), str(str), pos(pos), scale(scale), collision(col), state(state) {}
        int id;
        std::wstring str;
        DirectX::XMFLOAT2 pos;
        float scale;

        bool collision;
        int state;  //�؂�ւ��悤
    };
    //���r�[�I��
    std::vector<LobbySelectFont> lobbySelectFont =
    {
        LobbySelectFont(0,L"�V�K�����쐬",{303,320},1.5f,true,0),
        LobbySelectFont(1,L"���r�[��",{465,223},1,false,1),
        LobbySelectFont(2,L"",{784,223},1,false,1),   //���r�[���ł����ݗp
        LobbySelectFont(3,L"�쐬",{1203,624},1.5f,true,1),
        LobbySelectFont(4,L"�߂�",{100,700},1.5f,true,1),

        LobbySelectFont(5,L"���r�[�I��",{76,54},2,false,0),
        LobbySelectFont(6,L"�V�K�����쐬",{76,54},2,false,1),

        LobbySelectFont(10,L"�����Q��",{906,320},1.5f,true,0),
        LobbySelectFont(11,L"�߂�",{100,700},1.5f,true,2),
        LobbySelectFont(12,L"����",{1203,624},1.5f,true,2),

        LobbySelectFont(13,L"�����Q��",{76,54},2,false,2),

        //���r�[���p
        LobbySelectFont(20,L"",{536,218},1.0f,true,2),
        LobbySelectFont(21,L"",{536,218},1.0f,true,2),
        LobbySelectFont(22,L"",{536,218},1.0f,true,2),
        LobbySelectFont(23,L"",{536,218},1.0f,true,2),
        LobbySelectFont(24,L"",{536,218},1.0f,true,2),
    };
    //���r�[
    std::vector<LobbySelectFont> lobbyFont =
    {
        LobbySelectFont(0,L"���r�[",{76,54},2,false,0),
        LobbySelectFont(1,L"�Q����",{455,210},1,false,0),
        LobbySelectFont(2,L"�v���C",{1303,747},2.5f,true,0),

        LobbySelectFont(10,L"�Q�[�����[�h",{1260,52},1.5f,false,0),
        LobbySelectFont(11,L"�`�[���f�X�}�b�`",{1435,201},1,true,0),
        LobbySelectFont(12,L"����",{1435,281},1,true,0),

        //�l�b�g���p
        LobbySelectFont(20,L"",{536,218},1.0f,true,2),
        LobbySelectFont(21,L"",{536,218},1.0f,true,2),
        LobbySelectFont(22,L"",{536,218},1.0f,true,2),
        LobbySelectFont(23,L"",{536,218},1.0f,true,2),
    };

    int fontState = 0;
    int joinRoomCount = -1; //�����Q�����o�^�p
};
