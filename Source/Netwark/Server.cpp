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
    if (inet_pton(AF_INET, "224.10.1.15", &multicastAddr.sin_addr) == 0)
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

    //サーバーのキャラ情報
    NetData serverData;
    serverData.id = id;
    clientDatas.emplace_back(serverData);

    //リングバッファ初期化
    bufRing = std::make_unique<RingBuffer<SaveBuffer>>(30);
}

void __fastcall NetServer::Update()
{

    isNextFrame = false;

    ///******       データ受信        ******///
    Receive();


#ifdef PerfectSyn

    //入退室終了なら
    if (isEndJoin)
        //登録クライアントの情報が揃った場合に進む
        if (clientDatas.size() < clientNum)
        {
            return;
        }

#endif


    ///******       データ送信        ******///
    Send();


#ifdef EasyFrameSyn

    //フレーム数を合わせる
    if (!IsSynchroFrame(true))
        return;

#endif // EasyFrameSyn


    nowFrame++; //フレーム加算

    //次のフレームに行くことを許可する
    isNextFrame = true;


#ifdef PerfectSyn

    //入退室後処理
    if (isEndJoin)
    {
        //送信後はクライアントデータ削除
        clientDatas.clear();

        //サーバー情報だけ追加
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

    //エンドフラグ（一回onにしたら戻せない）
    bool endJoin = isEndJoin;
    if (ImGui::Checkbox("end", &endJoin))
    {
        //最初だけクライアント数保存
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

            ////フレーム差
            //static std::vector<int> saveInt;
            //for (auto& c : clientND)
            //{
            //    if (c.id == id)continue;
            //    saveInt.emplace_back(nowFrame - c.nowFrame);
            //}

            //登録済みなら上書き
            for (auto& nData : clientND)
            {
                bool isRegisterClient = false;
                for (auto& client : clientDatas)
                {
                    if (nData.id != client.id)continue;

                    isRegisterClient = true;
                    client = nData;

                    //入力追加
                    for (auto& in : saveInput)
                    {
                        if (nData.id != in.id)continue;

                        //フレームをみて追加
                        for (int inputSize = nData.saveInputBuf.size() - 1; inputSize >= 0; --inputSize)
                        {
                            auto& headInput = in.inputBuf->GetHead();   //先頭データ取得
                            if (headInput.frame < nData.saveInputBuf[inputSize].frame)
                            {
                                in.inputBuf->Enqueue(nData.saveInputBuf[inputSize]);
                            }
                        }

                        ////そのまま追加バージョン
                        //SaveBuffer sb;
                        //sb.frame = nData.nowFrame;

                        //in.inputBuf->Enqueue(sb);

                        break;
                    }
                    break;
                }
                //登録されていないなら登録
                if (!isRegisterClient)
                {
                    clientDatas.emplace_back(nData);

                    //入力用
                    auto& saveInputClient = saveInput.emplace_back(SaveInput());
                    saveInputClient.id = nData.id;
                }
            }

            //クライアント情報更新
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
    //入力情報更新
    GamePad& gamePad = Input::Instance().GetGamePad();

    input |= gamePad.GetButton();
    inputDown |= gamePad.GetButtonDown();
    inputUp |= gamePad.GetButtonUp();

    //入力をリングバッファに保存
    SaveBuffer s;
    s.frame = nowFrame;
    s.input = input;
    s.inputDown = inputDown;
    s.inputUp = inputUp;
    bufRing->Enqueue(s);

    for (auto& client : clientDatas)
    {
        //自分自身(server)のキャラ情報を送る
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
    //送信型に変換してデータを全て送る
    std::stringstream ss = NetDataSendCast(clientDatas);

    sendto(multicastSock, ss.str().c_str(), static_cast<int>(strlen(ss.str().c_str()) + 1), 0,
        reinterpret_cast<struct sockaddr*>(&multicastAddr), static_cast<int>(sizeof(multicastAddr)));
}
