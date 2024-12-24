#include "Photon_lib.h"
#include "limits.h"

#include "SystemStruct\Logger.h"

#include "Input\Input.h"
#include "Input\GamePad.h"

#include "../NetData.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Stage\StageEditorCom.h"

#include "PVPGameSystem/PVPGameSystem.h"

#include "StaticSendDataManager.h"

#include "imgui.h"

#include <fstream>

static const ExitGames::Common::JString appID = L"0d572336-477d-43ad-895e-59f4eeebbca9"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static const ExitGames::Common::JString PLAYER_NAME = L"user";

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

    //ネット使用名前取得
    std::ifstream ifs("Data/NETNAME.txt");
    if (ifs.fail()) {
        mpOutputListener->writeString(L"名前ファイルがありません");
        exit(0);
    }
    else
    {
        getline(ifs, netName);  //一行だけしか見ない
    }
    saveInputPhoton.resize(4);  //サイズ固定で使う
}

void PhotonLib::update(float elapsedTime)
{
    auto& myPlayer = GameObjectManager::Instance().Find("player");
    if (myPlayer)
    {
        int myPhotonID = GetMyPhotonID();
        for (auto& s : saveInputPhoton)
        {
            if (s.photonId != myPhotonID)continue;

            //自分の入力保存
            GamePad& gamePad = Input::Instance().GetGamePad();

            SaveBuffer save;
            save.frame = GetServerTime();
            save.input = gamePad.GetButton();
            save.inputDown = gamePad.GetButtonDown();
            save.inputUp = gamePad.GetButtonUp();

            //移動
            save.leftStick = gamePad.GetAxisL();
            save.pos = myPlayer->transform_->GetWorldPosition();
            save.rotato = myPlayer->transform_->GetRotation();

            //FPSカメラの向き保存
            auto& fpsCamera = myPlayer->GetChildFind("cameraPostPlayer");
            save.fpsDir = fpsCamera->transform_->GetWorldFront();

            //速力
            DirectX::XMFLOAT3 velo = myPlayer->GetComponent<MovementCom>()->GetVelocity();
            save.velo = velo;

            s.inputBuf->Enqueue(save);

            //ちーむID保存
            myPlayer->GetComponent<CharacterCom>()->GetNetCharaData().SetTeamID(s.teamID);

            //名前登録
            if (s.name.size() <= 0)
            {
                s.name = netName;
            }

            break;
        }
        int myPlayerID = GetMyPlayerID();
        myPlayer->GetComponent<CharacterCom>()->GetNetCharaData().SetNetPlayerID(myPlayerID);
    }

    switch (mState)
    {
    case PhotonState::INITIALIZED:
        mLoadBalancingClient.connect(ExitGames::LoadBalancing::ConnectOptions().setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS())).setUsername(PLAYER_NAME + GETTIMEMS()).setTryUseDatagramEncryption(true));
        mState = PhotonState::CONNECTING;
        break;
    case PhotonState::CONNECTED:
    {
        if (!connectFlg)break;
        mLoadBalancingClient.opJoinOrCreateRoom(ExitGames::Common::JString(roomName.c_str()));
        mState = PhotonState::JOINING;
        break;
    }
    case PhotonState::JOINING:
        oldMs = GetServerTime();
        break;
    case PhotonState::JOINED:
        //マスタープレイヤーID保存
        if (GetIsMasterPlayer())masterPlayerID = GetMyPlayerID();
        //情報送信
        if (GetServerTime() - oldMs > sendMs)
        {
            oldMs = GetServerTime();

            //入室申請
            if (!GetIsMasterPlayer() && !joinPermission)
            {
                sendJoinPermissionData(true);
                break;
            }

            static bool firstStartGame = false; //始めてゲーム開始に入った時
            if (isGamePlay) //ゲーム中か
            {
                //ゲーム開始時に入る
                if (!firstStartGame)
                {
                    //名前を保存
                    for (int pID = 0; pID < 4; ++pID)
                    {
                        if (saveInputPhoton[pID].useFlg)
                            savePlayerName[pID] = saveInputPhoton[pID].name;
                    }
                    //スタート時間保存
                    startTime = GetServerTime();
                }
                firstStartGame = true;
                //ゲーム中情報送信
                sendGameData();

                //ゲームモード情報
                sendGameModeData();
            }
            else
            {
                firstStartGame = false;
                //ロビー中情報送信
                sendLobbyData();
            }

            //ホスト時の処理
            if (GetIsMasterPlayer())
            {
                //入室申請がある場合
                if (joinManager.size() > 0)
                    sendJoinPermissionData(false);
            }
        }
        else
        {
            //もしoldMSに値が初期値だった場合
            if (oldMs == 0)
            {
                oldMs = GetServerTime();
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

    //追加リストにあれば追加する
    for (int i = 0; i < 4; ++i)
    {
        if (addSavePhotonID[i] >= 0)
        {
            AddPlayer(addSavePhotonID[i], i);
            addSavePhotonID[i] = -1;
        }
    }

    DelayUpdate();
    NetInputUpdate();
    MyCharaInput();
    NetCharaInput();

}


void PhotonLib::ImGui()
{
#ifdef _DEBUG
    //ネットワーク決定仮ボタン
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

    {
        //キャラ選択
        if (ImGui::TreeNode("character"))
        {
            int charaIndex = 0;
            for (auto& name : charaIDList)
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

                if (charaID == charaIndex)
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                }

                if (ImGui::TreeNodeEx(&name, nodeFlags, name.c_str()))
                {
                    if (ImGui::IsItemClicked())
                    {
                        charaID = charaIndex;

                        //キャラ変更
                        RegisterChara::Instance().ChangeChara("player",RegisterChara::CHARA_LIST(charaID));

                        ImGui::TreePop();
                        break;
                    }

                    ++charaIndex;

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }

    //ネットネーム表示
    char name[256];
    ::strncpy_s(name, sizeof(name), netName.c_str(), sizeof(name));
    ImGui::InputText("netName", name, sizeof(name));

    //ID表示
    int myPhotonID = GetMyPhotonID();
    ImGui::InputInt("photonID", &myPhotonID);

    //gamemode
    std::string mode[int(4)] =
    {
        "NONE",
        "Deathmatch",
        "Crown",
        "Button",
    };
    int modeI = 0;
    for (auto& name : mode)
    {
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

        if (gameMode == modeI)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        if (ImGui::TreeNodeEx(&name, nodeFlags, name.c_str()))
        {
            if (ImGui::IsItemClicked())
            {
                gameMode = modeI;
            }

            ++modeI;

            ImGui::TreePop();
        }
    }

    //切る数
    if (ImGui::TreeNode("killcount"))
    {
        for (auto& s : saveInputPhoton)
        {
            ImGui::InputInt(std::string(s.name).c_str(), &s.killCount);
        }
        ImGui::TreePop();
    }
    

    //マスタークライアントか
    bool isMaster = GetIsMasterPlayer();
    ImGui::Checkbox("master", &isMaster);

    if (isMaster)
    {
        //キャラセレクト
        bool isC = isCharaSelect;
        ImGui::Checkbox("isCharaSelect", &isC);
        if (isC)isCharaSelect = isC;

        //ゲーム開始
        bool g = isGamePlay;
        ImGui::Checkbox("PlayGame", &g);
        if (g)isGamePlay = g;

        //チーム分け
        if (ImGui::TreeNode("teamWake"))
        {
            for (auto& s : saveInputPhoton)
            {
                ImGui::InputInt(std::string(s.name + "  " + std::to_string(s.playerId)).c_str(), &s.teamID);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        //ゲーム開始表示のみ
        bool g = isGamePlay;
        ImGui::Checkbox("PlayGameLookOnly", &g);

        //チーム表示
        if (ImGui::TreeNode("team"))
        {
            for (auto& s : saveInputPhoton)
            {
                int tem = s.teamID;
                ImGui::InputInt(std::string(s.name + "  " + std::to_string(s.playerId)).c_str(), &tem);
            }

            ImGui::TreePop();
        }
    }

    //saveI保存プレイヤー数表示
    int saveINum = saveInputPhoton.size();
    ImGui::DragInt("saveINum", &saveINum);

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
    if (ImGui::Button("ResetTime"))
        startTime = GetServerTime();

    float time = (GetServerTime() - startTime) / 1000.0f;
    ImGui::DragFloat("time", &time);

    int roundTime = GetRoundTripTime();
    int roundTimeV = GetRoundTripTimeVariance();
    ImGui::InputInt("roundTime", &roundTime);

    //int sendMsCopy = SendMs();
    ImGui::InputInt("sendMS", &sendMs);

    ImGui::InputInt("delyaFrame", &delayFrame);

    ImGui::End();

    LobbyImGui();
    ChatImGui();
#endif
}

ExitGames::Common::JString PhotonLib::getStateString(void)
{
    switch (mState)
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
    if (!connectFlg)
    {
        //ネットワーク決定仮ボタン
        ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("PhotonNetLobby", nullptr, ImGuiWindowFlags_None);

        //新しくルーム生成
        char name[256];
        ::strncpy_s(name, sizeof(name), roomName.c_str(), sizeof(name));
        if (ImGui::InputText("roomName", name, sizeof(name)))
        {
            roomName = name;
        }
        //ネットネーム表示
        ::strncpy_s(name, sizeof(name), netName.c_str(), sizeof(name));
        if (ImGui::InputText("netName", name, sizeof(name)))
        {
            netName = name;
        }

        if (ImGui::Button("CreateOrJoinRoom"))
        {
            if (roomName.size() > 0)
                connectFlg = true;
        }

        ImGui::Separator();

        //ルームに参加
        auto rooms = mLoadBalancingClient.getRoomList();

        for (int i = 0; i < rooms.getSize(); ++i)
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            std::string selectRoom = WStringToString(rooms[i]->getName().cstr());
            if (ImGui::TreeNodeEx(&rooms[i], nodeFlags, selectRoom.c_str()))
            {
                // クリックすると選択
                if (ImGui::IsItemClicked())
                {
                    roomName = selectRoom;
                }

                ImGui::TreePop();
            }
        }

        ImGui::End();
    }
}

void PhotonLib::ChatImGui()
{
    //ネットワーク決定仮ボタン
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("ChatPhotonNet", nullptr, ImGuiWindowFlags_None);

    char name[500];
    ::strncpy_s(name, sizeof(name), chat.c_str(), sizeof(name));
    if (ImGui::InputText("roomName", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        isSendChat = true;
        chat = name;
        chatList.emplace_back(chat);
    }

    for (auto& c : chatList)
    {
        ImGui::Text(c.c_str());
    }

    ImGui::End();

}

void PhotonLib::NetInputUpdate()
{
    for (auto& s : saveInputPhoton)
    {
        //更新フラグ
        s.isInputUpdate = false;

        int nowTime = GetServerTime();

        std::vector<SaveBuffer> saveB;
        saveB = s.inputBuf->GetHeadFromSize(100);

        bool isInputInit = false;
        for (auto& b : saveB)
        {
            //前回のフレームから今回のフレームからディレイした分までの入力を保存
            if (b.frame < s.nextInput.oldFrame)break;
            if (b.frame > nowTime - delayFrame)continue;

            if (!isInputInit)	//最初に入った時に入力初期化
            {
                s.nextInput.inputDown = 0;
                s.nextInput.input = 0;
                s.nextInput.inputUp = 0;

                //移動
                s.nextInput.leftStick = b.leftStick;
                s.nextInput.pos = b.pos;
                s.nextInput.rotato = b.rotato;
                //カメラ情報
                s.nextInput.fpsCameraDir = b.fpsDir;
                //速力
                s.nextInput.velocity = b.velo;

                isInputInit = true;
            }

            s.nextInput.inputDown |= b.inputDown;
            s.nextInput.input |= b.input;
            s.nextInput.inputUp |= b.inputUp;

            s.isInputUpdate = true;
        }

        s.nextInput.oldFrame = nowTime - delayFrame;
    }
}

void PhotonLib::MyCharaInput()
{
    std::shared_ptr<GameObject> myPlayer = GameObjectManager::Instance().Find("player");
    if (myPlayer.use_count() == 0)return;

    std::shared_ptr<CharacterCom> chara = myPlayer->GetComponent<CharacterCom>();
    if (chara.use_count() == 0) return;

    GamePad& gamePad = Input::Instance().GetGamePad();

    //オフライン時処理
    if (!connectFlg)
    {
        chara->SetUserInput(gamePad.GetButton());
        chara->SetUserInputDown(gamePad.GetButtonDown());
        chara->SetUserInputUp(gamePad.GetButtonUp());

        chara->SetLeftStick(gamePad.GetAxisL());
        //カメラ情報
        auto& fpsCamera = myPlayer->GetChildFind("cameraPostPlayer");
        chara->SetFpsCameraDir(fpsCamera->transform_->GetWorldFront());
    }
    else
    {
        // 入力情報をプレイヤーキャラクターに送信
        int myPhotonID = GetMyPhotonID();
        for (auto& s : saveInputPhoton)
        {
            if (s.photonId != myPhotonID)continue;

            chara->SetUserInput(s.nextInput.input);
            chara->SetUserInputDown(s.nextInput.inputDown);
            chara->SetUserInputUp(s.nextInput.inputUp);

            chara->SetLeftStick(s.nextInput.leftStick);
            //カメラ情報
            chara->SetFpsCameraDir(s.nextInput.fpsCameraDir);

            s.nextInput.inputDown = 0;
            s.nextInput.inputUp = 0;

            break;
        }
    }

    //chara->SetLeftStick(gamePad.GetAxisL());
    chara->SetRightStick(gamePad.GetAxisR());
}

void PhotonLib::NetCharaInput()
{
    int count = 0;
    for (auto& s : saveInputPhoton)
    {
        std::string name = "netPlayer" + std::to_string(s.photonId);
        GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());

        if (!netPlayer)continue;

        auto& chara = netPlayer->GetComponent<CharacterCom>();

        chara->SetUserInput(s.nextInput.input);
        chara->SetUserInputDown(s.nextInput.inputDown);
        chara->SetUserInputUp(s.nextInput.inputUp);

        static bool upHokan[4] = { false,false,false,false }; //補間中か
        static DirectX::XMFLOAT3 hoknaPos[4] = {};  //補間する位置
        static DirectX::XMFLOAT3 nowPos[4] = {};    //今の位置
        static int saveFrameHokan[4] = { 0,0,0,0 };       //補間用フレーム
        int frameHokan = 6; //補完するフレーム

        //移動
        if (s.isInputUpdate)
        {
            hoknaPos[count] = s.nextInput.pos;
            nowPos[count] = netPlayer->transform_->GetWorldPosition();
            if (Mathf::Length(nowPos[count] - hoknaPos[count]) > 0.1f)
            {
                saveFrameHokan[count] = 1;
                upHokan[count] = true;
            }
        }

        //補間移動適用
        if (upHokan[count])
        {
            float w = float(saveFrameHokan[count]) / float(frameHokan);
            DirectX::XMFLOAT3 Hpos = Mathf::Lerp(nowPos[count], hoknaPos[count], w);
            netPlayer->transform_->SetWorldPosition(Hpos);
            saveFrameHokan[count]++;
            if (Mathf::Length(Hpos) <= 0.1f)
            {
                netPlayer->transform_->SetWorldPosition(hoknaPos[count]);
                upHokan[count] = false;
            }
            if (saveFrameHokan[count] >= frameHokan)upHokan[count] = false;
        }

        netPlayer->transform_->SetRotation(s.nextInput.rotato);
        chara->SetLeftStick(s.nextInput.leftStick);
        //カメラ情報
        chara->SetFpsCameraDir(s.nextInput.fpsCameraDir);

        //速力
        netPlayer->GetComponent<MovementCom>()->SetVelocity(s.nextInput.velocity);

        s.nextInput.inputDown = 0;
        s.nextInput.inputUp = 0;

        count++;
    }
}

void PhotonLib::DelayUpdate()
{
    if (saveInputPhoton.size() <= 0)return;

    int myPhotonID = GetMyPhotonID();
    delayFrame = 0;

    for (int i = 0; i < saveInputPhoton.size(); ++i)    //自分以外
    {
        if (myPhotonID == saveInputPhoton[i].photonId)continue;
        if (!saveInputPhoton[i].useFlg)continue;

        for (int j = 0; j < saveInputPhoton.size(); ++j)    //自分を探す
        {
            if (myPhotonID != saveInputPhoton[j].photonId)continue;
            //先頭フレームの差を保存
            int d = saveInputPhoton[j].inputBuf->GetHead().frame - saveInputPhoton[i].inputBuf->GetHead().frame;
            if (d > delayFrame) //一番大きい遅延を保存
            {
                delayFrame = d;
            }
            break;
        }
    }
}

#pragma region ゲッターセッター

int PhotonLib::GetKillCount(int team)
{
    int count = 0;
    for (auto& s : saveInputPhoton)
    {
        if (s.teamID == team)
            count += s.killCount;
    }
    return count;
}

int PhotonLib::GetMyPhotonID()
{
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();

    return myPlayerNumber;
}

int PhotonLib::GetMyPlayerID()
{
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId == GetMyPhotonID())
            return s.playerId;
    }
    return -1;
}

bool PhotonLib::GetIsMasterPlayer()
{
    bool isMaster = mLoadBalancingClient.getLocalPlayer().getIsMasterClient();

    return isMaster;
}

float PhotonLib::GetNowTime()
{
    return (GetServerTime() - startTime) / 1000.0f;
}

float PhotonLib::GetJoinNum()
{
    int useC = 0;
    for (auto& s : saveInputPhoton)
    {
        if (s.useFlg)useC++;
    }
    return useC;
}

void PhotonLib::SetTeamID(int teamID, int playerID)
{
    saveInputPhoton[playerID].teamID = teamID;
}

int PhotonLib::GetTeamID(int playerID)
{
    return saveInputPhoton[playerID].teamID;
}

void PhotonLib::SetMyPickCharaID(int pickChara)
{
    int myPlayerID = GetMyPlayerID();
    if (myPlayerID >= 0)
        saveInputPhoton[myPlayerID].charaID = pickChara;
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

std::vector<std::wstring> PhotonLib::GetRoomNames()
{
    std::vector<std::wstring> roomnames;
    auto rooms = mLoadBalancingClient.getRoomList();
    for (int i = 0; i < rooms.getSize(); ++i)
    {
        roomnames.emplace_back(rooms[i]->getName().cstr());
    }

    return roomnames;
}

int PhotonLib::SendMs()
{
    return GetServerTime() - oldMs;
}

#pragma endregion

#pragma region ライブラリ関数

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
    if (errorCode)
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
    if (errorCode)
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
    if (errorCode)
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
    if (errorCode)
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
    if (errorCode)
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
    if (errorCode)
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


#pragma endregion

///////////////↓↓↓↓↓           ↓よく使う関数↓           ↓↓↓↓↓///////////////

//入室時
void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
    Logger::Print(std::string("ls joined the game" + WStringToString(player.getName().cstr())).c_str());
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
    mpOutputListener->writeString(L"");
    mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

    //自分の枠を確保
    int myPhotonID = GetMyPhotonID();
    if (playerNr == myPhotonID)
    {
        //追加
        if (GetIsMasterPlayer())
        {
            AddPlayer(myPhotonID, 0);
            saveInputPhoton[0].name = netName;
        }
    }
}
//退出時
void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    mpOutputListener->writeString(L"");
    mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");

    //退出者処理
    for (int i = 0; i < saveInputPhoton.size(); ++i)
    {
        if (saveInputPhoton[i].photonId != playerNr)continue;

        std::string name = "netPlayer" + std::to_string(playerNr);
        GameObj net1 = GameObjectManager::Instance().Find(name.c_str());
        if (net1)GameObjectManager::Instance().Remove(net1);

        saveInputPhoton[i].useFlg = false;

        //saveInputPhoton.erase(saveInputPhoton.begin() + i);
        break;
    }
}

