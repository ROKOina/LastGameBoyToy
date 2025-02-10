#pragma once

class DelivertResultData
{
public:
    // インスタンス取得
    static DelivertResultData& Instance()
    {
        static DelivertResultData instance;
        return instance;
    }

    struct ResultData
    {
        int killNum = 0;
        int deathNum = 0;

        //必要な分足していく
        int charaID = -1;
        int playerID = -1;

        bool isWin = false;
        bool myTeam = false;

        std::string playerName = {};
    };

    void ResetResultData() {
        for (int i = 0; i < 4; ++i)
        {
            deliResultDatas[i].killNum = 0;
            deliResultDatas[i].deathNum = 0;
            deliResultDatas[i].charaID = -1;
            deliResultDatas[i].playerID = -1;
            deliResultDatas[i].isWin = false;
            deliResultDatas[i].myTeam = false;
            deliResultDatas[i].playerName.clear();
        }
        isMyWin = false;
    }

    ResultData& GetResultData(int id) { return deliResultDatas[id]; }
    void SetIsMyWin(bool win) { isMyWin = win; }
    bool GetIsMyWin() { return isMyWin; }
private:
    ResultData deliResultDatas[4];
    bool isMyWin = false;
};