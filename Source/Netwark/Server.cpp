#include "Server.h"

#include <iostream>
#pragma comment(lib,"ws2_32.lib")

#include "Input\Input.h"
#include "Input\GamePad.h"

#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"
#include "Components/MovementCom.h"
#include "Components/Character/CharacterCom.h"

__fastcall NetServer::~NetServer()
{
    NetwarkPost::~NetwarkPost();
    bufRing.release();
}

void __fastcall NetServer::Initialize()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    // �\�P�b�g�쐬(�N���C�A���g�����M�p)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7000);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // �ڑ���t�̃\�P�b�g���u���b�L���O(�m���u���b�L���O)�ɐݒ�
    u_long val = 1;
    ioctlsocket(sock, FIONBIO, &val);

    // �}���`�L���X�g�\�P�b�g�쐬(�N���C�A���g�֑��M�p)
    multicastSock = socket(AF_INET, SOCK_DGRAM, 0);
    multicastAddr.sin_family = AF_INET;      // IPv4�ݒ�
    multicastAddr.sin_port = htons(7002);    // �|�[�g(�}���`�L���X�g�p�|�[�g�A�ʏ�̃|�[�g�Ɣԍ���ς���K�v������)

    // inet_pton�c�W���e�L�X�g�\���`���̃C���^�[�l�b�g�l�b�g���[�N�A�h���X�𐔒l�o�C�i���`���ɕϊ�
    if (inet_pton(AF_INET, "224.10.1.15", &multicastAddr.sin_addr) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // ���[�J��IP�ݒ�
    ULONG localAddress = 0;
    // ���[�J��IP�͊e���̒[���p�ɕς��Ă�������
    if (inet_pton(AF_INET, "10.200.0.100", &localAddress) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // �}���`�L���X�g�I�v�V�����̐ݒ�(���M��)
    // ��1����,socket
    // ��2����,option�̎��
    // ��3����,���[�J��IP�A�h���X�̎w��
    // ��4����,���[�J��IP�A�h���X�̃T�C�Y
    setsockopt(multicastSock, IPPROTO_IP, IP_MULTICAST_IF,
        reinterpret_cast<char*>(&multicastAddr), sizeof(multicastAddr));

    // TTL�̃I�v�V�����ݒ�(Time To Live �L������)
    int ttl = 10;
    setsockopt(multicastSock, IPPROTO_IP, IP_MULTICAST_TTL,
        reinterpret_cast<char*>(&ttl), sizeof(ttl));

    //�T�[�o�[�̃L�������
    NetData serverData;
    serverData.id = id;
    clientDatas.emplace_back(serverData);

    //�����O�o�b�t�@������
    bufRing = std::make_unique<RingBuffer<SaveBuffer>>(30);
}

void __fastcall NetServer::Update()
{

    isNextFrame = false;

    ///******       �f�[�^��M        ******///
    Receive();


#ifdef PerfectSyn

    //���ގ��I���Ȃ�
    if (isEndJoin)
        //�o�^�N���C�A���g�̏�񂪑������ꍇ�ɐi��
        if (clientDatas.size() < clientNum)
        {
            return;
        }

#endif


    ///******       �f�[�^���M        ******///
    Send();


#ifdef EasyFrameSyn

    //�t���[���������킹��
    if (!IsSynchroFrame(true))
        return;

#endif // EasyFrameSyn


    nowFrame++; //�t���[�����Z

    //���̃t���[���ɍs�����Ƃ�������
    isNextFrame = true;


#ifdef PerfectSyn

    //���ގ��㏈��
    if (isEndJoin)
    {
        //���M��̓N���C�A���g�f�[�^�폜
        clientDatas.clear();

        //�T�[�o�[��񂾂��ǉ�
        NetData serverData;
        serverData.id = id;
        clientDatas.emplace_back(serverData);
    }

#endif
}

#include <imgui.h>
void NetServer::ImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("NetServer", nullptr, ImGuiWindowFlags_None);

    ImGui::Text(recvData.c_str());

    //�G���h�t���O�i���on�ɂ�����߂��Ȃ��j
    bool endJoin = isEndJoin;
    if (ImGui::Checkbox("end", &endJoin))
    {
        //�ŏ������N���C�A���g���ۑ�
        if (!isEndJoin)
        {
            clientNum = clientDatas.size();
        }
        isEndJoin |= endJoin;
    }

    //bool aaa = false;
    //if (ImGui::Checkbox("Enqueue", &aaa)) {
    //    static int bb = 0;
    //    bufRing->Enqueue(bb);
    //    bb++;
    //}
    //if (ImGui::Checkbox("Dele", &aaa)) {
    //    bufRing->Dequeue();
    //}


    //int frame = nowFrame;
    //ImGui::InputInt("nowFrame", &frame);
    //if (clientDatas.size() >= 2)
    //{
    //    frame = clientDatas[1].nowFrame;
    //    ImGui::InputInt("nowFram1", &frame);
    //}

    ImGui::End();
}

