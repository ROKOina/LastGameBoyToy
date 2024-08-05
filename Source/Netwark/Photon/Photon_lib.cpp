#include "Photon_lib.h"
#include "limits.h"

#include "Logger.h"

#include "Input\Input.h"
#include "Input\GamePad.h"

#include "../NetData.h"
#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Components/MovementCom.h"
#include "Components/ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"

#include "StaticSendDataManager.h"

#include "imgui.h"

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
	, mLoadBalancingClient(*this, appID, appVersion, ExitGames::LoadBalancing::ClientConstructOptions(0U, true, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, true))
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

void PhotonLib::update(float elapsedTime)
{
	//自分の入力保存
	int myID = GetPlayerNum();
	for (auto& s : saveInputPhoton)
	{
		if (s.id != myID)continue;

		GamePad& gamePad = Input::Instance().GetGamePad();

		SaveBuffer save;
		save.frame = GetServerTime();
		save.input = gamePad.GetButton();
		save.inputDown = gamePad.GetButtonDown();
		save.inputUp = gamePad.GetButtonUp();

		s.inputBuf->Enqueue(save);

		break;
	}

	switch(mState)
	{
	case PhotonState::INITIALIZED:
		mLoadBalancingClient.connect(ExitGames::LoadBalancing::ConnectOptions().setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS())).setUsername(PLAYER_NAME + GETTIMEMS()).setTryUseDatagramEncryption(true));
		mState = PhotonState::CONNECTING;
		break;
	case PhotonState::CONNECTED:
	{
		if (!joinPermission)break;
		mLoadBalancingClient.opJoinOrCreateRoom(ExitGames::Common::JString(roomName.c_str()));
		mState = PhotonState::JOINING;
		break;
	}
		case PhotonState::JOINING:
			oldMs = GetServerTime();
			startTime= GetServerTime();
			break;
		case PhotonState::JOINED:
			//情報送信
			if (GetServerTime() - oldMs > sendMs)
			{
				sendData();
				oldMs = GetServerTime();
			}
			else
			{
				//もしoldMSに値が初期値だった場合
				if (oldMs == 0)
				{
					oldMs = GetServerTime();
					startTime = GetServerTime();
				}
			}
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

	DelayUpdate();
	NetInputUpdate();
	MyCharaInput();
	NetCharaInput();
}

