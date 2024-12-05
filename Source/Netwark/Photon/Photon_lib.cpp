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
    //��l�����ŃN���C�A���g�̃p�����[�^�[�E�ݒ�����Ă���
    , mLoadBalancingClient(*this, appID, appVersion, ExitGames::LoadBalancing::ClientConstructOptions(0U, true, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, true))
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
    //�f�o�b�O�o�̓��x��
    mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
    mLogger.setListener(*this);
    mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
    ExitGames::Common::Base::setListener(this);
    ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base

    //�l�b�g�g�p���O�擾
    std::ifstream ifs("Data/NETNAME.txt");
    if (ifs.fail()) {
        mpOutputListener->writeString(L"���O�t�@�C��������܂���");
        exit(0);
    }
    else
    {
        getline(ifs, netName);  //��s�����������Ȃ�
    }
}

void PhotonLib::update(float elapsedTime)
{
    auto& myPlayer = GameObjectManager::Instance().Find("player");
    int myPhotonID = GetMyPhotonID();
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId != myPhotonID)continue;

        //�����̓��͕ۑ�
        GamePad& gamePad = Input::Instance().GetGamePad();

        SaveBuffer save;
        save.frame = GetServerTime();
        save.input = gamePad.GetButton();
        save.inputDown = gamePad.GetButtonDown();
        save.inputUp = gamePad.GetButtonUp();

        //�ړ�
        save.leftStick = gamePad.GetAxisL();
        save.pos = myPlayer->transform_->GetWorldPosition();
        save.rotato = myPlayer->transform_->GetRotation();

        //FPS�J�����̌����ۑ�
        auto& fpsCamera = myPlayer->GetChildFind("cameraPostPlayer");
        save.fpsDir = fpsCamera->transform_->GetWorldFront();

        //����
        DirectX::XMFLOAT3 velo = myPlayer->GetComponent<MovementCom>()->GetVelocity();
        save.velo = velo;

        s.inputBuf->Enqueue(save);

        //���[��ID�ۑ�
        myPlayer->GetComponent<CharacterCom>()->GetNetCharaData().SetTeamID(s.teamID);

        //���O�o�^
        if (s.name.size() <= 0)
        {
            s.name = netName;
        }

        break;
    }
    myPlayer->GetComponent<CharacterCom>()->GetNetCharaData().SetNetID(myPhotonID);

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
        //��񑗐M
        if (GetServerTime() - oldMs > sendMs)
        {
            oldMs = GetServerTime();

            //�����\��
            if (!GetIsMasterPlayer() && !joinPermission)
            {
                sendJoinPermissionData(true);
                break;
            }

            //�Q�[������񑗐M
            sendGameData();

            //�z�X�g���̏���
            if (GetIsMasterPlayer())
            {
                //�����\��������ꍇ
                if (joinManager.size() > 0)
                    sendJoinPermissionData(false);
            }
        }
        else
        {
            //����oldMS�ɒl�������l�������ꍇ
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

    DelayUpdate();
    NetInputUpdate();
    MyCharaInput();
    NetCharaInput();

}


