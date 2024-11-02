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
}

void PhotonLib::update(float elapsedTime)
{
    auto& myPlayer = GameObjectManager::Instance().Find("player");
    int myID = GetPlayerNum();
    for (auto& s : saveInputPhoton)
    {
        if (s.id != myID)continue;

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
        myPlayer->GetComponent<CharacterCom>()->SetTeamID(s.teamID);

        break;
    }
    myPlayer->GetComponent<CharacterCom>()->SetNetID(myID);

    switch (mState)
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
        break;
    case PhotonState::JOINED:
        //��񑗐M
        if (GetServerTime() - oldMs > sendMs)
        {
            sendData();
            oldMs = GetServerTime();
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
    //�l�b�g���[�N���艼�{�^��
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

    //ID�\��
    int photonID = GetPlayerNum();
    ImGui::InputInt("photonID", &photonID);

    //�}�X�^�[�N���C�A���g��
    bool isMaster = GetIsMasterPlayer();
    ImGui::Checkbox("master", &isMaster);

    if (isMaster)
    {
        if (ImGui::TreeNode("teamWake"))
        {
            for (auto& s : saveInputPhoton)
            {
                ImGui::InputInt(std::string("player" + std::to_string(s.id) + "teamID").c_str(), &s.teamID);
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
                ImGui::InputInt(std::string("player" + std::to_string(s.id) + "teamID").c_str(), &tem);
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

    std::vector<int> trips = GetTrips();
    for (int i = 0; i < trips.size(); ++i)
    {
        ImGui::InputInt(("trip" + std::to_string(i)).c_str(), &trips[i]);
    }

    int sendMs = SendMs();
    ImGui::InputInt("sendMS", &sendMs);

    int Delay = 0;
    int myID = GetPlayerNum();
    int netcount = 0;
    for (auto& s : saveInputPhoton)
    {
        Delay = s.myDelay;
        ImGui::InputInt(("Delay" + std::to_string(netcount)).c_str(), &Delay);
        netcount++;
    }

    ImGui::End();

    LobbyImGui();
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
    if (!joinPermission)
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
        if (ImGui::InputText(" ", name, sizeof(name)))
        {
            roomName = name;
        }

        if (ImGui::Button("CreateOrJoinRoom"))
        {
            if (roomName.size() > 0)
                joinPermission = true;
        }

        ImGui::Separator();

        //���[���ɎQ��
        auto rooms = mLoadBalancingClient.getRoomList();

        for (int i = 0; i < rooms.getSize(); ++i)
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            std::string selectRoom = WStringToString(rooms[i]->getName().cstr());
            ImGui::TreeNodeEx(&rooms[i], nodeFlags, selectRoom.c_str());

            // �N���b�N����ƑI��
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
        //�X�V�t���O
        s.isInputUpdate = false;

        int nowTime = GetServerTime();

        std::vector<SaveBuffer> saveB;
        saveB = s.inputBuf->GetHeadFromSize(100);

        std::vector<DirectX::XMFLOAT3>p;
        for (auto& b : saveB)
        {
            p.emplace_back(b.pos);
        }

        bool isInputInit = false;
        for (auto& b : saveB)
        {
            //�O��̃t���[�����獡��̃t���[������f�B���C�������܂ł̓��͂�ۑ�
            if (b.frame < s.nextInput.oldFrame)break;
            if (b.frame > nowTime - s.myDelay)continue;

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

    //�I�t���C��������
    if (!joinPermission)
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
        int myID = GetPlayerNum();
        for (auto& s : saveInputPhoton)
        {
            if (s.id != myID)continue;

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
    for (auto& s : saveInputPhoton)
    {
        std::string name = "netPlayer" + std::to_string(s.id);
        GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());

        if (!netPlayer)continue;

        auto& chara = netPlayer->GetComponent<CharacterCom>();

        chara->SetUserInput(s.nextInput.input);
        chara->SetUserInputDown(s.nextInput.inputDown);
        chara->SetUserInputUp(s.nextInput.inputUp);

        static bool upHokan = false;
        static DirectX::XMFLOAT3 hoknaPos = {};
        static DirectX::XMFLOAT3 nowPos = {};
        static int saveFrameHokan = 0;
        int frameHokan = 3; //�⊮����t���[��

        //�ړ�
        if (s.isInputUpdate)
        {
            static bool o = false;
            static int oi = 0;
            int frameAki = 3;

            oi++;
            if (o)
            {
                o = false;

                hoknaPos = s.nextInput.pos;
                nowPos = netPlayer->transform_->GetWorldPosition();
                if (Mathf::Length(nowPos - hoknaPos) > 0.1f)
                {
                    saveFrameHokan = 1;
                    upHokan = true;
                }
            }
            if (oi > frameAki)
            {
                o = true;
                oi = 0;
            }
        }
        else
        {
            //netPlayer->GetComponent<MovementCom>()->AddForce({ 1,0,0 });
        }

        if (upHokan)
        {
            float w = float(saveFrameHokan) / float(frameHokan);
            DirectX::XMFLOAT3 Hpos = Mathf::Lerp(nowPos, hoknaPos, w);
            netPlayer->transform_->SetWorldPosition(Hpos);
            saveFrameHokan++;
            if (Mathf::Length(Hpos) <= 0.1f)
            {
                netPlayer->transform_->SetWorldPosition(hoknaPos);
                upHokan = false;
            }
            if (saveFrameHokan > frameHokan)upHokan = false;
        }

        netPlayer->transform_->SetRotation(s.nextInput.rotato);
        chara->SetLeftStick(s.nextInput.leftStick);
        //�J�������
        chara->SetFpsCameraDir(s.nextInput.fpsCameraDir);

        //����
        netPlayer->GetComponent<MovementCom>()->SetVelocity(s.nextInput.velocity);

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
            //�擪�t���[���̍���ۑ�
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

std::vector<int> PhotonLib::GetTrips()
{
    if (saveInputPhoton.size() <= 0)return std::vector<int>();
    std::vector<int> trips;
    trips.resize(saveInputPhoton.size() - 1);

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

    //�}�X�^�[�N���C�A���g��
    netD.isMasterClient = GetIsMasterPlayer();

    //�_���[�W��񑗐M
    auto sendDatas = StaticSendDataManager::Instance().GetNetSendDatas();
    for (auto& data : sendDatas)
    {
        if (data.sendType == 0)	//�_���[�W
            netD.damageData[data.id] += data.valueI;
        else if (data.sendType == 1)	//�q�[��
            netD.healData[data.id] += data.valueI;
        else if (data.sendType == 2)	//�X�^��
        {
            //��Ԓ����X�^�����Ԃ�^����
            if (netD.stanData[data.id] < data.valueF)
                netD.stanData[data.id] = data.valueF;
        }
        else if (data.sendType == 3)	//�m�b�N�o�b�N
            netD.knockbackData[data.id] += data.valueF3;
        else if (data.sendType == 4)	//�ړ��ʒu
            netD.movePosData[data.id] = data.valueF3;
    }

    //�}�X�^�[�N���C�A���g�̏ꍇ�̓`�[��ID�𑗂�
    if (GetIsMasterPlayer())
    {
        for (auto& s : saveInputPhoton)
        {
            netD.teamID[s.id] = s.teamID;
        }
    }

    //�L����ID�𑗂�
    netD.charaID = obj->GetComponent<CharacterCom>()->GetCharaID();

    //�����̓��͂𑗂�
    int myID = GetPlayerNum();
    for (auto& s : saveInputPhoton)
    {
        if (s.id != myID)continue;

        //�擪10�t���[���̓��͂𑗂�
        netD.saveInputBuf = s.inputBuf->GetHeadFromSize(10);

        break;
    }

    std::stringstream s = NetDataSendCast(n);
    event.put(static_cast<nByte>(0), ExitGames::Common::JString(s.str().c_str()));
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    //�����ȊO�S���ɑ��M
    mLoadBalancingClient.opRaiseEvent(true, event, 0);
    //����̃i���o�[�ɑ��M
    //mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
    Logger::Print(std::string("ls joined the game" + WStringToString(player.getName().cstr())).c_str());
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
    mpOutputListener->writeString(L"");
    mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

    //���͏��ۑ��o�b�t�@�ǉ�
    for (int playerNum = 0; playerNum < playernrs.getSize(); ++playerNum)
    {
        //�V�K�N���C�A���g�m�F
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
            //�ǉ�
            SaveInput& saveInputJoin = saveInputPhoton.emplace_back(SaveInput());
            saveInputJoin.id = newClientID;
            saveInputJoin.nextInput.oldFrame = GetServerTime() - 100;

            //���̃t���[��������
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

    //�ޏo�ҏ���
    for (int i = 0; i < saveInputPhoton.size(); ++i)
    {
        if (saveInputPhoton[i].id != playerNr)continue;

        saveInputPhoton.erase(saveInputPhoton.begin() + i);
        break;
    }
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

            //�}�X�^�[�N���C�A���g�ȊO�̓`�[����ۑ�
            if (ne[0].isMasterClient)
            {
                for (auto& s : saveInputPhoton)
                {
                    s.teamID = ne[0].teamID[s.id];
                }
            }

            //���I�u�W�F
            std::string name = "netPlayer" + std::to_string(playerNr);
            GameObj net1 = GameObjectManager::Instance().Find(name.c_str());

            //�v���C���[�ǉ�
            if (!net1)
            {
                //net�v���C���[
                net1 = GameObjectManager::Instance().Create();
                net1->SetName(name.c_str());

                RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(ne[0].charaID), net1);
                net1->GetComponent<CharacterCom>()->SetNetID(playerNr);
            }

            //�_���[�W���
            for (int id = 0; id < ne[0].damageData.size(); ++id)
            {
                if (id != GetPlayerNum())continue;

                if (ne[0].damageData[id] > 0)
                {
                    auto& obj = GameObjectManager::Instance().Find("player");
                    obj->GetComponent<CharaStatusCom>()->AddDamagePoint(-ne[0].damageData[id]);
                    break;
                }
            }
            //�q�[�����
            for (int id = 0; id < ne[0].healData.size(); ++id)
            {
                if (id != GetPlayerNum())continue;

                if (ne[0].healData[id] > 0)
                {
                    auto& obj = GameObjectManager::Instance().Find("player");
                    obj->GetComponent<CharaStatusCom>()->AddHealPoint(ne[0].healData[id]);
                    break;
                }
            }
            //�X�^�����
            for (int id = 0; id < ne[0].stanData.size(); ++id)
            {
                if (id != GetPlayerNum())continue;

                if (ne[0].stanData[id] >= 0.1f)
                {
                    auto& obj = GameObjectManager::Instance().Find("player");
                    obj->GetComponent<CharacterCom>()->SetStanSeconds(ne[0].stanData[id]);
                    break;
                }
            }
            //�m�b�N�o�b�N���
            for (int id = 0; id < ne[0].knockbackData.size(); ++id)
            {
                if (id != GetPlayerNum())continue;

                if (Mathf::Length(ne[0].knockbackData[id]) >= 0.1f)
                {
                    auto& obj = GameObjectManager::Instance().Find("player");
                    obj->GetComponent<MovementCom>()->SetNonMaxSpeedVelocity(ne[0].knockbackData[id]);
                    break;
                }
            }
            //�ړ��ʒu���
            for (int id = 0; id < ne[0].movePosData.size(); ++id)
            {
                if (id != GetPlayerNum())continue;

                if (Mathf::Length(ne[0].movePosData[id]) >= 0.1f)
                {
                    auto& obj = GameObjectManager::Instance().Find("player");
                    obj->transform_->SetWorldPosition(ne[0].movePosData[id]);
                    break;
                }
            }

            //���͂�ۑ�
            for (int i = ne[0].saveInputBuf.size() - 1; i >= 0; --i)
            {
                SaveBuffer newInput = ne[0].saveInputBuf[i];

                for (auto& s : saveInputPhoton)
                {
                    if (s.id != playerNr)continue;
                    SaveBuffer currentInput = s.inputBuf->GetHead();
                    if (currentInput.frame < newInput.frame)	//�V�����t���[������n�߂�
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