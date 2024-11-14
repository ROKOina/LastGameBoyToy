#pragma once

#include "Component/Animation/AnimationCom.h"
#include "Component/System/Component.h"

//PVE�̉��o����
class PVEDirection
{
private:
    PVEDirection();
    ~PVEDirection();
public:
    //�B��̃C���X�^���X�擾
    static PVEDirection& Instance()
    {
        static PVEDirection instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);
    void DirectionStart();
private:
    //���o�̓���
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
