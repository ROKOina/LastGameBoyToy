#pragma once

#include <string>
#include "NetData.h"
#include "RingBuffer.h"

//完全同期設定
//#define PerfectSyn

//簡易フレーム同期
//#define EasyFrameSyn

//遅延方式
#define DelaySystem

class NetwarkPost
{
public:
    __fastcall NetwarkPost() {}
    virtual __fastcall ~NetwarkPost();

    virtual void __fastcall Initialize() = 0;

    virtual void __fastcall Update() = 0;

    virtual void ImGui() = 0;

    virtual void RenderUpdate();

public:
     std::vector<NetData>& GetNetDatas() { return clientDatas; }
    const int GetNetId() { return id; }

    const bool IsNextFrame() { return isNextFrame; }

protected:
    //フレーム同期できているか（出来てるならtrue）
    bool IsSynchroFrame(bool );

protected:

    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData = "ABC";
    std::vector<NetData> clientDatas;
    int id;
    std::vector<int> newLoginID;

    unsigned int input;
    unsigned int inputUp;
    unsigned int inputDown;

    //各クライアントのフレーム保存
    long long nowFrame;
    

    //リングバッファ
    std::unique_ptr<RingBuffer<SaveBuffer>> bufRing;

    //完全同期用
    bool isEndJoin = false; //入室終了フラグ
    bool isNextFrame = false;   //次のフレームに行けるか
    int clientNum;

    //ディレイ方式用
    const float delaySec = 6/60;
    int delayNow;
    long long playFrame;    //現在実行フレーム

    //インプット保存（サーバー用）
    struct SaveInput
    {
        int id;
        std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;
        SaveInput()
        {
            inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
        }
    };
    std::vector<SaveInput> saveInput;
};