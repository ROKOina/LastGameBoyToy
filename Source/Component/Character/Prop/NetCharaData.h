#pragma once

//キャラクターのネットで使う情報を纏める
class NetCharaData
{
    friend class CharacterCom;
public:
    NetCharaData() {}
    ~NetCharaData() {}

    void SetNetPlayerID(int id) { netPlayerID = id; }
    int GetNetPlayerID() { return netPlayerID; }

    void SetNetPhotonID(int id) { netPhotonID = id; }
    int GetNetPhotonID() { return netPhotonID; }

    void SetTeamID(int id) { teamID = id; }
    int GetTeamID() { return teamID; }

    int GetCharaID() { return charaID; }
    void  SetCharaID(const int id) { charaID = id; }

    void SetMyChara(bool flg) { myChara = flg; }

    void SetKillID(int id) { killID = id; }
    int GetKillID() { return killID; }

    void SetBulletNum(int num) { bulletNum = num; }
    int GetBulletNum() { return bulletNum; }

private:
    int teamID = 0;   //自分のチーム
    int netPlayerID = 0;//どのクライアントがこのキャラを担当するか
    int netPhotonID = 0;
    int charaID;    //キャラクター識別用
    bool myChara = false;

    int killID = -1;    //キルした相手保存
    int bulletNum = 0;    //球数
};