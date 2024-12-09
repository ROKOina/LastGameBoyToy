#pragma once

#include <vector>
#include <memory>

class GameObject;

class SettingScreen
{
public:
    SettingScreen() {}
    ~SettingScreen() {}

    //�ݒ�UI�𐶐�(�V�[���J�n�ɌĂԂƂ���)
    void CreateSettingUiObject();

    //�ݒ��ʍX�V
    void SettingScreenUpdate(float elaspedTime);

    bool IsViewSetting() { return viewSetting; }
    void SetViewSetting(bool flg);

    int GetSensitivity() { return sensitivity; }

private:
    bool viewSetting = false;   //���ڑ���֎~

    //�V�[�����ς��Ƃ����ɓ��������Z�b�g
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
    //�o�[���琔����ς���p�ɕۑ����Ă���
    std::vector<valueLimit> barObj;

    //���x�ۑ�
    int sensitivity = 30;

    //swich���p�ϐ�
    int state = 0;
};