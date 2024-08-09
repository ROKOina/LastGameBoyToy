#pragma once

#include "../RingBuffer.h"

//クライアントに送信する情報を管理する
class StaticSendDataManager
{
public:
    StaticSendDataManager() 
        : sendData(std::make_unique<RingBuffer<NetSendData>>(100))
    {}
    ~StaticSendDataManager() {}

    // インスタンス取得
    static StaticSendDataManager& Instance()
    {
        static StaticSendDataManager instance;
        return instance;
    }

    //送信情報
    struct NetSendData
    {
        int id; //送信相手

        int sendType;   //0:damage 1:heal 2:stan

        int value;
    };

    //ダメージを送信
    void SetSendDamage(int myID, int sendID, int damage);

    //ヒールを送信
    void SetSendHead(int myID, int sendID, int damage);

    //送信情報を全て取得して消す
    std::vector<NetSendData> GetNetSendDatas() 
    {
        std::vector<NetSendData> netDatas;
        while (1)
        {
            if (sendData->GetSize() > 0)
            {
                auto n = sendData->Dequeue();
                netDatas.emplace_back(n);
            }
            else
            {
                break;
            }
        }
        return netDatas;
    }

private:
    std::unique_ptr<RingBuffer<NetSendData>> sendData;
};