//受信
void PhotonLib::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
    ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
    switch (eventCode)
    {
    case 0:
        if (eventContent.getValue((nByte)0))
        {
            ExitGames::Common::JString jsString;
            jsString = ((ExitGames::Common::ValueObject<ExitGames::Common::JString>*)(eventContent.getValue((nByte)0)))->getDataCopy();
            //データ変換
            auto ne = NetDataRecvCast(WStringToString(jsString.cstr()));


            if (ne[0].playerId >= 0)
            {
                //名前保存
                saveInputPhoton[ne[0].playerId].name = ne[0].name;

                //マスタープレイヤーID
                if (ne[0].isMasterClient)
                {
                    masterPlayerID = ne[0].playerId;
                }
            }
                //for (auto& s : saveInputPhoton)
                //{
                //    if (s.photonId != ne[0].photonId)continue;

                //    if (s.name.size() <= 0)
                //    {
                //        s.name = ne[0].name;
                //    }
                //    break;
                //}

            //ゲームモード保存
            if (!GetIsMasterPlayer())
            {
                gameMode = ne[0].gameMode;
            }

            connectNow = connectBegin;
            connectBegin = false;

            //データ種別で分岐
            switch (ne[0].dataKind)
            {
            case NetData::DATA_KIND::GAME:
                if (joinPermission || GetIsMasterPlayer())
                    GameRecv(ne[0]);
                break;

            case NetData::DATA_KIND::JOIN:
                JoinRecv(ne[0]);
                break;

            case NetData::DATA_KIND::LOBBY:
                if (joinPermission || GetIsMasterPlayer())
                    LobbyRecv(ne[0]);
                break;
            case NetData::DATA_KIND::DEATHMATCH:
                if (isGamePlay)
                    DeathMatchRecv(ne[0]);
                break;
            }
        }
        break;
    default:
        break;
    }
}

