#pragma once

#include "../RingBuffer.h"
#include <DirectXMath.h>
#include <vector>
#include "../NetData.h"

//クライアントに送信する情報を管理する
class StaticSendDataManager
{
public:
    StaticSendDataManager()
        : sendGameData(std::make_unique<RingBuffer<NetSendData>>(100))
    {
    }
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
        int playerID = {}; //送信相手

        int sendType = {};   //0:damage 1:heal 2:stan 3:knockback 4:movePos

        int valueI = {};
        float valueF = {};
        DirectX::XMFLOAT3 valueF3 = {};
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
            if (sendGameData->GetSize() > 0)
            {
                auto n = sendGameData->Dequeue();
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
    std::unique_ptr<RingBuffer<NetSendData>> sendGameData;

    //値受け渡し用
public:
    std::vector<SaveBuffer>& GetSaveBuffer(int playerID) { return saveBuffer[playerID]; }
    bool& GetDeathID(int id) { return deathID[id]; }
    bool& GetKillID(int killID, int deathID) { return killlog[killID][deathID]; }
    std::vector<DirectX::XMFLOAT3>& GetDamagePos() { return damagePostPos; }
    int& GetTeamNum(int id) { return teamNum[id]; }

    //ピン関係
    struct PinSendData
    {
        bool isPos = false;
        //位置
        DirectX::XMFLOAT3 pinPos = {};

        //ターゲットデータ
        int photonid;
    };
    void SendNetPing(PinSendData data) {
        pinNum = 3;
        pinSend = data;
    }
    bool GetNetPing(PinSendData& data) {
        if (pinNum < 0)return false;

        data = pinSend;
        pinNum--;
        return true;
    }

    void SendMyDeath() { isMyDeath = true; }
    bool GetMyDeath() {
        //送信したらリセット
        bool d = isMyDeath;
        isMyDeath = false;
        return d;
    }

    void ResetData()
    {
        for (auto& s : saveBuffer)
            s.clear();
        for (int i = 0; i < 4; ++i) {
            teamNum[i] = -1;
            deathID[i] = false;

            for (int ii = 0; ii < 4; ++ii) {
                killlog[i][ii] = false;
            }
        }
        isMyDeath = false;
    }
private:
    std::vector<SaveBuffer> saveBuffer[5];
    bool deathID[4] = {};    //キルされた相手を保存
    bool killlog[4][4] = {};    //[キルID][デスID]
    std::vector<DirectX::XMFLOAT3> damagePostPos = {};
    int teamNum[4] = { -1,-1,-1,-1 };    //[]playerID  0:赤 1:青 -1:なし
    bool isMyDeath = false;   //デスした時にtrueに

    //ピン関係
    int pinNum = -1;
    PinSendData pinSend;
};