void PhotonLib::ImGui()
{
	//ネットワーク決定仮ボタン
	ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

	//ID表示
	int photonID = GetPlayerNum();
	ImGui::InputInt("photonID", &photonID);

	//状態表示
	std::string nowState = stateStr[GetPhotonState()];
	ImGui::Text(("State : " + nowState).c_str());

	ImGui::Separator();

	//ルーム名前
	ImGui::Text(("RoomName : " + GetRoomName()).c_str());

	//ルーム人数表示
	int roomCount = GetRoomPlayersNum();
	ImGui::InputInt("roomPlayersNum", &roomCount);


	//サーバー時間
	float time = (GetServerTime() - startTime) / 1000.0f;
	ImGui::DragFloat("time", &time);
	

	int serverTime = GetServerTime();
	int serverTimeOF = GetServerTimeOffset();
	ImGui::InputInt("serverTime", &serverTime);

	int A = serverTime / 1000;
	A %= 10;
	ImGui::InputInt("A", &A);


	//時間デバッグ比較用
	//serverTime += serverTimeOF;
	if (serverTime != 0)
	{
		int slTime = 10000;
		static int saveTime = 0;
		static int oldSerTime = serverTime;
		saveTime += serverTime - oldSerTime;
		oldSerTime = serverTime;
		static bool reverseB = true;
		if (saveTime > slTime)
		{
			saveTime = 0;
			reverseB = !reverseB;
		}
		if (saveTime < 0)saveTime = 0;
		ImGui::InputInt("saveTime", &saveTime);
		ImGui::Checkbox("SerVV", &reverseB);
	}


	ImGui::InputInt("serverTimeOFF", &serverTimeOF);

	int roundTime = GetRoundTripTime();
	int roundTimeV = GetRoundTripTimeVariance();
	ImGui::InputInt("roundTime", &roundTime);

	std::vector<int> trips = GetTrips();
	for (int i = 0; i < trips.size(); ++i)
	{
		ImGui::InputInt(("trip" + std::to_string(i)).c_str(), &trips[i]);
	}

	int sendMs = SendMs();
	ImGui::InputInt("sendMS", &sendMs);


	ImGui::End();

	LobbyImGui();
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
			return L"JOINED\n";
		case PhotonState::SENT_DATA:
			return L"SENT_DATA\n";
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

void PhotonLib::LobbyImGui()
{
	if (!joinPermission)
	{
		//ネットワーク決定仮ボタン
		ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGui::Begin("PhotonNetLobby", nullptr, ImGuiWindowFlags_None);


		//新しくルーム生成
		char name[256];
		::strncpy_s(name, sizeof(name), roomName.c_str(), sizeof(name));
		if (ImGui::InputText(" ", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			roomName = name;
		}

		if (ImGui::Button("CreateOrJoinRoom"))
		{
			if (roomName.size() > 0)
				joinPermission = true;
		}

		ImGui::Separator();

		//ルームに参加
		auto rooms = mLoadBalancingClient.getRoomList();

		for (int i = 0; i < rooms.getSize(); ++i)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

			std::string selectRoom = WStringToString(rooms[i]->getName().cstr());
			ImGui::TreeNodeEx(&rooms[i], nodeFlags, selectRoom.c_str());

			// クリックすると選択
			if (ImGui::IsItemClicked())
			{
				roomName = selectRoom;
			}

			ImGui::TreePop();
		}


		ImGui::End();
	}
}

void PhotonLib::NetInputUpdate()
{
	for (auto& s : saveInputPhoton)
	{
		int nowTime = GetServerTime();

		std::vector<SaveBuffer> saveB;
		saveB = s.inputBuf->GetHeadFromSize(100);

		bool isInputInit = false;
		for (auto& b : saveB)
		{
			//前回のフレームから今回のフレームからディレイした分までの入力を保存
			if (b.frame < s.nextInput.oldFrame)break;
			if (b.frame > nowTime - s.myDelay)continue;

			if (!isInputInit)	//最初に入った時に入力初期化
			{
				s.nextInput.inputDown = 0;
				s.nextInput.input = 0;
				s.nextInput.inputUp = 0;

				isInputInit = true;
			}

			s.nextInput.inputDown |= b.inputDown;
			s.nextInput.input |= b.input;
			s.nextInput.inputUp |= b.inputUp;
		}

		s.nextInput.oldFrame = nowTime - s.myDelay;
	}

}

void PhotonLib::MyCharaInput()
{
	std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
	if (obj.use_count() == 0)return;

	std::shared_ptr<CharacterCom> chara = obj->GetComponent<CharacterCom>();
	if (chara.use_count() == 0) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// 入力情報をプレイヤーキャラクターに送信
	int myID = GetPlayerNum();
	for (auto& s : saveInputPhoton)
	{
		if (s.id != myID)continue;

		chara->SetUserInput(s.nextInput.input);
		chara->SetUserInputDown(s.nextInput.inputDown);
		chara->SetUserInputUp(s.nextInput.inputUp);

		//s.nextInput.input = 0;
		s.nextInput.inputDown = 0;
		s.nextInput.inputUp = 0;

		break;
	}


	chara->SetLeftStick(gamePad.GetAxisL());
	chara->SetRightStick(gamePad.GetAxisR());
}

void PhotonLib::NetCharaInput()
{
	for (auto& s : saveInputPhoton)
	{
		std::string name = "netPlayer" + std::to_string(s.id);
		GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());

		if (!netPlayer)continue;

		auto& chara = netPlayer->GetComponent<CharacterCom>();

		chara->SetUserInput(s.nextInput.input);
		chara->SetUserInputDown(s.nextInput.inputDown);
		chara->SetUserInputUp(s.nextInput.inputUp);

		s.nextInput.inputDown = 0;
		s.nextInput.inputUp = 0;
	}
}