void PhotonLib::GameRecv(NetData recvData)
{
    //キャラ追加
    bool add = true;
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId == recvData.photonId)add = false;
    }
    //キャラ追加リストに追加
    if (add)addSavePhotonID[recvData.playerId] = recvData.photonId;

    //マスタークライアントからの受信の場合
    if (recvData.isMasterClient)
    {
        //チームを保存
        for (int pId = 0; pId < 4; ++pId)
        {
            saveInputPhoton[pId].teamID = recvData.gameData.teamID[pId];
        }
    }
    //ゲーム開始フラグ
    isGamePlay = true;

    //マスタークライアント以外はチームを保存
    if (recvData.isMasterClient)
    {
        //タイマー合わせる
        startTime = recvData.gameData.startTime;
    }

    //仮オブジェ
    std::string name = "netPlayer" + std::to_string(recvData.photonId);
    GameObj net1 = GameObjectManager::Instance().Find(name.c_str());

    //プレイヤー追加
    if (!net1)
    {
        //AddPlayer(recvData.photonId, recvData.playerId);

        //netプレイヤー
        net1 = GameObjectManager::Instance().Create();
        net1->SetName(name.c_str());

        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(recvData.gameData.charaID), net1);
        net1->GetComponent<CharacterCom>()->GetNetCharaData().SetNetPlayerID(recvData.playerId);
    }
    saveInputPhoton[recvData.playerId].charaID = recvData.gameData.charaID;

    //hp
    net1->GetComponent<CharaStatusCom>()->SetHitPoint(recvData.gameData.hp);

    int myPlayerID = GetMyPlayerID();
    auto& myPlayer = GameObjectManager::Instance().Find("player");
    if (!myPlayer)return;

    //ダメージ情報
    for (int id = 0; id < recvData.gameData.damageData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.damageData[id] > 0)
        {
            myPlayer->GetComponent<CharaStatusCom>()->AddDamagePoint(-recvData.gameData.damageData[id]);
            break;
        }
    }
    //ヒール情報
    for (int id = 0; id < recvData.gameData.healData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.healData[id] > 0)
        {
            myPlayer->GetComponent<CharaStatusCom>()->AddHealPoint(recvData.gameData.healData[id]);
            break;
        }
    }
    //スタン情報
    for (int id = 0; id < recvData.gameData.stanData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.stanData[id] >= 0.1f)
        {
            myPlayer->GetComponent<CharacterCom>()->SetStanSeconds(recvData.gameData.stanData[id]);
            break;
        }
    }
    //ノックバック情報
    for (int id = 0; id < recvData.gameData.knockbackData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (Mathf::Length(recvData.gameData.knockbackData[id]) >= 0.1f)
        {
            myPlayer->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(recvData.gameData.knockbackData[id]);
            break;
        }
    }
    //移動位置情報
    for (int id = 0; id < recvData.gameData.movePosData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (Mathf::Length(recvData.gameData.movePosData[id]) >= 0.1f)
        {
            myPlayer->transform_->SetWorldPosition(recvData.gameData.movePosData[id]);
            break;
        }
    }

    //保存情報
        //入力
    for (int i = recvData.gameData.saveInputBuf.size() - 1; i >= 0; --i)
    {
        SaveBuffer newInput = recvData.gameData.saveInputBuf[i];

        SaveBuffer currentInput = saveInputPhoton[recvData.playerId].inputBuf->GetHead();
        if (currentInput.frame < newInput.frame)	//新しいフレームから始める
            saveInputPhoton[recvData.playerId].inputBuf->Enqueue(newInput);
    }
    ////保存情報
    //for (auto& s : saveInputPhoton)
    //{
    //    if (s.photonId != recvData.photonId)continue;
    //    //入力
    //    for (int i = recvData.gameData.saveInputBuf.size() - 1; i >= 0; --i)
    //    {
    //        SaveBuffer newInput = recvData.gameData.saveInputBuf[i];

    //        SaveBuffer currentInput = s.inputBuf->GetHead();
    //        if (currentInput.frame < newInput.frame)	//新しいフレームから始める
    //            s.inputBuf->Enqueue(newInput);
    //    }
    //    break;
    //}
}

