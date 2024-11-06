#pragma once

#include <vector>
#include <memory>

class GameObject;

class SettingScreen
{
public:
    SettingScreen() {}
    ~SettingScreen() {}

    //設定UIを生成
    void CreateSettingUiObject();

    //設定画面更新
    void SettingScreenUpdate(float elaspedTime);

    bool IsViewSetting() { return viewSetting; }
    void SetViewSetting(bool flg);

    float uvX = 0;

private:
    bool viewSetting = false;   //直接操作禁止

    enum ValueID
    {
        MOUSESEN,
        FOV,
    };

    struct valueLimit
    {
        valueLimit(std::shared_ptr<GameObject> obj, ValueID id, float min = 0, float max = 100)
            :obj(obj), id(id), min(min), max(max)
        {}

        std::weak_ptr<GameObject> obj;
        ValueID id;
        float min = 0;
        float max = 100;
    };
    //バーから数字を変える用に保存しておく
    std::vector<valueLimit> barObj;

};