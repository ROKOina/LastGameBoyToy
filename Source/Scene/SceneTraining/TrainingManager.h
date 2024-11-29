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