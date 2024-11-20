#pragma once

#include "Component/Animation/AnimationCom.h"
#include "Component/System/Component.h"

//PVEÌo
class PVEDirection
{
private:
    PVEDirection();
    ~PVEDirection();
public:
    //BêÌCX^Xæ¾
    static PVEDirection& Instance()
    {
        static PVEDirection instance;
        return instance;
    }

    //XV
    void Update(float elapsedTime);
    void DirectionStart();
private:
    //oÌ
    void DirectionSupervision(float elapsedTime);

    

private:
    void DirectionFOne(float elapsedTime);
    void DirectionFTwo(float elapsedTime);
    void DirectionFEnd(float elapsedTIme);

    void DirectionCOne(float elapsedTime);
    void DirectionCTwo(float elapsedTime);
    void DirectionCThi(float elapsedTime);
    void DirectionCFou(float elapsedTime);
    void DirectionCEnd(float elapsedTime);

private:
    GameObj eventBoss;

    std::shared_ptr<TransformCom>t;
    std::weak_ptr<AnimationCom> animationCom;

    int directionNumber = 0;

    float deleyTimer = 0;
    float deleyTime = 2.0f;
    bool flag = false; 
};
