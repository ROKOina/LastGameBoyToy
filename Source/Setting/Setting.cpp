#include "Setting.h"

#include "Component\System\GameObject.h"
#include "Component/Sprite/Sprite.h"
#include "Component/System/TransformCom.h"
#include "Math/Mathf.h"

void SettingScreen::CreateSettingUiObject()
{
    //settingCanvas
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("settingCanvas");

    //設定
    {
        auto& settingTitle = obj->AddChildObject();
        settingTitle->SetName("settingTitle");
        settingTitle->AddComponent<Sprite>("Data/SerializeData/UIData/setting/settingTitle.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //感度
    {
        auto& mouseSen = obj->AddChildObject();
        mouseSen->SetName("mouseSen");
        mouseSen->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSen.ui", Sprite::SpriteShader::DEFALT, false);

        //バー
        {
            auto& Bar = mouseSen->AddChildObject();
            Bar->SetName("Bar");
            Bar->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenBar.ui", Sprite::SpriteShader::DEFALT, true);

            //バーオブジェ追加
            valueLimit v(Bar, ValueID::MOUSESEN);
            barObj.emplace_back(v);

            //ボタン
            {
                auto& button = Bar->AddChildObject();
                button->SetName("Button");
                button->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenBarButton.ui", Sprite::SpriteShader::DEFALT, false);
            }

            //数字一の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num1");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum1.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //数字十の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num10");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum10.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //数字百の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num100");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/mouseSenNum100.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }

    //視野角
    {
        auto& fov = obj->AddChildObject();
        fov->SetName("fov");
        fov->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fov.ui", Sprite::SpriteShader::DEFALT, false);

        //バー
        {
            auto& Bar = fov->AddChildObject();
            Bar->SetName("Bar");
            Bar->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovBar.ui", Sprite::SpriteShader::DEFALT, true);

            //バーオブジェ追加
            valueLimit v(Bar, ValueID::FOV, 45, 110);
            barObj.emplace_back(v);

            //ボタン
            {
                auto& button = Bar->AddChildObject();
                button->SetName("Button");
                button->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovBarButton.ui", Sprite::SpriteShader::DEFALT, false);
            }

            //数字一の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num1");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum1.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //数字十の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num10");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum10.ui", Sprite::SpriteShader::DEFALT, false);
            }
            //数字百の位
            {
                auto& num = Bar->AddChildObject();
                num->SetName("num100");
                num->AddComponent<Sprite>("Data/SerializeData/UIData/setting/fovNum100.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }
}

void SettingScreen::SettingScreenUpdate(float elaspedTime)
{
    if (!viewSetting)return;

    //バーに応じて数値を変える
    for (auto& bar : barObj)
    {
        auto& obj = bar.obj.lock();
        if (!obj)continue;
        auto& barSprite = obj->GetComponent<Sprite>();
        if (!barSprite)continue;

        //バーが操作されていないならcontinue

        //数値を変える
        int value = Mathf::Lerp(bar.min, bar.max, uvX);
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

        //ID毎に処理
        switch (bar.id)
        {
        case ValueID::MOUSESEN: //マウス感度変更
        {
        }
        break;
        case ValueID::FOV:  //視野角変更
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

    //表示設定をする
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
