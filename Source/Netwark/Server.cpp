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
    // ソケット作成(クライアントから受信用)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7000);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // 接続受付のソケットをブロッキング(ノンブロッキング)に設定
    u_long val = 1;
    ioctlsocket(sock, FIONBIO, &val);

    // マルチキャストソケット作成(クライアントへ送信用)
    multicastSock = socket(AF_INET, SOCK_DGRAM, 0);
    multicastAddr.sin_family = AF_INET;      // IPv4設定
    multicastAddr.sin_port = htons(7002);    // ポート(マルチキャスト用ポート、通常のポートと番号を変える必要がある)

    // inet_pton…標準テキスト表示形式のインターネットネットワークアドレスを数値バイナリ形式に変換
    if (inet_pton(AF_INET, "224.10.1.1", &multicastAddr.sin_addr) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // ローカルIP設定
    ULONG localAddress = 0;
    // ローカルIPは各自の端末用に変えてください
    if (inet_pton(AF_INET, "10.200.0.100", &localAddress) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // マルチキャストオプションの設定(送信側)
    // 第1引数,socket
    // 第2引数,optionの種類
    // 第3引数,ローカルIPアドレスの指定
    // 第4引数,ローカルIPアドレスのサイズ
    setsockopt(multicastSock, IPPROTO_IP, IP_MULTICAST_IF,
        reinterpret_cast<char*>(&multicastAddr), sizeof(multicastAddr));

    // TTLのオプション設定(Time To Live 有効時間)
    int ttl = 10;
    setsockopt(multicastSock, IPPROTO_IP, IP_MULTICAST_TTL,
        reinterpret_cast<char*>(&ttl), sizeof(ttl));


}

void __fastcall NetServer::Update()
{
    //データ受信
    char buffer[MAX_BUFFER_NET] = {};
    struct sockaddr_in fromAddr;
    int addrSize = sizeof(struct sockaddr_in);
    //クライアントの数だけ回すようにする
    for (int i = 0; i < clientDatas.size() + 1; ++i)
    {
        int isRecv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&fromAddr, &addrSize);

        if (isRecv > 0)
        {
            std::cout << "msg : " << buffer << std::endl;
            recvData = buffer;

            //仮
            std::vector<NetData> clientND = NetDataRecvCast(recvData);

            //登録済みなら上書き
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
                //登録されていないなら登録
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

    // マルチキャストアドレスを宛先に指定してメッセージを送信、パケットロス回避のため３フレーム毎
    static int cou = 0;
    cou++;
    if (cou > 3)
    {
        //送信型に変換してデータを全て送る
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
