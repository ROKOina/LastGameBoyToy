#pragma once

#include "../System\Component.h"

class RemoveTimerCom : public Component
{
public:
    RemoveTimerCom(float time) :time(time) {}
    ~RemoveTimerCom() {}

    //–¼‘OŽæ“¾
    const char* GetName()const override { return "RemoveTimer"; }

    void Update(float elapsedTime) override
    {
        timer += elapsedTime;
        if (time < timer)
            GameObjectManager::Instance().Remove(GetGameObject());
    }

private:
    float timer = 0;
    float time;
};