void PhotonLib::JoinRecv(NetData recvData)
{
    if (GetIsMasterPlayer()) //ホストの場合
    {
        //リクエストがあれば全部リストに追加
        for (auto& j : recvData.joinData)
        {
            if (j.joinRequest)
            {
                auto& joinM = joinManager.emplace_back();
                joinM.jData.photonId = recvData.photonId;
                joinM.joinName = recvData.name;
            }
        }
    }
    else    //ホスト以外
    {
        //入室許可を見て参加する
        for (auto& j : recvData.joinData)
        {
            //自分だけの処理
            if (j.photonId == GetMyPhotonID())
            {
                //入室許可が下りない場合
                if (!j.joinPermission)
                {
                    mLoadBalancingClient.disconnect();
                    mState = PhotonState::CONNECTED;
                    connectFlg = false;

                    //退出者処理
                    for (int i = 0; i < saveInputPhoton.size(); ++i)
                    {
                        std::string name = "netPlayer" + std::to_string(saveInputPhoton[i].photonId);
                        GameObj net1 = GameObjectManager::Instance().Find(name.c_str());
                        if (net1)GameObjectManager::Instance().Remove(net1);
                    }

                    saveInputPhoton.clear();
                    return;
                }

                //入室処理
                joinPermission = true;  //入室
                //for (auto& s : saveInputPhoton) //自分のプレイヤーID決定
                //{
                //    if (s.photonId == GetMyPhotonID())
                //    {
                        //s.playerId = j.playerId;
                addSavePhotonID[j.playerId] = GetMyPhotonID();
                //        break;
                //    }
                //}

                //名前登録
                saveInputPhoton[j.playerId].name = netName;

            }
        }
    }
}

