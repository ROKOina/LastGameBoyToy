#pragma once

#include "Component/Animation/AnimationCom.h"
#include "Component/System/Component.h"
#include "PvPUi/CharaPicks.h"
//PVEの演出統括
class PVEDirection
{
private:
    PVEDirection();
    ~PVEDirection();
public:
    //唯一のインスタンス取得
    static PVEDirection& Instance()
    {
        static PVEDirection instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime);
    void DirectionStart();
    int GetDirectionNum() { return directionNumber; }

    void DirectionEnd();

    bool GetCharaSelect() { return CharaSelectFlag; }
    void SetCharaSelect(bool flag) { CharaSelectFlag = flag; }

private:
    //演出の統括
    void DirectionSupervision(float elapsedTime);

    //Setting
    void Setting();

    //背景初期化
    void InitializeBack();

    void InitializeChara();
private:

    void CharaSlect(float elapsedTime);


    void DirectionFOne(float elapsedTime);
    void DirectionFTwo(float elapsedTime);
    void DirectionFEnd(float elapsedTIme);

    void DirectionCOne(float elapsedTime);
    void DirectionCTwo(float elapsedTime);
    void DirectionCThi(float elapsedTime);
    void DirectionCFou(float elapsedTime);
    void DirectionCEnd(float elapsedTime);

private:
    //GameObj eventBoss;

    std::shared_ptr<TransformCom>t;
    std::weak_ptr<AnimationCom> animationCom;
    std::shared_ptr<CharaPicks>   charaPicks;  //PVPのキャラ選択をここでも使用
    std::vector<std::weak_ptr<GameObject>> tempRemoveObj;   //画面切り替え時に削除するオブジェクト
    std::string charName[4] = { "Kanizo-Player","FaraicPlayer","SantorattoPlayer","Matya-Player"};

    int directionNumber = 0;

    float deleyTimer = 0;
    float deleyTime = 2.0f;
    bool flag = false; 

    bool deleyFlag = false;

    bool CharaSelectFlag = false;
};
