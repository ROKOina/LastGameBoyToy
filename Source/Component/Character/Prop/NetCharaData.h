#pragma once

//�L�����N�^�[�̃l�b�g�Ŏg������Z�߂�
class NetCharaData
{
    friend class CharacterCom;
public:
    NetCharaData() {}
    ~NetCharaData() {}

    void SetNetID(int id) { netID = id; }
    int GetNetID() { return netID; }

    void SetTeamID(int id) { teamID = id; }
    int GetTeamID() { return teamID; }

    int GetCharaID() { return charaID; }
    void  SetCharaID(const int id) { charaID = id; }

private:
    int teamID = 0;   //�����̃`�[��
    int netID = 0;//�ǂ̃N���C�A���g�����̃L������S�����邩
    int charaID;    //�L�����N�^�[���ʗp
};