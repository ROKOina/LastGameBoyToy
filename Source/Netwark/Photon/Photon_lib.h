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

	void MyCharaInput();

	int GetPlayerNum();

	int GetServerTime();
	int GetServerTimeOffset();

	//レイテンシ
	int GetRoundTripTime();
	int GetRoundTripTimeVariance();

	int GetRoomPlayersNum();
	std::string GetRoomName();

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
	// receive and print out debug out here
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);

	// events, triggered by certain operations of all players in the same room
	//入室時に入る
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr, bool isInactive);
	//データ受信
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

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
	virtual void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegions*/, const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegionServers*/);

	void sendData(void);

	PhotonState::States mState;

	UIListener* mpOutputListener;
	ExitGames::LoadBalancing::Client mLoadBalancingClient;
	ExitGames::Common::Logger mLogger;

	int64 mSendCount;
	int64 mReceiveCount;

	//各クライアントインプット保存
	struct SaveInput
	{
		int id;
		std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;
		SaveInput()
		{
			inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
		}
	};
	std::vector<SaveInput> saveInputPhoton;
};