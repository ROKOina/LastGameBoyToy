#pragma once
#include "Component/System/Component.h"





class TrainingManager
{
private:
    TrainingManager();
    ~TrainingManager();
public:
    //唯一のインスタンス取得
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

    //最初はトレーニングモード
    bool tutorialFlag = false;

};

class TrainingSystem
{
private:
    TrainingSystem();
    ~TrainingSystem();
public:
    //唯一のインスタンス取得
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
    //射撃のロジック？
    void ShootingSystem(float elapsdTime);

    void ShootingRandomSpawn();

private:
    int     shootingScore = 0;
    float   scarecrowSpawnTime = 1.0f;  //倒してから1秒間待つ
    float   scarecrowSpawnTimer = 0.0f;
    float   scarecrowLifeTime = 1.0f;
    float   scarecrowLifeTimer = 0.0f;
    float   scarecrowSpawnIntervalTime  = 1.0f;
    float   scarecrowSpawnIntervalTimer = 0.0f;
    DirectX::XMFLOAT3 randomPos = { 0,0,0 };
    bool    shootingStartFlag = true;
    bool    shootingIntervalFlag = false;


    //難易度変化させるか？
    //分からん

};

class TutorialSystem
{
private:
    TutorialSystem();
    ~TutorialSystem();
public:
    //唯一のインスタンス取得
    static TutorialSystem& Instance()
    {
        static TutorialSystem instance;
        return instance;
    }

    void TutorialSystemUpdate(float elapsedTime);
    void TutorialSystemStart();
    void TutorialSystemClear();
};