void PhotonLib::LobbyRecv(NetData recvData)
{
    //キャラ追加
    bool add = true;
    for (auto& s : saveInputPhoton)
    {
        //登録済みなら追加しない
        if (s.photonId == recvData.photonId)add = false;
    }
    //キャラ追加リストに追加
    if (add)addSavePhotonID[recvData.playerId] = recvData.photonId;
    
    //AddPlayer(recvData.photonId, recvData.playerId);

    saveInputPhoton[recvData.playerId].charaID = recvData.lobbyData.charaID;

    //ピック選択に移行
    if (recvData.isMasterClient)
    {
        if (recvData.lobbyData.pickSelect == 1)
            isCharaSelect = true;
    }
    charaState[recvData.playerId] = recvData.lobbyData.pickSelect;

    //マスタークライアントからの受信の場合
    if (recvData.isMasterClient)
    {
        //チームを保存
        for (int pID = 0; pID < 4; ++pID)
            saveInputPhoton[pID].teamID = recvData.lobbyData.teamID[pID];
    }

    //チャットに文字が入っているなら
    if (((std::string)recvData.lobbyData.chat) != "0")
    {
        chatList.emplace_back(recvData.lobbyData.chat);
    }
}

void PhotonLib::DeathMatchRecv(NetData recvData)
{
    saveInputPhoton[recvData.playerId].killCount = recvData.deathMatchData.killCount;
    //for (auto& s : saveInputPhoton)
    //{
    //    if (s.playerId == recvData.playerId)
    //    {
    //        s.killCount = recvData.deathMatchData.killCount;
    //        break;
    //    }
    //}
}

