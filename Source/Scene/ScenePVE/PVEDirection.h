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
    void DirectionOne(float elapsedTime);
    void DirectionTwo(float elapsedTime);

private:
    std::shared_ptr<TransformCom>t;

    int directionNumber = 0;

    bool flag = false;
};
