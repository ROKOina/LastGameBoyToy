#pragma once

#include "LoadBalancing-cpp/inc/Client.h"
#include "UIListener.h"
#include <vector>
#include <string>

#include "../NetData.h"
#include "../RingBuffer.h"
#include "Component/Character/RegisterChara.h"

static std::string WStringToString
(
    std::wstring oWString
)
{
    // wstring �� SJIS
    int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str()
        , -1, (char*)NULL, 0, NULL, NULL);

    // �o�b�t�@�̎擾
    CHAR* cpMultiByte = new CHAR[iBufferSize];

    // wstring �� SJIS
    WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte
        , iBufferSize, NULL, NULL);

    // string�̐���
    std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

    // �o�b�t�@�̔j��
    delete[] cpMultiByte;

    // �ϊ����ʂ�Ԃ�
    return(oRet);
}

class PhotonLib : private ExitGames::LoadBalancing::Listener
{
public:
    PhotonLib(UIListener*);
    void update(float elapsedTime);
    void ImGui();
    ExitGames::Common::JString getStateString(void);

    void LobbyImGui();
    void ChatImGui();

private:
    //�ڑ��J�n�t���O
    bool connectFlg = false;

    //�ڑ������u��
    bool connectBegin = false;
    //�ڑ���
    bool connectNow = false;

    //������
    std::string roomName;
    //charaID
    std::string charaIDList[int(RegisterChara::CHARA_LIST::MAX)] =
    {
        "INAZAWA",
        "FARAH",
        "JANKRAT",
        "SOLDIER",
    };
    int charaID = 0;

public:
    //���͏��X�V
    void NetInputUpdate();
    void MyCharaInput();
    void NetCharaInput();

    //���̓����҂Ƃ̍���ۑ�
    void DelayUpdate();

    //�L�����擾
    int GetKillCount(int team);

    int GetMyPhotonID();
    int GetMyPlayerID();

    void SetRoomName(std::wstring wstr) { roomName = WStringToString(wstr); }

    //�}�X�^�[�N���C�A���g�Ȃ̂�
    bool GetIsMasterPlayer();

    //�Q�[���X�^�[�g������
    bool GetIsGamePlay() { return isGamePlay; }

    //�L�����Z���N�g����
    bool GetIsCharaSelect() { return isCharaSelect; }

    //�Q�[�����[�h�o�^
    int GetGameMode() { return gameMode; }

    //��������
    bool IsJoinPermission() { return joinPermission; }

    //���̎�������
    float GetNowTime();

    //�Q���l��
    float GetJoinNum();

    int GetServerTime();
    int GetServerTimeOffset();

    //���C�e���V
    int GetRoundTripTime();
    int GetRoundTripTimeVariance();

    int GetRoomPlayersNum();
    std::string GetRoomName();
    //�S�Ă̕��������擾
    std::vector<std::wstring> GetRoomNames();

    //�ڑ��J�n
    void StartConnect() { connectFlg = true; }
    bool GetConnectBegin() { return connectBegin; }
    bool GetConnectNow() { return connectNow; }

    int SendMs();

    class PhotonState
    {
    public:
        enum States
        {
            INITIALIZED = 0,
            CONNECTING,
            CONNECTED,
            JOINING,
            JOINED,
            SENT_DATA,
            RECEIVED_DATA,
            LEAVING,
            LEFT,
            DISCONNECTING,
            DISCONNECTED,
            CONNECTSAVE
        };
    };
    //�\���p
    std::vector<std::string> stateStr = {
        "INITIALIZED",
        "CONNECTING",
        "CONNECTED",
        "JOINING",
        "JOINED",
        "SENT_DATA",
        "RECEIVED_DATA",
        "LEAVING",
        "LEFT",
        "DISCONNECTING",
        "DISCONNECTED",
        "CONNECTSAVE"
    };
    PhotonState::States GetPhotonState() { return mState; }

private:
    //�v���C���[�ǉ�
    void AddPlayer(int photonID, int playerID);

