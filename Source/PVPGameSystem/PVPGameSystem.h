#pragma once

class PVPGameSystem
{
public:
    PVPGameSystem() {}
    ~PVPGameSystem() {}

    enum TEAM_KIND
    {
        RED_GROUP,
        BLUE_GROUP,
        DRAW,
    };

    enum class GAME_MODE
    {
        None,

        Deathmatch,
        Crown,
        Button,
    };


    //�f�X�}�b�`���s�K�v���
    struct DeathMatchData   //�Q�[��
    {
        int victoryCount = 10;  //�����L����
        float endTime = 120; //�I������(�b)

        float nowTime = 0;
        struct DeathMatchTeamData   //�`�[������
        {
            int killCount;
        };
        DeathMatchTeamData teamData[2];
    };

    //�������s�K�v���
    struct CrownData   //�Q�[��
    {
        struct CrownTeamData   //�`�[������
        {
        };
        CrownTeamData teamData[2];
    };

    //�{�^�����s�K�v���
    struct ButtonData   //�Q�[��
    {
        struct ButtonTeamData   //�`�[������
        {
        };
        ButtonTeamData teamData[2];
    };

public:
    void update(float elapsedTime);

    void SetGameMode(GAME_MODE GM) { gameMode = GM; }
    GAME_MODE GetGameMode() { return gameMode; }

    //�K�v���X�V�p
    DeathMatchData& GetDeathMatchData() { return deathMatchData; }
    CrownData& GetCrownData() { return crownData; }
    ButtonData& GetButtonData() { return buttonData; }

    //���s�����肵����
    bool IsGameEnd() { return isEndGame; }
    //�����`�[��
    TEAM_KIND GetVictoryTeam() { return victoryTeam; }

private:
    //�e�Q�[�����[�h�X�V
    void DeathmatchUpdate();
    void CrownUpdate();
    void ButtonUpdate();


private:
    //�l�b�g����擾����
    GAME_MODE gameMode = GAME_MODE::None;

    //�K�v���X�V
    DeathMatchData deathMatchData;
    CrownData crownData;
    ButtonData buttonData;

    bool isEndGame = false; //�Q�[���I���t���O
    TEAM_KIND victoryTeam = TEAM_KIND::DRAW;
};