void PhotonLib::DelayUpdate()
{
	if (saveInputPhoton.size() <= 0)return;

	int myID = GetPlayerNum();
	for (int i = 0; i < saveInputPhoton.size(); ++i)
	{
		if (myID == saveInputPhoton[i].id)continue;

		for (int j = 0; j < saveInputPhoton.size(); ++j)
		{
			if (myID != saveInputPhoton[j].id)continue;
			//先頭フレームの差を保存
			saveInputPhoton[i].myDelay = saveInputPhoton[j].inputBuf->GetHead().frame - saveInputPhoton[i].inputBuf->GetHead().frame;
			break;
		}
	}
}

int PhotonLib::GetPlayerNum()
{
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();

	return myPlayerNumber;
}

int PhotonLib::GetServerTime()
{
	int serverTime = mLoadBalancingClient.getServerTime();

	return serverTime;
}

int PhotonLib::GetServerTimeOffset()
{
	return mLoadBalancingClient.getServerTimeOffset();
}

int PhotonLib::GetRoundTripTime()
{
	return mLoadBalancingClient.getRoundTripTime();
}

int PhotonLib::GetRoundTripTimeVariance()
{
	return mLoadBalancingClient.getRoundTripTimeVariance();
}



std::vector<int> PhotonLib::GetTrips()
{
	if (saveInputPhoton.size() <= 0)return std::vector<int>();
	std::vector<int> trips;
	trips.resize(saveInputPhoton.size()-1);

	int myID = GetPlayerNum();
	int count = 0;
	for (auto& s : saveInputPhoton)
	{
		if (myID == s.id)continue;
		if (trips.size() <= count)break;
		trips[count] = s.myDelay;
		count++;
	}

	return trips;
}

int PhotonLib::GetRoomPlayersNum()
{
	auto a = mLoadBalancingClient.getCurrentlyJoinedRoom().getCustomProperties();

	int maxPlayersCount = mLoadBalancingClient.getCurrentlyJoinedRoom().getMaxPlayers();

	int count = mLoadBalancingClient.getCurrentlyJoinedRoom().getPlayerCount();
	return count;
}

std::string PhotonLib::GetRoomName()
{
	return WStringToString(mLoadBalancingClient.getCurrentlyJoinedRoom().getName().cstr());
}

int PhotonLib::SendMs()
{
	return GetServerTime() - oldMs;
}




void PhotonLib::sendData(void)
{
	ExitGames::Common::Hashtable event;

	auto& obj = GameObjectManager::Instance().Find("player");

	std::vector<NetData> n;
	NetData& netD = n.emplace_back(NetData());
	auto tra = obj->transform_->GetWorldPosition();
	netD.pos = { tra.x,tra.y,tra.z };

	auto& move = obj->GetComponent<MovementCom>();
	netD.velocity = move->GetVelocity();

	//ダメージ情報送信
	auto sendDatas = StaticSendDataManager::Instance().GetNetSendDatas();
	for (auto& data : sendDatas)
	{
		netD.damageData[data.id] += data.damage;
		data.damage = 0;
	}

	//自分の入力を送る
	int myID = GetPlayerNum();
	for (auto& s : saveInputPhoton)
	{
		if (s.id != myID)continue;

		//先頭10フレームの入力を送る
		netD.saveInputBuf = s.inputBuf->GetHeadFromSize(10);

		break;
	}



	std::stringstream s = NetDataSendCast(n);
	event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
	//自分以外全員に送信
	mLoadBalancingClient.opRaiseEvent(true, event, 0);
	//特定のナンバーに送信
	//mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));

}