void PhotonLib::ImGui()
{
#ifdef _DEBUG
    //�l�b�g���[�N���艼�{�^��
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

    //�l�b�g�l�[���\��
    char name[256];
    ::strncpy_s(name, sizeof(name), netName.c_str(), sizeof(name));
    ImGui::InputText("netName", name, sizeof(name));

    //ID�\��
    int myPhotonID = GetMyPhotonID();
    ImGui::InputInt("photonID", &myPhotonID);

    //�}�X�^�[�N���C�A���g��
    bool isMaster = GetIsMasterPlayer();
    ImGui::Checkbox("master", &isMaster);

    if (isMaster)
    {
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

    //��ԕ\��
    std::string nowState = stateStr[GetPhotonState()];
    ImGui::Text(("State : " + nowState).c_str());

    ImGui::Separator();

    //���[�����O
    ImGui::Text(("RoomName : " + GetRoomName()).c_str());

    //���[���l���\��
    int roomCount = GetRoomPlayersNum();
    ImGui::InputInt("roomPlayersNum", &roomCount);

    //�T�[�o�[����
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
        //�l�b�g���[�N���艼�{�^��
        ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("PhotonNetLobby", nullptr, ImGuiWindowFlags_None);

        //�L�����I��
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

                ImGui::TreeNodeEx(&name, nodeFlags, name.c_str());

                if (ImGui::IsItemClicked())
                {
                    charaID = charaIndex;
                }

                ++charaIndex;

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        //�V�������[������
        char name[256];
        ::strncpy_s(name, sizeof(name), roomName.c_str(), sizeof(name));
        if (ImGui::InputText("roomName", name, sizeof(name)))
        {
            roomName = name;
        }
        //�l�b�g�l�[���\��
        ::strncpy_s(name, sizeof(name), netName.c_str(), sizeof(name));
        if(ImGui::InputText("netName", name, sizeof(name)))
        {
            netName = name;
        }

        if (ImGui::Button("CreateOrJoinRoom"))
        {
            if (roomName.size() > 0)
                connectFlg = true;
        }

        ImGui::Separator();

        //���[���ɎQ��
        auto rooms = mLoadBalancingClient.getRoomList();

        for (int i = 0; i < rooms.getSize(); ++i)
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            std::string selectRoom = WStringToString(rooms[i]->getName().cstr());
            if (ImGui::TreeNodeEx(&rooms[i], nodeFlags, selectRoom.c_str()))
            {

                // �N���b�N����ƑI��
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

void PhotonLib::NetInputUpdate()
{
    for (auto& s : saveInputPhoton)
    {
        //�X�V�t���O
        s.isInputUpdate = false;

        int nowTime = GetServerTime();

        std::vector<SaveBuffer> saveB;
        saveB = s.inputBuf->GetHeadFromSize(100);

        bool isInputInit = false;
        for (auto& b : saveB)
        {
            //�O��̃t���[�����獡��̃t���[������f�B���C�������܂ł̓��͂�ۑ�
            if (b.frame < s.nextInput.oldFrame)break;
            if (b.frame > nowTime - delayFrame)continue;

            if (!isInputInit)	//�ŏ��ɓ��������ɓ��͏�����
            {
                s.nextInput.inputDown = 0;
                s.nextInput.input = 0;
                s.nextInput.inputUp = 0;

                //�ړ�
                s.nextInput.leftStick = b.leftStick;
                s.nextInput.pos = b.pos;
                s.nextInput.rotato = b.rotato;
                //�J�������
                s.nextInput.fpsCameraDir = b.fpsDir;
                //����
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
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
    if (obj.use_count() == 0)return;

    std::shared_ptr<CharacterCom> chara = obj->GetComponent<CharacterCom>();
    if (chara.use_count() == 0) return;

    GamePad& gamePad = Input::Instance().GetGamePad();

    //�I�t���C��������
    if (!connectFlg)
    {
        chara->SetUserInput(gamePad.GetButton());
        chara->SetUserInputDown(gamePad.GetButtonDown());
        chara->SetUserInputUp(gamePad.GetButtonUp());

        chara->SetLeftStick(gamePad.GetAxisL());
        //�J�������
        auto& fpsCamera = obj->GetChildFind("cameraPostPlayer");
        chara->SetFpsCameraDir(fpsCamera->transform_->GetWorldFront());
    }
    else
    {
        // ���͏����v���C���[�L�����N�^�[�ɑ��M
        int myPhotonID = GetMyPhotonID();
        for (auto& s : saveInputPhoton)
        {
            if (s.photonId != myPhotonID)continue;

            chara->SetUserInput(s.nextInput.input);
            chara->SetUserInputDown(s.nextInput.inputDown);
            chara->SetUserInputUp(s.nextInput.inputUp);

            chara->SetLeftStick(s.nextInput.leftStick);
            //�J�������
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

        static bool upHokan[4] = { false }; //��Ԓ���
        static DirectX::XMFLOAT3 hoknaPos[4] = {};  //��Ԃ���ʒu
        static DirectX::XMFLOAT3 nowPos[4] = {};    //���̈ʒu
        static int saveFrameHokan[4] = { 0 };       //��ԗp�t���[��
        int frameHokan = 6; //�⊮����t���[��

        //�ړ�
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

        //��Ԉړ��K�p
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
        //�J�������
        chara->SetFpsCameraDir(s.nextInput.fpsCameraDir);

        //����
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

    for (int i = 0; i < saveInputPhoton.size(); ++i)    //�����ȊO
    {
        if (myPhotonID == saveInputPhoton[i].photonId)continue;

        for (int j = 0; j < saveInputPhoton.size(); ++j)    //������T��
        {
            if (myPhotonID != saveInputPhoton[j].photonId)continue;
            //�擪�t���[���̍���ۑ�
            int d = saveInputPhoton[j].inputBuf->GetHead().frame - saveInputPhoton[i].inputBuf->GetHead().frame;
            if (d > delayFrame) //��ԑ傫���x����ۑ�
            {
                delayFrame = d;
            }
            break;
        }
    }
}



int PhotonLib::GetMyPhotonID()
{
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();

    return myPlayerNumber;
}

bool PhotonLib::GetIsMasterPlayer()
{
    bool isMaster = mLoadBalancingClient.getLocalPlayer().getIsMasterClient();

    return isMaster;
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

int PhotonLib::SendMs()
{
    return GetServerTime() - oldMs;
}


#pragma region ���C�u�����֐�

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

///////////////����������           ���悭�g���֐���           ����������///////////////


//������
void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
    Logger::Print(std::string("ls joined the game" + WStringToString(player.getName().cstr())).c_str());
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
    mpOutputListener->writeString(L"");
    mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

    //�����̘g���m��
    int myPhotonID = GetMyPhotonID();
    if (playerNr == myPhotonID)
    {
        //�ǉ�
        AddPlayer(myPhotonID, -1);
    }
}
//�ޏo��
void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    mpOutputListener->writeString(L"");
    mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");

    //�ޏo�ҏ���
    for (int i = 0; i < saveInputPhoton.size(); ++i)
    {
        if (saveInputPhoton[i].photonId != playerNr)continue;

        std::string name = "netPlayer" + std::to_string(playerNr);
        GameObj net1 = GameObjectManager::Instance().Find(name.c_str());
        if (net1)GameObjectManager::Instance().Remove(net1);

        saveInputPhoton.erase(saveInputPhoton.begin() + i);
        break;
    }
}

//��M
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
            //�f�[�^�ϊ�
            auto ne = NetDataRecvCast(WStringToString(jsString.cstr()));

            //���O�ۑ�
            for (auto& s : saveInputPhoton)
            {
                if (s.photonId != ne[0].photonId)continue;

                if (s.name.size() <= 0)
                {
                    s.name = ne[0].name;
                }
                break;
            }

            //�f�[�^��ʂŕ���
            switch (ne[0].dataKind)
            {
            case NetData::DATA_KIND::GAME:
                GameRecv(ne[0]);
                break;

            case NetData::DATA_KIND::JOIN:
                JoinRecv(ne[0]);
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
    //�}�X�^�[�N���C�A���g�ȊO�̓`�[����ۑ�
    if (recvData.isMasterClient)
    {
        for (auto& s : saveInputPhoton)
        {
            s.teamID = recvData.gameData.teamID[s.playerId];
        }
        startTime = recvData.gameData.startTime;
    }

    //���I�u�W�F
    std::string name = "netPlayer" + std::to_string(recvData.photonId);
    GameObj net1 = GameObjectManager::Instance().Find(name.c_str());

    //�v���C���[�ǉ�
    if (!net1)
    {
        AddPlayer(recvData.photonId, recvData.playerId);

        //net�v���C���[
        net1 = GameObjectManager::Instance().Create();
        net1->SetName(name.c_str());

        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(recvData.gameData.charaID), net1);
        net1->GetComponent<CharacterCom>()->GetNetCharaData().SetNetID(recvData.photonId);
    }

    //�_���[�W���
    for (int id = 0; id < recvData.gameData.damageData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.damageData[id] > 0)
        {
            auto& obj = GameObjectManager::Instance().Find("player");
            obj->GetComponent<CharaStatusCom>()->AddDamagePoint(-recvData.gameData.damageData[id]);
            break;
        }
    }
    //�q�[�����
    for (int id = 0; id < recvData.gameData.healData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.healData[id] > 0)
        {
            auto& obj = GameObjectManager::Instance().Find("player");
            obj->GetComponent<CharaStatusCom>()->AddHealPoint(recvData.gameData.healData[id]);
            break;
        }
    }
    //�X�^�����
    for (int id = 0; id < recvData.gameData.stanData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (recvData.gameData.stanData[id] >= 0.1f)
        {
            auto& obj = GameObjectManager::Instance().Find("player");
            obj->GetComponent<CharacterCom>()->SetStanSeconds(recvData.gameData.stanData[id]);
            break;
        }
    }
    //�m�b�N�o�b�N���
    for (int id = 0; id < recvData.gameData.knockbackData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (Mathf::Length(recvData.gameData.knockbackData[id]) >= 0.1f)
        {
            auto& obj = GameObjectManager::Instance().Find("player");
            obj->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(recvData.gameData.knockbackData[id]);
            break;
        }
    }
    //�ړ��ʒu���
    for (int id = 0; id < recvData.gameData.movePosData.size(); ++id)
    {
        if (id != myPlayerID)continue;

        if (Mathf::Length(recvData.gameData.movePosData[id]) >= 0.1f)
        {
            auto& obj = GameObjectManager::Instance().Find("player");
            obj->transform_->SetWorldPosition(recvData.gameData.movePosData[id]);
            break;
        }
    }

    //���͂�ۑ�
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId != recvData.photonId)continue;
        for (int i = recvData.gameData.saveInputBuf.size() - 1; i >= 0; --i)
        {
            SaveBuffer newInput = recvData.gameData.saveInputBuf[i];

            SaveBuffer currentInput = s.inputBuf->GetHead();
            if (currentInput.frame < newInput.frame)	//�V�����t���[������n�߂�
                s.inputBuf->Enqueue(newInput);
        }
        break;
    }
}

void PhotonLib::JoinRecv(NetData recvData)
{
    if (GetIsMasterPlayer()) //�z�X�g�̏ꍇ
    {
        //���N�G�X�g������ΑS�����X�g�ɒǉ�
        for (auto& j : recvData.joinData)
        {
            if (j.joinRequest)
            {
                auto& joinM = joinManager.emplace_back();
                joinM.photonId = recvData.photonId;
            }
        }
    }
    else    //�z�X�g�ȊO
    {
        //�����������ă��X�g�ɒǉ�
        for (auto& j : recvData.joinData)
        {
            //���������̏���
            if (j.photonId == GetMyPhotonID())
            {
                //������������Ȃ��ꍇ
                if (!j.joinPermission)
                {
                    mLoadBalancingClient.disconnect();
                    mState = PhotonState::CONNECTED;
                    connectFlg = false;

                    //�ޏo�ҏ���
                    for (int i = 0; i < saveInputPhoton.size(); ++i)
                    {
                        std::string name = "netPlayer" + std::to_string(saveInputPhoton[i].photonId);
                        GameObj net1 = GameObjectManager::Instance().Find(name.c_str());
                        if (net1)GameObjectManager::Instance().Remove(net1);
                    }

                    saveInputPhoton.clear();
                    return;
                }

                //��������
                joinPermission = true;
                myPlayerID = j.playerId;

            }
        }
    }
}

//���M
void PhotonLib::sendGameData(void)
{
    ExitGames::Common::Hashtable event;

    auto& obj = GameObjectManager::Instance().Find("player");

    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());

    //��ʂ��Q�[����
    netD.dataKind = NetData::DATA_KIND::GAME;

    //�}�X�^�[�N���C�A���g��
    netD.isMasterClient = GetIsMasterPlayer();

    //ID
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;

    //ID
    netD.playerId = myPlayerID;

    //���O
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));


    //�_���[�W��񑗐M
    auto sendDatas = StaticSendDataManager::Instance().GetNetSendDatas();
    for (auto& data : sendDatas)
    {
        if (data.sendType == 0)	//�_���[�W
            netD.gameData.damageData[data.id] += data.valueI;
        else if (data.sendType == 1)	//�q�[��
            netD.gameData.healData[data.id] += data.valueI;
        else if (data.sendType == 2)	//�X�^��
        {
            //��Ԓ����X�^�����Ԃ�^����
            if (netD.gameData.stanData[data.id] < data.valueF)
                netD.gameData.stanData[data.id] = data.valueF;
        }
        else if (data.sendType == 3)	//�m�b�N�o�b�N
            netD.gameData.knockbackData[data.id] += data.valueF3;
        else if (data.sendType == 4)	//�ړ��ʒu
            netD.gameData.movePosData[data.id] = data.valueF3;
    }

    //�}�X�^�[�N���C�A���g�̏ꍇ�̓`�[��ID�𑗂�
    if (GetIsMasterPlayer())
    {
        for (auto& s : saveInputPhoton)
        {
            netD.gameData.teamID[s.playerId] = s.teamID;
        }
    }

    //�L����ID�𑗂�
    netD.gameData.charaID = obj->GetComponent<CharacterCom>()->GetNetCharaData().GetCharaID();

    //�����̓��͂𑗂�
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId != myPhotonID)continue;

        //�擪20�t���[���̓��͂𑗂�
        netD.gameData.saveInputBuf = s.inputBuf->GetHeadFromSize(20);

        break;
    }

    //�^�C�}�[
    netD.gameData.startTime = startTime;

    std::stringstream s = NetDataSendCast(n);
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //�����ȊO�S���ɑ��M
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //����̃i���o�[�ɑ��M
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

