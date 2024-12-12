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

void PVPGameSystem::DeathmatchUpdate()
{
	for (int i = 0; i < 2; ++i)
	{
		//�L�����B��
		if (deathMatchData.teamData[i].killCount >= deathMatchData.victoryCount)
		{
			isEndGame = true;	//�Q�[���I��
			victoryTeam = TEAM_KIND(i);	//�����`�[��
			return;
		}

	}
	//�^�C���A�b�v
	if (deathMatchData.nowTime >= deathMatchData.endTime)
	{

		isEndGame = true;	//�Q�[���I��
		//�����`�[��
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
}

void PVPGameSystem::ButtonUpdate()
{
}