void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
	Logger::Print(std::string("ls joined the game" + WStringToString(player.getName().cstr())).c_str());
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

	//入力情報保存バッファ追加
	for (int playerNum = 0; playerNum < playernrs.getSize(); ++playerNum)
	{
		//新規クライアント確認
		int newClientID = playernrs[playerNum];
		bool isNewClient = true;
		for (auto& s : saveInputPhoton)
		{
			if (s.id != newClientID)continue;

			isNewClient = false;
			break;
		}
		if (isNewClient)
		{
			//追加
			SaveInput& saveInputJoin = saveInputPhoton.emplace_back(SaveInput());
			saveInputJoin.id = newClientID;
			saveInputJoin.nextInput.oldFrame = GetServerTime() - 100;

			//今のフレームを入れる
			SaveBuffer saveBuf;
			saveBuf.frame = GetServerTime();
			saveInputJoin.inputBuf->Enqueue(saveBuf);
		}
	}
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
	switch (eventCode)
	{
	case 0:
		if (eventContent.getValue((nByte)0))
		{
			ExitGames::Common::JString s;
			s = ((ExitGames::Common::ValueObject<ExitGames::Common::JString>*)(eventContent.getValue((nByte)0)))->getDataCopy();
			auto ne = NetDataRecvCast(WStringToString(s.cstr()));

			//仮オブジェ
			std::string name = "netPlayer" + std::to_string(playerNr);
			GameObj net1 = GameObjectManager::Instance().Find(name.c_str());
			if (!net1)
			{
				//netプレイヤー
				net1 = GameObjectManager::Instance().Create();
				net1->SetName(name.c_str());
				net1->transform_->SetWorldPosition({ 0, 0, 0 });
				net1->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
				std::shared_ptr<RendererCom> r = net1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
				r->LoadModel("Data/OneCoin/robot.mdl");
				std::shared_ptr<AnimationCom> a = net1->AddComponent<AnimationCom>();
				a->PlayAnimation(0, true, false, 0.001f);
				//std::shared_ptr<TestCharacterCom> c = net1->AddComponent<TestCharacterCom>();
				std::shared_ptr<InazawaCharacterCom> c = net1->AddComponent<InazawaCharacterCom>();
				c->SetCharaID(playerNr);
				std::shared_ptr<MovementCom> m = net1->AddComponent<MovementCom>();

				std::shared_ptr<SphereColliderCom> sphere = net1->AddComponent<SphereColliderCom>();
				sphere->SetMyTag(COLLIDER_TAG::Enemy);
				sphere->SetRadius(0.5f);

				////当たり判定オブジェ(エラー直し用)
				//{
				//	//ヒットスキャン用オブジェクト
				//	GameObj collision = GameObjectManager::Instance().Create();
				//	std::shared_ptr<CapsuleColliderCom> capsule = collision->AddComponent<CapsuleColliderCom>();
				//	collision->SetName(("playerCollision" + std::to_string(playerNr)).c_str());

				//	c->SetGunFireCollision(collision);

				//}
			}

			net1->transform_->SetWorldPosition({ ne[0].pos.x,ne[0].pos.y,ne[0].pos.z });
			net1->GetComponent<MovementCom>()->SetVelocity(ne[0].velocity);

			//ダメージ情報
			for (int id = 0; id < ne[0].damageData.size(); ++id)
			{
				if (id != GetPlayerNum())continue;

				if (ne[0].damageData[id] > 0)
				{
					int a = 0;
				}

				auto& obj = GameObjectManager::Instance().Find("player");
				obj->GetComponent<CharacterCom>()->AddHitPoint(-ne[0].damageData[id]);
				break;
			}

			//入力を保存
			for (int i = ne[0].saveInputBuf.size() - 1; i >= 0; --i)
			{
				SaveBuffer newInput = ne[0].saveInputBuf[i];

				for (auto& s : saveInputPhoton)
				{
					if (s.id != playerNr)continue;
					SaveBuffer currentInput = s.inputBuf->GetHead();
					if (currentInput.frame < newInput.frame)	//新しいフレームから始める
						s.inputBuf->Enqueue(newInput);
				}
			}


		}
		break;
	default:
		break;
	}

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

void PhotonLib::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
{
	ExitGames::Common::Hashtable h;
	auto a = playerProperties.getKeys();
	if (a.getSize() > 0)
	{
		auto v = h.getValue(playerProperties.getKeys()[0]);
		int i = 0;
	}
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

void PhotonLib::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers)
{
	std::vector<ExitGames::Common::JString> v;
	for (int i = 0; i < availableRegions.getSize(); ++i)
	{
		v.emplace_back(availableRegions[i]);
		if (availableRegions[i] == ExitGames::Common::JString(L"jp"))
		{
			if (!mLoadBalancingClient.selectRegion(ExitGames::Common::JString(L"jp")))
			{
				Logger::Print("onAvailableRegions sippai");
			}
		}
	}
}
