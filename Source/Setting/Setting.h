#pragma once

#include <vector>
#include <memory>

class GameObject;

class SettingScreen
{
public:
    SettingScreen() {}
    ~SettingScreen() {}

    //設定UIを生成(シーン開始に呼ぶといい)
    void CreateSettingUiObject();

    //設定画面更新
    void SettingScreenUpdate(float elaspedTime);

    bool IsViewSetting() { return viewSetting; }
    void SetViewSetting(bool flg);

    int GetSensitivity() { return sensitivity; }

private:
    bool viewSetting = false;   //直接操作禁止

    //シーンが変わるとき等に内部をリセット
    void ResetObj();

    enum ValueID
    {
        MOUSESEN,
        FOV,
    };

    struct valueLimit
    {
        valueLimit(std::shared_ptr<GameObject> obj, ValueID id, float min = 0, float max = 100)
            :obj(obj), id(id), minValue(min), maxValue(max)
        {}

        std::weak_ptr<GameObject> obj;
        ValueID id;
        float minValue = 0;
        float maxValue = 100;
    };
    //バーから数字を変える用に保存しておく
    std::vector<valueLimit> barObj;

    //感度保存
    int sensitivity = 30;

    //swich文用変数
    int state = 0;
};