//送信
void PhotonLib::sendGameData(void)
{
    ExitGames::Common::Hashtable event;

    auto& myPlayer = GameObjectManager::Instance().Find("player");
    if (!myPlayer)return;

    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());

    //種別をゲームに
    netD.dataKind = NetData::DATA_KIND::GAME;

    //マスタークライアントか
    netD.isMasterClient = GetIsMasterPlayer();

    //ID
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;

    //ID
    int myPlayerID = GetMyPlayerID();
    netD.playerId = myPlayerID;

    //マスタークライアントの場合
    if (GetIsMasterPlayer())
    {
        //チームIDを送る
        for (int i = 0; i < 4; ++i)
            netD.gameData.teamID[i] = saveInputPhoton[i].teamID;
    }

    //名前
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));

    //gamemode
    netD.gameMode = gameMode;

    //HP
    netD.gameData.hp = int(*myPlayer->GetComponent<CharaStatusCom>()->GetHitPoint());

    //ダメージ情報送信
    auto sendDatas = StaticSendDataManager::Instance().GetNetSendDatas();
    for (auto& data : sendDatas)
    {
        if (data.sendType == 0)	//ダメージ
            netD.gameData.damageData[data.playerID] += data.valueI;
        else if (data.sendType == 1)	//ヒール
            netD.gameData.healData[data.playerID] += data.valueI;
        else if (data.sendType == 2)	//スタン
        {
            //一番長いスタン時間を与える
            if (netD.gameData.stanData[data.playerID] < data.valueF)
                netD.gameData.stanData[data.playerID] = data.valueF;
        }
        else if (data.sendType == 3)	//ノックバック
            netD.gameData.knockbackData[data.playerID] += data.valueF3;
        else if (data.sendType == 4)	//移動位置
            netD.gameData.movePosData[data.playerID] = data.valueF3;
    }

    //キャラIDを送る
    netD.gameData.charaID = myPlayer->GetComponent<CharacterCom>()->GetNetCharaData().GetCharaID();

    //自分の入力を送る
    //先頭20フレームの入力を送る
    netD.gameData.saveInputBuf = saveInputPhoton[myPlayerID].inputBuf->GetHeadFromSize(20);
    //for (auto& s : saveInputPhoton)
    //{
    //    if (s.photonId != myPhotonID)continue;

    //    //先頭20フレームの入力を送る
    //    netD.gameData.saveInputBuf = saveInputPhoton[myPlayerID].inputBuf->GetHeadFromSize(20);

    //    break;
    //}

    //タイマー
    netD.gameData.startTime = startTime;

    std::stringstream s = NetDataSendCast(n);
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //自分以外全員に送信
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //特定のナンバーに送信
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

