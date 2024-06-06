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

    // ソケット終了
    if (closesocket(multicastSock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }
    if (closesocket(sock) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // WSA終了
    WSACleanup();
}

//winsock2.h　インクルードエラーでこれだけグローバルに
struct sockaddr_in addr;

//受信スレッド用
void NetClient::RecvThread()
{
    char buffer[256];
    struct sockaddr_in fromAddr;
    int addrSize = sizeof(struct sockaddr_in);
    do
    {
        //マルチキャストアドレスからデータ受信
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
    //WSA初期化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // ソケット作成(サーバへ送信用)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7000);

    addr.sin_addr.S_un.S_un_b.s_b1 = 192;
    addr.sin_addr.S_un.S_un_b.s_b2 = 168;
    addr.sin_addr.S_un.S_un_b.s_b3 = 1;
    addr.sin_addr.S_un.S_un_b.s_b4 = 7;

    // マルチキャストソケット作成(サーバから受信用)
    multicastSock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicastAddress;
    multicastAddress.sin_family = AF_INET;
    multicastAddress.sin_port = htons(7002);// マルチキャスト用のポート番号は送信用と変更すること
    multicastAddress.sin_addr.S_un.S_addr = INADDR_ANY;  // サーバ側で割り当てられているIPを自動で設定

    if (bind(multicastSock, (struct sockaddr*)&multicastAddress, sizeof(multicastAddress)) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // マルチキャストグループへ登録処理( join )
    // マルチキャストグループ用構造体ip_mreqを使用する
    // マルチキャストアドレスは「224.10.1.1」を使用
    struct ip_mreq mr;
    if (inet_pton(AF_INET, "224.10.1.1", &mr.imr_multiaddr.s_addr) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // インターフェースのアドレス設定
    mr.imr_interface.s_addr = INADDR_ANY;
    
    setsockopt(multicastSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        reinterpret_cast<const char*>(&mr), sizeof(mr));

    // レシーブ用マルチスレッド起動
    thread = std::make_unique<std::thread>(&NetClient::RecvThread, this);
}

void __fastcall NetClient::Update()
{
    // 通常のソケットでサーバにメッセージを送信

    //test:vector int送り

    std::vector<int> vec{ 6, 3, 8, -9, 1, -2, 8 };
    std::stringstream ss;
    for (auto it = vec.begin(); it != vec.end(); it++) {
        if (it != vec.begin()) {
            ss << " ";
        }
        ss << *it;
    }
    //sendto(sock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0, reinterpret_cast<struct sockaddr*>(&addr), static_cast<int>(sizeof(addr)));

    //imguiで送る
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
