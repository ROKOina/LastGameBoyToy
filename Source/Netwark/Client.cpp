#include "Client.h"

#include <iostream>
//#include <winsock2.h>
//#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "Input\Input.h"
#include "Input\GamePad.h"

#include "Components/System/GameObject.h"
#include "Components/TransformCom.h"
#include "Components/MovementCom.h"

NetClient::~NetClient()
{
    NetwarkPost::~NetwarkPost();
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

    //�������؂�
    auto StringSplit = [](std::string& st, const char del)
        {
            char* sc = (char*)malloc(sizeof(char) * 10);
            
            int count = 0;
            while (1)
            {
                //������؂�I��
                if (st[0] == del || st[0] == '\0')
                {
                    if(st[0])
                        st = st.substr(1);
                    sc[count] = '\0';
                    break;
                }

                //�����ǉ�
                sc[count] = st[0];
                count++;
                st = st.substr(1);
            }

            std::string s = sc;
            delete sc;

            return s;
        };
    
    //ip�o�^
    std::string s = StringSplit(ipv4Adress, '.');
    int ip = std::stoi(s);
    addr.sin_addr.S_un.S_un_b.s_b1 = ip;
    s = StringSplit(ipv4Adress, '.');
    ip = std::stoi(s);
    addr.sin_addr.S_un.S_un_b.s_b2 = ip;
    s = StringSplit(ipv4Adress, '.');
    ip = std::stoi(s);
    addr.sin_addr.S_un.S_un_b.s_b3 = ip;
    s = StringSplit(ipv4Adress, '.');
    ip = std::stoi(s);
    addr.sin_addr.S_un.S_un_b.s_b4 = ip;

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

    // �ڑ���t�̃\�P�b�g���u���b�L���O(�m���u���b�L���O)�ɐݒ�
    u_long val = 1;
    ioctlsocket(multicastSock, FIONBIO, &val);

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
}

void __fastcall NetClient::Update()
{
    ///******       �f�[�^���M        ******///
    //���͏��X�V
    GamePad& gamePad = Input::Instance().GetGamePad();

    input |= gamePad.GetButton();
    inputDown |= gamePad.GetButtonDown();
    inputUp |= gamePad.GetButtonUp();

    //�p�P�b�g���X����̂��߁A3�t���[�����ɑ���
    //static int cou = 0;
    //cou++;
    //if (cou > 3) {
        //���Ń|�W�V�����𑗂�
        std::vector<NetData> netData;
        NetData n;
        n.id = id;
        n.radi = 1.1f;
        DirectX::XMFLOAT3 p = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();
        n.pos = p;
        n.velocity = GameObjectManager::Instance().Find("player")->GetComponent<MovementCom>()->GetVelocity();
        n.nonVelocity = GameObjectManager::Instance().Find("player")->GetComponent<MovementCom>()->GetNonMaxSpeedVelocity();
        n.rotato = GameObjectManager::Instance().Find("player")->transform_->GetRotation();

        n.input = input;
        n.inputDown = inputDown;
        n.inputUp = inputUp;
        input = 0;
        inputDown = 0;
        inputUp = 0;

        netData.emplace_back(n);

        //���M�^�ɕϊ�
        std::stringstream ss = NetDataSendCast(netData);

        sendto(sock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0, reinterpret_cast<struct sockaddr*>(&addr), static_cast<int>(sizeof(addr)));
    //    cou = 0;
    //}


    ///******       �f�[�^��M        ******///
    //�}���`�L���X�g�A�h���X����f�[�^��M
    char buffer[MAX_BUFFER_NET] = {};
    struct sockaddr_in fromAddr;
    int addrSize = sizeof(struct sockaddr_in);
    int isRecv = recvfrom(multicastSock, buffer,
        sizeof(buffer), 0,
        reinterpret_cast<struct sockaddr*>(&fromAddr),
        &addrSize);

    if (isRecv > 0)
    {
        recvData = buffer;
        std::cout << "multicast msg recieve: " << buffer << std::endl;

        clientDatas = NetDataRecvCast(recvData);

        RenderUpdate();
    }
    else
    {
        std::cout << WSAGetLastError() << std::endl;
    }
}

#include <imgui.h>
void NetClient::ImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("NetClient", nullptr, ImGuiWindowFlags_None);

    int ID = id;
    ImGui::InputInt("id", &ID);

    ImGui::End();
}
