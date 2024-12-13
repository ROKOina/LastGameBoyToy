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
    // wstring → SJIS
    int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str()
        , -1, (char*)NULL, 0, NULL, NULL);

    // バッファの取得
    CHAR* cpMultiByte = new CHAR[iBufferSize];

    // wstring → SJIS
    WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte
        , iBufferSize, NULL, NULL);

    // stringの生成
    std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

    // バッファの破棄
    delete[] cpMultiByte;

    // 変換結果を返す
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
    //接続開始フラグ
    bool connectFlg = false;

    //接続した瞬間
    bool connectBegin = false;
    //接続中
    bool connectNow = false;

    //部屋名
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
    //入力情報更新
    void NetInputUpdate();
    void MyCharaInput();
    void NetCharaInput();

    //他の入室者との差を保存
    void DelayUpdate();

    //キル数取得
    int GetKillCount(int team);

    int GetMyPhotonID();
    int GetMyPlayerID();

    void SetRoomName(std::wstring wstr) { roomName = WStringToString(wstr); }

    //マスタークライアントなのか
    bool GetIsMasterPlayer();

    //ゲームスタートしたか
    bool GetIsGamePlay() { return isGamePlay; }

    //キャラセレクト中か
    bool GetIsCharaSelect() { return isCharaSelect; }

    //ゲームモード登録
    int GetGameMode() { return gameMode; }

    //入室許可
    bool IsJoinPermission() { return joinPermission; }

    //今の試合時間
    float GetNowTime();

    //参加人数
    float GetJoinNum();

    int GetServerTime();
    int GetServerTimeOffset();

    //レイテンシ
    int GetRoundTripTime();
    int GetRoundTripTimeVariance();

    int GetRoomPlayersNum();
    std::string GetRoomName();
    //全ての部屋名を取得
    std::vector<std::wstring> GetRoomNames();

    //接続開始
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
    //表示用
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
    //プレイヤー追加
    void AddPlayer(int photonID, int playerID);

    //ゲームデータ送信
    void sendGameData(void);

    //入室許可送信(申請の場合はtrue)
    void sendJoinPermissionData(bool request);

    //入室許可送信(申請の場合はtrue)
    void sendLobbyData(void);

    //ゲームモード情報送信
    void sendGameModeData(void);
    void sendDeathMatchData(void);

    // events, triggered by certain operations of all players in the same room
    //入室時に入る
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
    virtual void leaveRoomEventAction(int playerNr, bool isInactive);
    //データ受信
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);
    void GameRecv(NetData recvData);    //ゲーム中受信
    void JoinRecv(NetData recvData);    //入室受信
    void LobbyRecv(NetData recvData);    //ロビー受信
    void DeathMatchRecv(NetData recvData);    //デスマッチ受信

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
    //地域を決める
    virtual void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegions*/, const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegionServers*/);

    PhotonState::States mState;

    UIListener* mpOutputListener;
    ExitGames::LoadBalancing::Client mLoadBalancingClient;
    ExitGames::Common::Logger mLogger;

    //送信頻度（ms）
    int sendMs = 1000 / 60.0f * 5;
    //int sendMs = 35;
    int oldMs;

    //各クライアントインプット保存
    struct SaveInput
    {
        SaveInput()
        {
            inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
        }
        bool useFlg = false;    //使用されているか
        std::string name = {};
        int photonId;
        int playerId;
        std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;

        ////自分のIDから見たディレイ
        //int myDelay = 50;

        int teamID = 0;

        //情報が更新されたか
        bool isInputUpdate = false;

        //キル数
        int killCount = 0;

        //次の入力情報を格納
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
    //追加予約
    int addSavePhotonID[4] = { -1,-1,-1,-1 };   //要素がプレイヤーID、値がフォトンIDになる

    //ロビーで名前とIDを紐づけする
    std::string savePlayerName[4];  //ゲーム中に参加処理をするため

    //タイマースタート時間
    int startTime = 0;

    //キャラ選択画面か
    bool isCharaSelect = false;

    //ゲーム中か
    bool isGamePlay = false;

    //入室申請リスト(ホストのみ使用)
    struct JoinManager
    {
        NetData::JoinData jData;
        std::string joinName;
    };
    std::vector<JoinManager> joinManager;
    bool joinPermission = false;    //入室許可

    //遅延フレーム
    int delayFrame = 0;
    //プレイヤーの名前
    std::string netName = {};

    //ゲームモード
    int gameMode = -1;

    //仮機能
    bool isSendChat = false;    //チャット送信フラグ
    std::string chat;   //チャット
    std::vector<std::string> chatList;
};