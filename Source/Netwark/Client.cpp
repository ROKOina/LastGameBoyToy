#include "Client.h"

#include <iostream>
//#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include <sstream>
#include <vector>

static bool endThread = false;

NetClient::~NetClient()
{
    endThread = true;
    thread->join();

    // �\�P�b�g�I��
    if (closesocket(multicastSock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    if (closesocket(sock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // WSA�I��
    WSACleanup();
}

//winsock2.h�@�C���N���[�h�G���[�ł��ꂾ���O���[�o����
struct sockaddr_in addr;

//��M�X���b�h�p
void NetClient::RecvThread()
{
    char buffer[256];
    struct sockaddr_in fromAddr;
    int addrSize = sizeof(struct sockaddr_in);
    do
    {
        //�}���`�L���X�g�A�h���X����f�[�^��M
        int n = recvfrom(multicastSock, buffer,
            sizeof(buffer), 0,
            reinterpret_cast<struct sockaddr*>(&fromAddr),
            &addrSize);
        if (n > 0)
        {
            recvData = buffer;
            std::cout << "multicast msg recieve: " << buffer << std::endl;
        }
        else
        {
            std::cout << WSAGetLastError() << std::endl;
        }
    } while (strcmp(buffer, "exit") != 0);
}

void __fastcall NetClient::Initialize()
{
    //WSA������
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // �\�P�b�g�쐬(�T�[�o�֑��M�p)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7000);

    addr.sin_addr.S_un.S_un_b.s_b1 = 192;
    addr.sin_addr.S_un.S_un_b.s_b2 = 168;
    addr.sin_addr.S_un.S_un_b.s_b3 = 1;
    addr.sin_addr.S_un.S_un_b.s_b4 = 7;

    // �}���`�L���X�g�\�P�b�g�쐬(�T�[�o�����M�p)
    multicastSock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicastAddress;
    multicastAddress.sin_family = AF_INET;
    multicastAddress.sin_port = htons(7002);// �}���`�L���X�g�p�̃|�[�g�ԍ��͑��M�p�ƕύX���邱��
    multicastAddress.sin_addr.S_un.S_addr = INADDR_ANY;  // �T�[�o���Ŋ��蓖�Ă��Ă���IP�������Őݒ�

    if (bind(multicastSock, (struct sockaddr*)&multicastAddress, sizeof(multicastAddress)) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // �}���`�L���X�g�O���[�v�֓o�^����( join )
    // �}���`�L���X�g�O���[�v�p�\����ip_mreq���g�p����
    // �}���`�L���X�g�A�h���X�́u224.10.1.1�v���g�p
    struct ip_mreq mr;
    if (inet_pton(AF_INET, "224.10.1.1", &mr.imr_multiaddr.s_addr) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // �C���^�[�t�F�[�X�̃A�h���X�ݒ�
    mr.imr_interface.s_addr = INADDR_ANY;
    
    setsockopt(multicastSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        reinterpret_cast<const char*>(&mr), sizeof(mr));

    // ���V�[�u�p�}���`�X���b�h�N��
    thread = std::make_unique<std::thread>(&NetClient::RecvThread, this);
}

void __fastcall NetClient::Update()
{
    // �ʏ�̃\�P�b�g�ŃT�[�o�Ƀ��b�Z�[�W�𑗐M

    //test:vector int����

    std::vector<int> vec{ 6, 3, 8, -9, 1, -2, 8 };
    std::stringstream ss;
    for (auto it = vec.begin(); it != vec.end(); it++) {
        if (it != vec.begin()) {
            ss << " ";
        }
        ss << *it;
    }
    //sendto(sock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0, reinterpret_cast<struct sockaddr*>(&addr), static_cast<int>(sizeof(addr)));

    //imgui�ő���
    if (isSend)
    {
        sendto(sock, sendData.c_str(), static_cast<int>(strlen(sendData.c_str()) + 1), 0, reinterpret_cast<struct sockaddr*>(&addr), static_cast<int>(sizeof(addr)));
        isSend = false;
    }
}

#include <imgui.h>
void NetClient::ImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("NetClient", nullptr, ImGuiWindowFlags_None);

    char data[256];
    ::strncpy_s(data, sizeof(data), sendData.c_str(), sizeof(data));
    if (ImGui::InputText("data", data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        sendData = data;
    }

    if (ImGui::Button("sendData"))
    {
        isSend = true;
    }
    ImGui::Text(recvData.c_str());

    ImGui::End();
}