void NetServer::Receive()
{
    char buffer[MAX_BUFFER_NET] = {};
    struct sockaddr_in fromAddr;
    int addrSize = sizeof(struct sockaddr_in);
    //�N���C�A���g�̐������񂷂悤�ɂ���
    for (int i = 0; i < clientDatas.size() + 1; ++i)
    {
        int isRecv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&fromAddr, &addrSize);

        if (isRecv > 0)
        {
            std::cout << "msg : " << buffer << std::endl;
            recvData = buffer;

            //��
            std::vector<NetData> clientND = NetDataRecvCast(recvData);

            ////�t���[����
            //static std::vector<int> saveInt;
            //for (auto& c : clientND)
            //{
            //    if (c.id == id)continue;
            //    saveInt.emplace_back(nowFrame - c.nowFrame);
            //}

            //�o�^�ς݂Ȃ�㏑��
            for (auto& nData : clientND)
            {
                bool isRegisterClient = false;
                for (auto& client : clientDatas)
                {
                    if (nData.id != client.id)continue;

                    isRegisterClient = true;
                    client = nData;

                    //���͒ǉ�
                    for (auto& in : saveInput)
                    {
                        if (nData.id != in.id)continue;

                        //�t���[�����݂Ēǉ�
                        for (int inputSize = nData.saveInputBuf.size() - 1; inputSize >= 0; --inputSize)
                        {
                            auto& headInput = in.inputBuf->GetHead();   //�擪�f�[�^�擾
                            if (headInput.frame < nData.saveInputBuf[inputSize].frame)
                            {
                                in.inputBuf->Enqueue(nData.saveInputBuf[inputSize]);
                            }
                        }

                        ////���̂܂ܒǉ��o�[�W����
                        //SaveBuffer sb;
                        //sb.frame = nData.nowFrame;

                        //in.inputBuf->Enqueue(sb);

                        break;
                    }
                    break;
                }
                //�o�^����Ă��Ȃ��Ȃ�o�^
                if (!isRegisterClient)
                {
                    clientDatas.emplace_back(nData);

                    //���͗p
                    auto& saveInputClient = saveInput.emplace_back(SaveInput());
                    saveInputClient.id = nData.id;
                }
            }

            //�N���C�A���g���X�V
            RenderUpdate();
}
        else
        {
            std::cout << WSAGetLastError << std::endl;
        }
        std::cout << "message send:" << buffer << std::endl;
    }

}

void NetServer::Send()
{
    //���͏��X�V
    GamePad& gamePad = Input::Instance().GetGamePad();

    input |= gamePad.GetButton();
    inputDown |= gamePad.GetButtonDown();
    inputUp |= gamePad.GetButtonUp();

    //���͂������O�o�b�t�@�ɕۑ�
    SaveBuffer s;
    s.frame = nowFrame;
    s.input = input;
    s.inputDown = inputDown;
    s.inputUp = inputUp;
    bufRing->Enqueue(s);

    for (auto& client : clientDatas)
    {
        //�������g(server)�̃L�������𑗂�
        if (client.id != id)continue;

        GameObj player = GameObjectManager::Instance().Find(("player" + std::to_string(client.id)).c_str());
        client.pos = player->transform_->GetWorldPosition();
        client.velocity = player->GetComponent<MovementCom>()->GetVelocity();
        client.nonVelocity = player->GetComponent<MovementCom>()->GetNonMaxSpeedVelocity();
        client.rotato = player->transform_->GetRotation();

        //client.input = input;
        //client.inputDown = inputDown;
        //client.inputUp = inputUp;
        input = 0;
        inputDown = 0;
        inputUp = 0;

        //client.nowFrame = nowFrame;
        client.damageData = player->GetComponent<CharacterCom>()->GetGiveDamage();

        break;
    }
    //���M�^�ɕϊ����ăf�[�^��S�đ���
    std::stringstream ss = NetDataSendCast(clientDatas);

    sendto(multicastSock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0,
        reinterpret_cast<struct sockaddr*>(&multicastAddr), static_cast<int>(sizeof(multicastAddr)));
}
