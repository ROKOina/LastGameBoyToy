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


    //デスマッチ勝敗必要情報
    struct DeathMatchData   //ゲーム
    {
        int victoryCount = 10;  //勝ちキル数
        float endTime = 120; //終了時間(秒)

        float nowTime = 0;
        struct DeathMatchTeamData   //チームごと
        {
            int killCount;
        };
        DeathMatchTeamData teamData[2];
    };

    //王冠勝敗必要情報
    struct CrownData   //ゲーム
    {
        struct CrownTeamData   //チームごと
        {
        };
        CrownTeamData teamData[2];
    };

    //ボタン勝敗必要情報
    struct ButtonData   //ゲーム
    {
        struct ButtonTeamData   //チームごと
        {
        };
        ButtonTeamData teamData[2];
    };

public:
    void update(float elapsedTime);

    void SetGameMode(GAME_MODE GM) { gameMode = GM; }
    GAME_MODE GetGameMode() { return gameMode; }

    //必要情報更新用
    DeathMatchData& GetDeathMatchData() { return deathMatchData; }
    CrownData& GetCrownData() { return crownData; }
    ButtonData& GetButtonData() { return buttonData; }

    //勝敗が決定したか
    bool IsGameEnd() { return isEndGame; }
    //勝利チーム
    TEAM_KIND GetVictoryTeam() { return victoryTeam; }

private:
    //各ゲームモード更新
    void DeathmatchUpdate();
    void CrownUpdate();
    void ButtonUpdate();


private:
    //ネットから取得する
    GAME_MODE gameMode = GAME_MODE::None;

    //必要情報更新
    DeathMatchData deathMatchData;
    CrownData crownData;
    ButtonData buttonData;

    bool isEndGame = false; //ゲーム終了フラグ
    TEAM_KIND victoryTeam = TEAM_KIND::DRAW;
};