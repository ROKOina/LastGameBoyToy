#include "LoadBalancing-cpp/inc/Client.h"
#include "UIListener.h"
#include <vector>
#include <string>

#include "../NetwarkPost.h"

class PhotonLib : private ExitGames::LoadBalancing::Listener
{
public:
	PhotonLib(UIListener*);
	void update(void);
	ExitGames::Common::JString getStateString(void);

	void NetInputUpdate();
	void MyCharaInput();
	void NetCharaInput();

	//他の入室者との差を保存
	void DelayUpdate();

	int GetPlayerNum();

	int GetServerTime();
	int GetServerTimeOffset();

	//レイテンシ
	int GetRoundTripTime();
	int GetRoundTripTimeVariance();

	//他の入室者との最新フレーム差
	std::vector<int> GetTrips();


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
	void sendData(void);

	// events, triggered by certain operations of all players in the same room
	//入室時に入る
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr, bool isInactive);
	//データ受信
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

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

	int64 mSendCount;
	int64 mReceiveCount;

	//送信頻度（ms）
	int sendMs = 35;
	int oldMs;



	//各クライアントインプット保存
	struct SaveInput
	{
		SaveInput()
		{
			inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
		}

		int id;
		std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;

		//自分のIDから見たディレイ
		int myDelay = 50;

		//次の入力情報を格納
		struct NextInput
		{
			int oldFrame;
			unsigned int inputDown = 0;
			unsigned int input = 0;
			unsigned int inputUp = 0;
		};
		NextInput nextInput;
	};

	std::vector<SaveInput> saveInputPhoton;
};