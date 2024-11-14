#include "Setting.h"

#include "Component\System\GameObject.h"
#include "Component/Sprite/Sprite.h"
#include "Component/System/TransformCom.h"
#include "Math/Mathf.h"
#include "Input\Input.h"

void SettingScreen::CreateSettingUiObject()
{
    ResetObj();

    //settingCanvas
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("settingCanvas");

    //�ݒ�
    {
        auto& settingTitle = obj->AddChildObject();
        settingTitle->SetName("settingTitle");
        settingTitle->AddComponent<Sprite>("Data/SerializeData/UIData/setting/settingTitle.ui", Sprite::SpriteShader::DEFALT, false);
    }

    std::vector<int> barInitial;

    //���x
    {
        auto& mouseSen = obj->AddChildObject();
        mouseSen->SetName("mouseSen");
        mouseSen->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSen.ui", Sprite::SpriteShader::DEFALT, false);

        //�o�[
        {
            auto& Bar = mouseSen->AddChildObject();
            Bar->SetName("Bar");
            Bar->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenBar.ui", Sprite::SpriteShader::DEFALT, true);

            //�o�[�I�u�W�F�ǉ�
            valueLimit v(Bar, ValueID::MOUSESEN);
            barObj.emplace_back(v);
            barInitial.emplace_back(sensitivity);

            //�{�^��
            {
                auto& button = Bar->AddChildObject();
                button->SetName("Button");
                button->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenBarButton.ui", Sprite::SpriteShader::DEFALT, false);
            }

            //������̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num1");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum1.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //�����\�̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num10");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum10.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //�����S�̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num100");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum100.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }

    //����p
    {
        auto& fov = obj->AddChildObject();
        fov->SetName("fov");
        fov->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fov.ui", Sprite::SpriteShader::DEFALT, true);

        //�o�[
        {
            auto& Bar = fov->AddChildObject();
            Bar->SetName("Bar");
            Bar->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovBar.ui", Sprite::SpriteShader::DEFALT, true);

            //�o�[�I�u�W�F�ǉ�
            valueLimit v(Bar, ValueID::FOV, 45, 110);
            barObj.emplace_back(v);
            int fov = 45;
            auto& cameraObj = GameObjectManager::Instance().Find("cameraPostPlayer");
            if (cameraObj)
            {
                auto& camera = cameraObj->GetComponent<CameraCom>();
                if (camera)
                    fov = camera->GetFov();
            }
            barInitial.emplace_back(fov);

            //�{�^��
            {
                auto& button = Bar->AddChildObject();
                button->SetName("Button");
                button->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovBarButton.ui", Sprite::SpriteShader::DEFALT, false);
            }

            //������̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num1");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum1.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //�����\�̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num10");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum10.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //�����S�̈�
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num100");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum100.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }

    //�{�^���ʒu�Ɛ���������
    int count = 0;
    for (auto& bar : barObj)
    {
        auto& obj = bar.obj.lock();
        if (!obj)continue;
        auto& barSprite = obj->GetComponent<Sprite>();
        if (!barSprite)continue;

        //�{�^���ʒu��ς���
        float sx = barSprite->spc.position.x;
        float sxMax = sx + barSprite->spc.texSize.x * barSprite->spc.scale.x;

        //��������ɂ���
        float barSizeX = sxMax - sx;

        //�䗦�����߂�
        float valuePivX = barInitial[count] - bar.minValue;
        float valueSizeX = bar.maxValue - bar.minValue;
        float vPer = valuePivX / valueSizeX;

        //�{�^���ʒu�ύX
        auto& button = obj->GetChildFind("Button");
        if (!button)continue;
        auto& buttonSprite = button->GetComponent<Sprite>();
        if (!buttonSprite)continue;
        buttonSprite->spc.position.x = sx + barSizeX * vPer
            + buttonSprite->spc.texSize.x * buttonSprite->spc.scale.x;


        //���l��ς���
        std::string names[3] = { "num1","num10","num100" };

        for (int i = 0; i < 3; ++i)
        {
            auto& num = obj->GetChildFind(names[i].c_str());
            if (!num)continue;
            auto& numSprite = num->GetComponent<Sprite>();
            if (!numSprite)continue;

            int order = barInitial[count];
            for (int i2 = 0; i2 < i; ++i2)
                order /= 10;
            order %= 10;

            numSprite->numUVScroll.x = 1.0f + (order / 10.0f - 0.1f);
        }
        count++;
    }
}

void SettingScreen::SettingScreenUpdate(float elaspedTime)
{
    if (!viewSetting)return;

    //�o�[�ɉ����Đ��l��ς���
    for (auto& bar : barObj)
    {
        auto& obj = bar.obj.lock();
        if (!obj)continue;
        auto& barSprite = obj->GetComponent<Sprite>();
        if (!barSprite)continue;

        //�o�[�����삳��Ă��Ȃ��Ȃ�continue
        if (!barSprite->GetHitSprite())continue;

        //�N���b�N��������Ă��Ȃ��Ȃ�continue
        GamePad& gamePad = Input::Instance().GetGamePad();
        if (!(GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButton()))continue;

        //bar�����Đ��l���X�V
        float sx = barSprite->spc.position.x;
        float sxMax = sx + barSprite->spc.texSize.x * barSprite->spc.scale.x;

        float mx = Input::Instance().GetMouse().GetPositionX();

        //��������ɂ���
        float barSizeX = sxMax - sx;
        float mousePosX = mx - sx;

        //�䗦�����߂�
        float per = mousePosX / barSizeX;

        //�{�^���ʒu�ύX
        auto& button = obj->GetChildFind("Button");
        if (!button)continue;
        auto& buttonSprite = button->GetComponent<Sprite>();
        if (!buttonSprite)continue;
        buttonSprite->spc.position.x = mx + buttonSprite->spc.texSize.x * buttonSprite->spc.scale.x;

        //���l��ς���
        int value = Mathf::Lerp(bar.minValue, bar.maxValue, per);
        std::string names[3] = { "num1","num10","num100" };

        for (int i = 0; i < 3; ++i)
        {
            auto& num = obj->GetChildFind(names[i].c_str());
            if (!num)continue;
            auto& numSprite = num->GetComponent<Sprite>();
            if (!numSprite)continue;

            int order = value;
            for (int i2 = 0; i2 < i; ++i2)
                order /= 10;
            order %= 10;

           numSprite->numUVScroll.x = 1.0f + (order / 10.0f - 0.1f);
        }

        //ID���ɏ���
        switch (bar.id)
        {
        case ValueID::MOUSESEN: //�}�E�X���x�ύX
        {
            sensitivity = value;
        }
        break;
        case ValueID::FOV:  //����p�ύX
        {
            auto& cameraObj = GameObjectManager::Instance().Find("cameraPostPlayer");
            if (!cameraObj)break;
            auto& camera = cameraObj->GetComponent<CameraCom>();
            
            camera->SetFov(value);
        }
        break;
        };
    }
}

void SettingScreen::SetViewSetting(bool flg)
{
    viewSetting = flg;
    auto& canvas = GameObjectManager::Instance().Find("settingCanvas");
    if (!canvas)return;

    //�\���ݒ������
    if (flg)
    {
        if (canvas)
            canvas->SetEnabled(true);
    }
    else
    {
        if (canvas)
            canvas->SetEnabled(false);
    }
}

void SettingScreen::ResetObj()
{
    barObj.clear();
    SetViewSetting(false);
}