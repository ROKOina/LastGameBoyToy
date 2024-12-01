#pragma once
#include "Component/System/Component.h"





class TrainingManager
{
private:
    TrainingManager();
    ~TrainingManager();
public:
    //�B��̃C���X�^���X�擾
    static TrainingManager& Instance()
    {
        static TrainingManager instance;
        return instance;
    }

    void TrainingManagerUpdate(float elapsedTime);
    void TrainingManagerStart();
    void TrainingManagerClear();

public:
    void SetTutorialFlag(bool flag) { tutorialFlag = flag; }
private:

    //�ŏ��̓g���[�j���O���[�h
    bool tutorialFlag = false;

};

class TrainingSystem
{
private:
    TrainingSystem();
    ~TrainingSystem();
public:
    //�B��̃C���X�^���X�擾
    static TrainingSystem& Instance()
    {
        static TrainingSystem instance;
        return instance;
    }

    void TrainingSystemUpdate(float elapsdTime);
    void TrainingSystemStart();
    void TrainingSystemClear();

public:

    bool GetShootingIntervalFlag() { return shootingIntervalFlag; }

private:
    //�ˌ��̃��W�b�N�H
    void ShootingSystem(float elapsdTime);

    void ShootingRandomSpawn();

private:
    int     shootingScore = 0;
    float   scarecrowSpawnTime = 1.0f;  //�|���Ă���1�b�ԑ҂�
    float   scarecrowSpawnTimer = 0.0f;
    float   scarecrowLifeTime = 1.0f;
    float   scarecrowLifeTimer = 0.0f;
    float   scarecrowSpawnIntervalTime  = 1.0f;
    float   scarecrowSpawnIntervalTimer = 0.0f;
    DirectX::XMFLOAT3 randomPos = { 0,0,0 };
    bool    shootingStartFlag = true;
    bool    shootingIntervalFlag = false;


    //��Փx�ω������邩�H
    //�������

};

class TutorialSystem
{
private:
    TutorialSystem();
    ~TutorialSystem();
public:
    //�B��̃C���X�^���X�擾
    static TutorialSystem& Instance()
    {
        static TutorialSystem instance;
        return instance;
    }

    void TutorialSystemUpdate(float elapsedTime);
    void TutorialSystemStart();
    void TutorialSystemClear();
};