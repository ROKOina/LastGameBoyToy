#include "Server.h"

#include <iostream>
#pragma comment(lib,"ws2_32.lib")

#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"

__fastcall NetServer::~NetServer()
{
    NetwarkPost::~NetwarkPost();
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
    if (inet_pton(AF_INET, "224.10.1.1", &multicastAddr.sin_addr) == 0)
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


}

void __fastcall NetServer::Update()
{
    //�f�[�^��M
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

            //�o�^�ς݂Ȃ�㏑��
            for (auto& nData : clientND)
            {
                bool isRegisterClient = false;
                for (auto& client : clientDatas)
                {
                    if (nData.id == client.id)
                    {
                        isRegisterClient = true;
                        client = nData;
                        break;
                    }
                }
                //�o�^����Ă��Ȃ��Ȃ�o�^
                if (!isRegisterClient)
                    clientDatas.emplace_back(nData);
            }
        }
        else
        {
            std::cout << WSAGetLastError << std::endl;
        }
        std::cout << "message send:" << buffer << std::endl;
    }

    // �}���`�L���X�g�A�h���X������Ɏw�肵�ă��b�Z�[�W�𑗐M�A�p�P�b�g���X����̂��߂R�t���[����
    static int cou = 0;
    cou++;
    if (cou > 3)
    {
        //���M�^�ɕϊ����ăf�[�^��S�đ���
        std::stringstream ss = NetDataSendCast(clientDatas);

        sendto(multicastSock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0,
            reinterpret_cast<struct sockaddr*>(&multicastAddr), static_cast<int>(sizeof(multicastAddr)));
        cou = 0;
    }
}

#include <imgui.h>
void NetServer::ImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("NetServer", nullptr, ImGuiWindowFlags_None);

    ImGui::Text(recvData.c_str());

    static int cID = 0;

    ImGui::InputInt("drawID", &cID);
    if (clientDatas.size() > 0)
    {
        if (cID < 0)cID = 0;
        if (clientDatas.size() >= cID + 1)
        {
            GameObjectManager::Instance().Find("player")->transform_->SetWorldPosition(clientDatas[cID].pos);
        }
        else
        {
            cID = 0;
        }
    }

    ImGui::End();
}