void PhotonLib::sendJoinPermissionData(bool request)
{
    ExitGames::Common::Hashtable event;
    std::vector<NetData> n;
    NetData& netD = n.emplace_back(NetData());
    int myPhotonID = GetMyPhotonID();
    netD.photonId = myPhotonID;
    netD.isMasterClient = GetIsMasterPlayer();    
    ::strncpy_s(netD.name, sizeof(netD.name), netName.c_str(), sizeof(netD.name));

    //��ʂ������
    netD.dataKind = NetData::DATA_KIND::JOIN;

    if (request)    //�����\��
    {
        auto& join = netD.joinData.emplace_back();
        join.joinRequest = true;    //�����\��

        //�����͓K��
        join.photonId = -1;
        join.playerId = -1;
        join.joinPermission = false;
    }
    else    //�z�X�g�̏���
    {
        for (auto& j : joinManager) //�\�����X�g����R�c
        {
            auto& join = netD.joinData.emplace_back();
            join.photonId = j.photonId;
            join.playerId = j.playerId;
            join.joinPermission = false;
            join.joinRequest =false;
            //�S�l�܂Œǉ�
            if (saveInputPhoton.size() < 4)
            {
                join.joinPermission = true; //����������

                //�󂫂�T���ăv���C���[ID�����肷��
                bool playerID[4] = { false };
                for (auto& s : saveInputPhoton)
                {
                    playerID[s.playerId] = true;
                }
                for (int pID = 0; pID < 4; ++pID)
                {
                    if (playerID[pID])
                    {
                        join.playerId = pID;
                        break;
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
    //�����ȊO�S���ɑ��M
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //����̃i���o�[�ɑ��M
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));

}

//�v���C���[�ǉ�
void PhotonLib::AddPlayer(int photonID, int playerID)
{
    //�d������
    bool dabu = false;
    for (auto& s : saveInputPhoton)
    {
        if (s.photonId == photonID)dabu = true;
    }
    if (dabu)return;

    //�ǉ�
    SaveInput& saveInputJoin = saveInputPhoton.emplace_back(SaveInput());
    saveInputJoin.photonId = photonID;

    //�v���C���[ID����
    if (GetIsMasterPlayer())
        saveInputJoin.playerId = 0;
    else
        saveInputJoin.playerId = playerID;

    //�����Â��t���[����ۑ�
    saveInputJoin.nextInput.oldFrame = GetServerTime() - 100;

    //���̃t���[��������
    SaveBuffer saveBuf;
    saveBuf.frame = GetServerTime();
    saveInputJoin.inputBuf->Enqueue(saveBuf);
}