//入室許可送信(申請の場合はtrue)
void PhotonLib::sendJoinPermissionData(bool request)
{
    ExitGames::Common::Hashtable event;
    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;
    netD.playerId = -1;
    netD.isMasterClient = GetIsMasterPlayer();
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));

    //種別を入室に
    netD.dataKind = NetData::DATA_KIND::JOIN;

    if (request)    //入室申請
    {
        auto& join = netD.joinData.emplace_back();
        join.joinRequest = true;    //入室申請

        //ここは適当
        join.photonId = -1;
        join.playerId = -1;
        join.joinPermission = false;
    }
    else    //ホストの処理
    {
        //重複を防ぐ
        std::vector<int> joinList;
        for (auto& j : joinManager) //申請リストから審議
        {
            //重複を防ぐ
            bool continueFlg = false;
            for (auto& jIn : joinList)
            {
                if (j.jData.photonId == jIn)
                    continueFlg = true;
            }
            if (continueFlg)continue;;
            joinList.emplace_back(j.jData.photonId);

            //データ送信
            auto& join = netD.joinData.emplace_back();
            join.photonId = j.jData.photonId;
            join.playerId = j.jData.playerId;
            join.joinPermission = false;
            join.joinRequest =false;
            //４人まで追加
            int useCount = GetJoinNum();
            if (useCount < 4)
            {
                if (isGamePlay)  //プレイ中の場合
                {
                    for (int sP = 0; sP < 4; ++sP)
                    {
                        if (savePlayerName[sP] != j.joinName)continue;

                        join.joinPermission = true; //入室を許可
                        join.playerId = sP;
                        break;
                    }
                }
                else    //ロビーの場合
                {
                    join.joinPermission = true; //入室を許可

                    for (int openID = 0; openID < 4; ++openID)
                    {
                        //使用されていない番号を探す
                        if (!saveInputPhoton[openID].useFlg)
                        {
                            join.playerId = openID;
                            break;
                        }
                    }
                }
            }
        }
        joinManager.clear();
    }

    std::stringstream s = NetDataSendCast(n);
    auto ne = NetDataRecvCast(s.str());
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //自分以外全員に送信
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //特定のナンバーに送信
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));

}

