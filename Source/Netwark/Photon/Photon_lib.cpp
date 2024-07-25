#include "Photon_lib.h"
#include "limits.h"

#include "Logger.h"

static const ExitGames::Common::JString appID = L"0d572336-477d-43ad-895e-59f4eeebbca9"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static ExitGames::Common::JString gameName = L"Basics";

static const ExitGames::Common::JString PLAYER_NAME = L"user";
static const int MAX_SENDCOUNT = 100;

PhotonLib::PhotonLib(UIListener* uiListener)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mState(PhotonState::INITIALIZED)
	, mpOutputListener(uiListener)
	//第四引数でクライアントのパラメーター・設定をしている
	, mLoadBalancingClient(*this, appID, appVersion, ExitGames::LoadBalancing::ClientConstructOptions(0U, true, 0U, true))
	, mSendCount(0)
	, mReceiveCount(0)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	//デバッグ出力レベル
	mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
	ExitGames::Common::Base::setListener(this);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
}

void PhotonLib::update(void)
{
	switch(mState)
	{
		case PhotonState::INITIALIZED:
			mLoadBalancingClient.connect(ExitGames::LoadBalancing::ConnectOptions().setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString()+GETTIMEMS())).setUsername(PLAYER_NAME+GETTIMEMS()).setTryUseDatagramEncryption(true));
			mState = PhotonState::CONNECTING;
			break;
		case PhotonState::CONNECTED:
			mLoadBalancingClient.opJoinOrCreateRoom(gameName);
			mState = PhotonState::JOINING;

			break;
		case PhotonState::JOINED:
			sendData();
			break;
		case PhotonState::RECEIVED_DATA:
			mLoadBalancingClient.opLeaveRoom();
			mState = PhotonState::LEAVING;
			break;
		case PhotonState::LEFT:
			mState = PhotonState::DISCONNECTING;
			mLoadBalancingClient.disconnect();
			break;
		case PhotonState::DISCONNECTED:
			mState = PhotonState::INITIALIZED;
			break;
		case PhotonState::CONNECTSAVE:
			mState = PhotonState::CONNECTSAVE;
			break;
		default:
			break;
	}
	mLoadBalancingClient.service();
}

ExitGames::Common::JString PhotonLib::getStateString(void)
{
	switch(mState)
	{
		case PhotonState::INITIALIZED:
			return L"disconnected\n";
		case PhotonState::CONNECTING:
			return L"connecting\n";
		case PhotonState::CONNECTED:
			return L"connected\n";
		case PhotonState::JOINING:
			return L"joining\n";
		case PhotonState::JOINED:
			return ExitGames::Common::JString(L"ingame\nsent event Nr. ") + mSendCount + L"\nreceived event Nr. " + mReceiveCount;
		case PhotonState::SENT_DATA:
			return ExitGames::Common::JString(L"sending completed") + L"\nreceived event Nr. " + mReceiveCount;
		case PhotonState::RECEIVED_DATA:
			return L"receiving completed\n";
		case PhotonState::LEAVING:
			return L"leaving\n";
		case PhotonState::LEFT:
			return L"left";
		case PhotonState::DISCONNECTING:
			return L"disconnecting\n";
		case PhotonState::DISCONNECTED:
			return L"disconnected\n";
		default:
			return L"unknown state";
	}
}

int PhotonLib::GetPlayerNum()
{
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();

	return myPlayerNumber;
}

void PhotonLib::sendData(void)
{
	ExitGames::Common::Hashtable event;
	event.put(static_cast<nByte>(0), ++mSendCount);
	// send to ourselves only
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
	mLoadBalancingClient.opRaiseEvent(true, event, 0);
	//mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
	if(mSendCount >= MAX_SENDCOUNT)
		mState = PhotonState::SENT_DATA;
}

void PhotonLib::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
	mpOutputListener->writeString(string);
}

void PhotonLib::connectionErrorReturn(int errorCode)
{
	Logger::Print(WStringToString(L"code: %d" + errorCode).c_str());
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received connection error ") + errorCode);
	mState = PhotonState::DISCONNECTED;
}

void PhotonLib::clientErrorReturn(int errorCode)
{
	Logger::Print(std::string("code: " + errorCode).c_str());
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + L" from client");
}

void PhotonLib::warningReturn(int warningCode)
{
	Logger::Print(std::string("code: " + warningCode).c_str());
	EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client");
}

void PhotonLib::serverErrorReturn(int errorCode)
{
	Logger::Print(std::string("code: " + errorCode).c_str());
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + " from server");
}

void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	Logger::Print(std::string("ls joined the game" + WStringToString(player.getName().cstr())).c_str());
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");
}

void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");
}

void PhotonLib::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
	switch(eventCode)
	{
	case 0:
		if (eventContent.getValue((nByte)0))
		{
			auto p = ((ExitGames::Common::ValueObject<int64>*)(eventContent.getValue((nByte)0)));
			mReceiveCount = ((ExitGames::Common::ValueObject<int64>*)(eventContent.getValue((nByte)0)))->getDataCopy();
		}
		if(mState == PhotonState::SENT_DATA && mReceiveCount >= mSendCount)
		{
			mState = PhotonState::RECEIVED_DATA;
			mSendCount = 0;
			mReceiveCount = 0;
		}
		break;
	default:
		break;
	}

}

void PhotonLib::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
	Logger::Print(std::string("connected to cluster " + WStringToString(cluster.cstr()) + " of region " + WStringToString(region.cstr())).c_str());

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mState = PhotonState::DISCONNECTING;
		return;
	}
	mpOutputListener->writeString(L"connected to cluster " + cluster);
	mState = PhotonState::CONNECTED;
}

void PhotonLib::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"disconnected");
	mState = PhotonState::DISCONNECTED;
}

void PhotonLib::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opCreateRoom() failed: " + errorString);
		mState = PhotonState::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been created");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = PhotonState::JOINED;
}

void PhotonLib::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinOrCreateRoom() failed: " + errorString);
		mState = PhotonState::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	mpOutputListener->writeString(L"regularly sending dummy events now");

	mState = PhotonState::JOINED;
}

void PhotonLib::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{		
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRoom() failed: " + errorString);
		mState = PhotonState::CONNECTED;
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");

	mState = PhotonState::JOINED;
}

void PhotonLib::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRandomRoom() failed: " + errorString);
		mState = PhotonState::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);	
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = PhotonState::JOINED;
}

void PhotonLib::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opLeaveRoom() failed: " + errorString);
		mState = PhotonState::DISCONNECTING;
		return;
	}
	mState = PhotonState::LEFT;
	mpOutputListener->writeString(L"room has been successfully left");
}

void PhotonLib::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"joined lobby");
}

void PhotonLib::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"left lobby");
}