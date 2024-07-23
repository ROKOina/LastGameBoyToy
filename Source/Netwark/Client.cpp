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
#include "Components/Character/CharacterCom.h"

NetClient::~NetClient()
{
    NetwarkPost::~NetwarkPost();
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

    //文字列区切り
    auto StringSplit = [](std::string& st, const char del)
        {
            char* sc = (char*)malloc(sizeof(char) * 10);
            
            int count = 0;
            while (1)
            {
                //文字区切り終了
                if (st[0] == del || st[0] == '\0')
                {
                    if(st[0])
                        st = st.substr(1);
                    sc[count] = '\0';
                    break;
                }

                //文字追加
                sc[count] = st[0];
                count++;
                st = st.substr(1);
            }

            std::string s = sc;
            delete sc;

            return s;
        };
    
    //ip登録
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

#ifdef PerfectSyn

#else

    // 接続受付のソケットをブロッキング(ノンブロッキング)に設定
    u_long val = 1;
    ioctlsocket(multicastSock, FIONBIO, &val);

#endif

    // マルチキャストグループへ登録処理( join )
    // マルチキャストグループ用構造体ip_mreqを使用する
    // マルチキャストアドレスは「224.10.1.1」を使用
    struct ip_mreq mr;
    if (inet_pton(AF_INET, "224.10.1.15", &mr.imr_multiaddr.s_addr) == 0)
    {
        std::cout << "error_code:" << WSAGetLastError();
    }

    // インターフェースのアドレス設定
    mr.imr_interface.s_addr = INADDR_ANY;
    
    setsockopt(multicastSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        reinterpret_cast<const char*>(&mr), sizeof(mr));

    isNextFrame = true;

    //リングバッファ初期化
    bufRing = std::make_unique<RingBuffer<int>>(10);
}

void __fastcall NetClient::Update()
{    
    //完全同期用
    isNextFrame = false;


    ///******       データ受信        ******///
    Receive();

    ////フレーム数を合わせる
    //if (!IsSynchroFrame())
    //    return;

    nowFrame++;

    ///******       データ送信        ******///
    Send();

    //次のフレームに行くことを許可する
    isNextFrame = true;
}

#include <imgui.h>
void NetClient::ImGui()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("NetClient", nullptr, ImGuiWindowFlags_None);

    int ID = id;
    ImGui::InputInt("id", &ID);

    int count = 0;
    for (auto& c : clientDatas)
    {
        ImGui::DragFloat("damageData" + count, &c.damageData[0]);
        count++;
    }

    ImGui::End();
}

void NetClient::Receive()
{    
    //マルチキャストアドレスからデータ受信
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

        //最初の交信時
        if (!firstConect)
        {
            firstConect = true;
            //フレームを保存
            for (auto& c : clientDatas)
            {
                if (c.id == 0)
                {
                    nowFrame = c.nowFrame;
                    break;
                }
            }
        }

        RenderUpdate();
    }
    else
    {
        std::cout << WSAGetLastError() << std::endl;
    }
}

void NetClient::Send()
{
    //入力情報更新
    GamePad& gamePad = Input::Instance().GetGamePad();

    input |= gamePad.GetButton();
    inputDown |= gamePad.GetButtonDown();
    inputUp |= gamePad.GetButtonUp();

    //仮でポジションを送る
    std::vector<NetData> netData;
    NetData n;
    n.id = id;
    n.radi = 1.1f;

    GameObject* player = GameObjectManager::Instance().Find(("player" + std::to_string(id)).c_str()).get();

    n.pos = player->transform_->GetWorldPosition();
    n.velocity = player->GetComponent<MovementCom>()->GetVelocity();
    n.nonVelocity = player->GetComponent<MovementCom>()->GetNonMaxSpeedVelocity();
    n.rotato = player->transform_->GetRotation();

    n.input = input;
    n.inputDown = inputDown;
    n.inputUp = inputUp;
    input = 0;
    inputDown = 0;
    inputUp = 0;

    n.nowFrame = nowFrame;

    n.damageData = player->GetComponent<CharacterCom>()->GetGiveDamage();
    n.damageData[0] = id;//player->GetComponent<CharacterCom>()->GetGiveDamage();

    netData.emplace_back(n);

    //送信型に変換
    std::stringstream ss = NetDataSendCast(netData);

    sendto(sock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0, reinterpret_cast<struct sockaddr*>(&addr), static_cast<int>(sizeof(addr)));
}
