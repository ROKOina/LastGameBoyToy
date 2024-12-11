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

    void SetTeamID(int id) { teamID = id; }
    int GetTeamID() { return teamID; }

    int GetCharaID() { return charaID; }
    void  SetCharaID(const int id) { charaID = id; }

private:
    int teamID = 0;   //自分のチーム
    int netPlayerID = 0;//どのクライアントがこのキャラを担当するか
    int charaID;    //キャラクター識別用
};