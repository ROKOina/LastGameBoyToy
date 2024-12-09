#pragma once
#include "Component/System/Component.h"




//トレーニングの統括
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

//トレーニングモード
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

    void SetItemFlag(bool flag) { itemFlag = flag; }

private:
    //射撃の開始処理と終了処理
    void ShootingStartEndSystem();

    //射撃のロジック？
    void ShootingSystem(float elapsdTime);

    //射撃時の案山子スポーン
    void ShootingSpawnCrow();

    void SpawnItem();

    void SpawnItemSystem(float elapsdTime);

private:
    int     shootingScore = 0;                     //射撃のスコア
    int     scarecrowMaxTotal = 30;                //射撃時の最大案山子スポーン数
    int     scarecrowCount = 0;                    //射撃時の案山子スポーン
          
    float   scarecrowLifeTime = 1.0f;              //スポーンしてからの生存時間
    float   scarecrowLifeTimer = 0.0f;             //生存時間タイマー
    float   scarecrowSpawnIntervalTime  = 1.0f;    //案山子が倒されてからの次の案山子をスポーンの間隔
    float   scarecrowSpawnIntervalTimer = 0.0f;    //↑のタイマー
    float   spawnItemIntervalTime = 2.0f;          //アイテムが取得されてからの次のアイテムをスポーンさせる間隔
    float   spawnItemIntervalTimer = 0.0f;         //↑のタイマー

    bool    shootingStartFlag = false;             //射撃が開始か終了か　false=終了　True＝開始
    bool    shootingIntervalFlag = false;          //次の案山子をスポーンさせる信号
    bool    itemFlag = true;                      //アイテムがあるかどうか
};

//チュートリアルモード
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

public:
    //チュートリアルのID
    enum TutorialID
    { MOVE, GUN, SKILL, ULT, END };
public:
    //チュートリアル管理システム
    void TutorialManagerSystem(float elapsedTime);

    //チュートリアルを進める
    void NextTutorial(TutorialID id);
public:
    //動きのチュートリアル管理
    void MoveTutorialManager(float elapsedTime);

    //銃のチュートリアル管理
    void GunTutorialManager(float elapsedTime);

    //スキルのチュートリアル管理
    void SkillTutorialManager(float elapsedTime);

    //ウルトのチュートリアル管理
    void UltTutorialManager(float elapsedTime);
private:

    int tutorialID = 0;
    
                                           
    bool moveInspectionFlag = false;         //動きのチュートリアル検査
    bool gunInspectionFlag = false;          //銃のチュートリアル検査
    bool skillInspectionFlag = false;        //スキルのチュートリアル検査
    bool ultInspectionFlag = false;          //ウルトのチュートリアル検査
};