void PhotonLib::sendLobbyData(void)
{
    ExitGames::Common::Hashtable event;
    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());
    //ID
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;
    //ID
    int myPlayerID = GetMyPlayerID();
    netD.playerId = myPlayerID;
    netD.isMasterClient = GetIsMasterPlayer();
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));

    //gamemode
    netD.gameMode = gameMode;

    //種別をロビーに
    netD.dataKind = NetData::DATA_KIND::LOBBY;

    //キャラIDを送る
    netD.lobbyData.charaID = saveInputPhoton[myPlayerID].charaID;

    //マスタークライアントの場合
    if (GetIsMasterPlayer())
    {
        //チームIDを送る
        for (int i = 0; i < 4; ++i)
            netD.lobbyData.teamID[i] = saveInputPhoton[i].teamID;
    }

    netD.lobbyData.pickSelect = charaState[myPlayerID];

    //チャットを送る
    ::strncpy_s(netD.lobbyData.chat, sizeof(netD.lobbyData.chat), "0", sizeof(netD.lobbyData.chat));
    if (isSendChat)
    {
        ::strncpy_s(netD.lobbyData.chat, sizeof(netD.lobbyData.chat), chat.c_str(), sizeof(netD.lobbyData.chat));
        isSendChat = false;
        chat.clear();
    }

    std::stringstream s = NetDataSendCast(n);
    auto ne = NetDataRecvCast(s.str());
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //自分以外全員に送信
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //特定のナンバーに送信
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

void PhotonLib::sendGameModeData(void)
{
    switch (gameMode)
    {
        case int(PVPGameSystem::GAME_MODE::Deathmatch) :
            sendDeathMatchData();
            break;
            case int(PVPGameSystem::GAME_MODE::Crown) :
                break;
                case int(PVPGameSystem::GAME_MODE::Button) :
                    break;
    }
}

void PhotonLib::sendDeathMatchData(void)
{
    ExitGames::Common::Hashtable event;
    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());
    //ID
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;
    //ID
    int myPlayerID = GetMyPlayerID();
    netD.playerId = myPlayerID;
    netD.isMasterClient = GetIsMasterPlayer();
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));

    //gamemode
    netD.gameMode = gameMode;

    //種別をデスマッチに
    netD.dataKind = NetData::DATA_KIND::DEATHMATCH;

    //切る数取得
    netD.deathMatchData.killCount = saveInputPhoton[myPlayerID].killCount;
    //for (auto& s : saveInputPhoton)
    //{
    //    if (s.playerId == myPlayerID)
    //    {
    //        netD.deathMatchData.killCount = s.killCount;
    //        break;
    //    }
    //}

    std::stringstream s = NetDataSendCast(n);
    auto ne = NetDataRecvCast(s.str());
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //自分以外全員に送信
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //特定のナンバーに送信
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

//プレイヤー追加
void PhotonLib::AddPlayer(int photonID, int playerID)
{
    ////重複処理
    //bool dabu = false;
    //for (auto& s : saveInputPhoton)
    //{
    //    if (s.photonId == photonID)dabu = true;
    //}
    //if (dabu)return;

    //追加
    //SaveInput& saveInputJoin = saveInputPhoton.emplace_back(SaveInput());
    //saveInputJoin.photonId = photonID;

    //プレイヤーID決定
    //int pID = 0;
    //if (GetIsMasterPlayer() && photonID == GetMyPhotonID())
    //{
    //    saveInputPhoton[0].playerId = 0;
    //    pID = 0;
    //}
    //else
    //{
        saveInputPhoton[playerID].playerId = playerID;
    //    pID = playerID;
    //}
    ////プレイヤーID決定
    //if (GetIsMasterPlayer() && photonID == GetMyPhotonID())
    //    saveInputJoin.playerId = 0;
    //else
    //    saveInputJoin.playerId = playerID;

    //少し古いフレームを保存
    saveInputPhoton[playerID].nextInput.oldFrame = GetServerTime() - 100;
    //saveInputJoin.nextInput.oldFrame = GetServerTime() - 100;

    //今のフレームを入れる
    SaveBuffer saveBuf;
    saveBuf.frame = GetServerTime();
    saveInputPhoton[playerID].inputBuf->Enqueue(saveBuf);

    saveInputPhoton[playerID].useFlg = true;
    saveInputPhoton[playerID].photonId = photonID;
}