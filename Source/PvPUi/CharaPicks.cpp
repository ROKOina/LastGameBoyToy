#include "CharaPicks.h"
#include "Input/Input.h"

#include "Component/Sprite/Sprite.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"

// キャラピックUI生成
void CharaPicks::CreateCharaPicksUiObject()
{
    // キャラピックキャンパス
    auto& charaPicksCanvas = GameObjectManager::Instance().Create();
    charaPicksCanvas->SetName("CharaPicksCanvas");

    // 各キャラのUI設定
    {
        // INAZAWA
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("INAZAWA");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon0.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName0.ui", Sprite::SpriteShader::DEFALT, false);
                name->SetEnabled(false);
            }

            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill0.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
            }


        }
        // FARAH
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("FARAH");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon1.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName1.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }


            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill1.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

        }
        // JANKRAT
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("JANKRAT");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon2.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName2.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }

            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill2.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

        }
        // 4
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("chara4");
            chara->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaIcon3.ui", Sprite::SpriteShader::DEFALT, true);

            // 名前表記
            {
                auto& name = chara->AddChildObject();
                name->SetName("name");
                name->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaName3.ui", Sprite::SpriteShader::DEFALT, false);
                name->GetComponent<Sprite>()->SetEnabled(false);
            }

            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill3.ui", Sprite::SpriteShader::DEFALT, false);
                skill->GetComponent<Sprite>()->SetEnabled(false);
            }

        }
    }

    // 決定ボタン
    {
        auto& decision = charaPicksCanvas->AddChildObject();
        decision->SetName("decision");
    }

    // 時間制限
    {
        auto& timeLimit = charaPicksCanvas->AddChildObject();
        timeLimit->SetName("TimeLimit");
    }
}

// キャラピック更新処理
void CharaPicks::CharaPicksUpdate(float elapsedTime)
{
    // 各キャラ詳細
    CharaDetails();
    
    // 時間制限システム
    TimeLimitSystem(elapsedTime);
}

// キャラ詳細
void CharaPicks::CharaDetails()
{
    // アイコンが横並び
    // クリックするとスキル表示、キャラ名、選択キャラ、アイコンが表示
    GamePad& gamePad = Input::Instance().GetGamePad();

    // アイコンに触れていたら表示
    {
        auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");

        {
            auto& chara = canvas->GetChildFind("INAZAWA");
            auto& sprite = chara->GetComponent<Sprite>();

            auto& name = chara->GetChildFind("name");
            auto& skill = chara->GetChildFind("skill");

            // スプライト判定
            if (sprite->GetHitSprite())
            {
                name->SetEnabled(true);
                skill->SetEnabled(true);
            }
            else
            {
                name->SetEnabled(false);
                skill->SetEnabled(false);
            }
        }
    }


    if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
    {

    }
}

// 決定処理

// 時間制限システム
void CharaPicks::TimeLimitSystem(float elapsedTime)
{
    // 制限時間は設定できるように
    // 制限時間あり、なし切り替えれるようにする
}

// キャラピック表示設定
void CharaPicks::SetViewCharaPicks(bool flg)
{
    auto& charaPicksCanvas = GameObjectManager::Instance().Find("CharaPicksCanvas");
    if (!charaPicksCanvas) return;

    if (flg)
        charaPicksCanvas->SetEnabled(true);
    else
        charaPicksCanvas->SetEnabled(false);
}