#pragma once

#include <vector>
#include <memory>

class GameObject;

class SettingScreen
{
public:
    SettingScreen() {}
    ~SettingScreen() {}

    //�ݒ�UI�𐶐�
    void CreateSettingUiObject();

    //�ݒ��ʍX�V
    void SettingScreenUpdate(float elaspedTime);

    bool IsViewSetting() { return viewSetting; }
    void SetViewSetting(bool flg);

    float uvX = 0;

private:
    bool viewSetting = false;   //���ڑ���֎~

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
    //�o�[���琔����ς���p�ɕۑ����Ă���
    std::vector<valueLimit> barObj;

};