    //�Q�[���f�[�^���M
    void sendGameData(void);

    //���������M(�\���̏ꍇ��true)
    void sendJoinPermissionData(bool request);

    //���������M(�\���̏ꍇ��true)
    void sendLobbyData(void);

    //�Q�[�����[�h��񑗐M
    void sendGameModeData(void);
    void sendDeathMatchData(void);

    // events, triggered by certain operations of all players in the same room
    //�������ɓ���
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
    virtual void leaveRoomEventAction(int playerNr, bool isInactive);
    //�f�[�^��M
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
    void GameRecv(NetData recvData);    //�Q�[������M
    void JoinRecv(NetData recvData);    //������M
    void LobbyRecv(NetData recvData);    //���r�[��M
    void DeathMatchRecv(NetData recvData);    //�f�X�}�b�`��M

    // receive and print out debug out here
    virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

    // implement your error-handling here
    virtual void connectionErrorReturn(int errorCode);
    virtual void clientErrorReturn(int errorCode);
    virtual void warningReturn(int warningCode);
    virtual void serverErrorReturn(int errorCode);

    // callbacks for operations on PhotonLoadBalancing server
    virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster);
    virtual void disconnectReturn(void);
    virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinLobbyReturn(void);
    virtual void leaveLobbyReturn(void);
    //�n������߂�
    virtual void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegions*/, const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegionServers*/);

    PhotonState::States mState;

    UIListener* mpOutputListener;
    ExitGames::LoadBalancing::Client mLoadBalancingClient;
    ExitGames::Common::Logger mLogger;

    //���M�p�x�ims�j
    int sendMs = 1000 / 60.0f * 5;
    //int sendMs = 35;
    int oldMs;

    //�e�N���C�A���g�C���v�b�g�ۑ�
    struct SaveInput
    {
        SaveInput()
        {
            inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
        }
        bool useFlg = false;    //�g�p����Ă��邩
        std::string name = {};
        int photonId;
        int playerId;
        std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;

        ////������ID���猩���f�B���C
        //int myDelay = 50;

        int teamID = 0;

        //��񂪍X�V���ꂽ��
        bool isInputUpdate = false;

        //�L����
        int killCount = 0;

        //���̓��͏����i�[
        struct NextInput
        {
            int oldFrame;
            unsigned int inputDown = 0;
            unsigned int input = 0;
            unsigned int inputUp = 0;

            DirectX::XMFLOAT2 leftStick = { 0,0 };
            DirectX::XMFLOAT3 pos = { 0,0,0 };
            DirectX::XMFLOAT4 rotato = { 0,0,0,1 };

            DirectX::XMFLOAT3 fpsCameraDir = { 0,0,1 };

            DirectX::XMFLOAT3 velocity = { 0,0,0 };
        };
        NextInput nextInput;
    };
    std::vector<SaveInput> saveInputPhoton;
    //�ǉ��\��
    int addSavePhotonID[4] = { -1,-1,-1,-1 };   //�v�f���v���C���[ID�A�l���t�H�g��ID�ɂȂ�

    //���r�[�Ŗ��O��ID��R�Â�����
    std::string savePlayerName[4];  //�Q�[�����ɎQ�����������邽��

    //�^�C�}�[�X�^�[�g����
    int startTime = 0;

    //�L�����I����ʂ�
    bool isCharaSelect = false;

    //�Q�[������
    bool isGamePlay = false;

    //�����\�����X�g(�z�X�g�̂ݎg�p)
    struct JoinManager
    {
        NetData::JoinData jData;
        std::string joinName;
    };
    std::vector<JoinManager> joinManager;
    bool joinPermission = false;    //��������

    //�x���t���[��
    int delayFrame = 0;
    //�v���C���[�̖��O
    std::string netName = {};

    //�Q�[�����[�h
    int gameMode = -1;

    //���@�\
    bool isSendChat = false;    //�`���b�g���M�t���O
    std::string chat;   //�`���b�g
    std::vector<std::string> chatList;
};