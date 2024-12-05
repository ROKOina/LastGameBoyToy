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

private:
    //�ڑ��J�n�t���O
    bool connectFlg = false;
    //������
    std::string roomName;
    //charaID
    std::string charaIDList[int(RegisterChara::CHARA_LIST::MAX)] =
    {
        "INAZAWA",
        "FARAH",
        "HAVE_ALL_ATTACK",
    };
    int charaID = 0;

public:
    //���͏��X�V
    void NetInputUpdate();
    void MyCharaInput();
    void NetCharaInput();

    //���̓����҂Ƃ̍���ۑ�
    void DelayUpdate();

    int GetPlayerNum();

    //�}�X�^�[�N���C�A���g�Ȃ̂�
    bool GetIsMasterPlayer();

    int GetServerTime();
    int GetServerTimeOffset();

    //���C�e���V
    int GetRoundTripTime();
    int GetRoundTripTimeVariance();

    int GetRoomPlayersNum();
    std::string GetRoomName();

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
    void AddPlayer(int id);

    //�Q�[���f�[�^���M
    void sendGameData(void);

    //���������M(�\���̏ꍇ��true)
    void sendJoinPermissionData(bool request);

    // events, triggered by certain operations of all players in the same room
    //�������ɓ���
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
    virtual void leaveRoomEventAction(int playerNr, bool isInactive);
    //�f�[�^��M
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
    void GameRecv(NetData recvData);    //�Q�[������M
    void JoinRecv(NetData recvData);    //������M

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

    int startTime = 0;

    PhotonState::States mState;

    UIListener* mpOutputListener;
    ExitGames::LoadBalancing::Client mLoadBalancingClient;
    ExitGames::Common::Logger mLogger;

    //���M�p�x�ims�j
    int sendMs = 1000 / 60.0f * 5;
    //int sendMs = 35;
    int oldMs;

    //�����\�����X�g(�z�X�g�̂ݎg�p)
    std::vector<NetData::JoinData> joinManager;
    bool joinPermission = false;    //��������

    //�e�N���C�A���g�C���v�b�g�ۑ�
    struct SaveInput
    {
        SaveInput()
        {
            inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
        }

        std::string name = {};
        int id;
        std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;

        ////������ID���猩���f�B���C
        //int myDelay = 50;

        int teamID = 0;

        //��񂪍X�V���ꂽ��
        bool isInputUpdate = false;

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

    int delayFrame = 0;

    std::string netName = {};

    std::vector<SaveInput> saveInputPhoton;
};