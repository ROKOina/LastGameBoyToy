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

        int sendType;   //0:damage 1:heal 2:stan 3:knockback 4:movePos

        int valueI;
        float valueF;
        DirectX::XMFLOAT3 valueF3;
    };

    //ダメージを送信
    void SetSendDamage(int myID, int sendID, float damage);

    //ヒールを送信
    void SetSendHeal(int myID, int sendID, float heal);

    //スタンを送信
    void SetSendStan(int myID, int sendID, float stanSec);

    //ノックバックを送信
    void SetSendKnockback(int myID, int sendID, DirectX::XMFLOAT3 knockbackVec);

    //移動位置を送信
    void SetSendMovePos(int myID, int sendID, DirectX::XMFLOAT3 movePos);

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