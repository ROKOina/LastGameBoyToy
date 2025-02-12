#include "PVPGameSystem.h"
void PVPGameSystem::update(float elapsedTime)
{
    switch (gameMode)
    {
    case PVPGameSystem::GAME_MODE::Deathmatch:
        DeathmatchUpdate();
        break;
    case PVPGameSystem::GAME_MODE::Crown:
        CrownUpdate();
        break;
    case PVPGameSystem::GAME_MODE::Button:
        ButtonUpdate();
        break;
    }
}

void PVPGameSystem::OnGUI()
{
    const char* victryTeams[] = {
    "Red",
    "Bule",
    "Draw"
    };
    int cuurentNum = static_cast<int>(victoryTeam);
    ImGui::Begin("PVPGameSystem");
    // ImGuiのComboウィジェット
    if (ImGui::Combo("VictoryTeam", &cuurentNum, victryTeams, static_cast<int>(TEAM_KIND::Max))) {
        // Comboで選択が変更された場合、値を更新
        victoryTeam = static_cast<TEAM_KIND>(cuurentNum);
    }
    ImGui::Checkbox("isWEnd", &isEndGame);
    ImGui::End();
}

void PVPGameSystem::DeathmatchUpdate()
{
    for (int i = 0; i < 2; ++i)
    {
        //キル数達成
        if (deathMatchData.teamData[i].killCount >= deathMatchData.victoryCount)
        {
            isEndGame = true;	//ゲーム終了
            victoryTeam = TEAM_KIND(i);	//勝利チーム
            return;
        }
    }
    //タイムアップ
    if (deathMatchData.nowTime >= deathMatchData.endTime)
    {
        isEndGame = true;	//ゲーム終了
        //勝利チーム
        if (deathMatchData.teamData[0].killCount == deathMatchData.teamData[1].killCount)
        {
            victoryTeam = TEAM_KIND::DRAW;
        }
        else if (deathMatchData.teamData[0].killCount > deathMatchData.teamData[1].killCount)
        {
            victoryTeam = TEAM_KIND::RED_GROUP;
        }
        else
        {
            victoryTeam = TEAM_KIND::BLUE_GROUP;
        }
        return;
    }
}

void PVPGameSystem::CrownUpdate()
{
    //クラウン所持時間
    for (int i = 0; i < 2; ++i)
    {
        if (crownData.teamData[i].crownTime > crownData.haveWinTime)
        {
            isEndGame = true;	//ゲーム終了
            victoryTeam = TEAM_KIND(i);	//勝利チーム
            return;
        }
    }

    //タイムアップ
    if (crownData.nowTime >= crownData.endTime)
    {
        isEndGame = true;	//ゲーム終了
        //勝利チーム
        if (crownData.teamData[0].crownTime == crownData.teamData[1].crownTime)
        {
            victoryTeam = TEAM_KIND::DRAW;
        }
        else if (crownData.teamData[0].crownTime > crownData.teamData[1].crownTime)
        {
            victoryTeam = TEAM_KIND::RED_GROUP;
        }
        else
        {
            victoryTeam = TEAM_KIND::BLUE_GROUP;
        }
    }
}

void PVPGameSystem::ButtonUpdate()
{
    for (int i = 0; i < 2; ++i)
    {
        //ボタンを押した回数
        if (buttonData.teamData[i].buttoncount >= buttonData.winbuttoncount)
        {
            isEndGame = true;	//ゲーム終了
            victoryTeam = TEAM_KIND(i);	//勝利チーム
            return;
        }
    }

    //タイムアップ
    if (buttonData.nowTime >= buttonData.endTime)
    {
        isEndGame = true;	//ゲーム終了
        //勝利チーム
        if (buttonData.teamData[0].buttoncount == buttonData.teamData[1].buttoncount)
        {
            victoryTeam = TEAM_KIND::DRAW;
        }
        else if (buttonData.teamData[0].buttoncount > buttonData.teamData[1].buttoncount)
        {
            victoryTeam = TEAM_KIND::RED_GROUP;
        }
        else
        {
            victoryTeam = TEAM_KIND::BLUE_GROUP;
        }
    }
}