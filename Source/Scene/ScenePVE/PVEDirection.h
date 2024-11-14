#pragma once

#include "Component/Animation/AnimationCom.h"
#include "Component/System/Component.h"

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
private:
    //演出の統括
    void DirectionSupervision(float elapsedTime);

private:
    void DirectionOne(float elapsedTime);
    void DirectionTwo(float elapsedTime);
    void DirectionEnd(float elapsedTIme);

private:
    std::shared_ptr<TransformCom>t;
    std::weak_ptr<AnimationCom> animationCom;

    int directionNumber = 0;

    bool flag = false;
};
