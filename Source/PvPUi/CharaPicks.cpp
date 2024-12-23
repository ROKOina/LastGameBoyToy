#include "CharaPicks.h"
#include "Input/Input.h"

#include "Component/Sprite/Sprite.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"

CharaPicks::CharaPicks()
{
    color          = { 1.0f, 1.0f, 1.0f, 1.0f };
    selectColor = { 0.3f, 0.3f, 0.3f, 1.0f };
}

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
                name->SetEnabled(false);
            }


            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill1.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
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
                name->SetEnabled(false);
            }

            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill2.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
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
                name->SetEnabled(false);
            }

            // スキル表記
            {
                auto& skill = chara->AddChildObject();
                skill->SetName("skill");
                skill->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/charaSkill3.ui", Sprite::SpriteShader::DEFALT, false);
                skill->SetEnabled(false);
            }

        }
    }

    // 決定ボタン
    {
        auto& decision = charaPicksCanvas->AddChildObject();
        decision->SetName("decision");
        decision->AddComponent<Sprite>("Data/SerializeData/UIData/CharaPick/decision.ui", Sprite::SpriteShader::DEFALT, true);
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

    // 決定処理
    DecisionButton();
}

// キャラ詳細
void CharaPicks::CharaDetails()
{
    // 決定していたら操作不可
    if (decisionFlg) return;

    GamePad& gamePad = Input::Instance().GetGamePad();
    auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");

    auto getCharacterInfo = [&](const std::string& charaName, int id) -> CharacterInfo {
        auto& chara = canvas->GetChildFind(charaName.c_str());
        return {
            chara,
            chara->GetComponent<Sprite>(),
            chara->GetChildFind("name"),
            chara->GetChildFind("skill"),
            id
        };
        };

    // キャラ、ID設定
    std::vector<CharacterInfo> characters = {
        getCharacterInfo("INAZAWA", 0),
        getCharacterInfo("FARAH", 1),
        getCharacterInfo("JANKRAT", 2),
        getCharacterInfo("chara4", 3)
    };

    // クリックするとスキル表示、キャラ名、選択キャラ、アイコンが表示
    auto handleCharacterSelection = [&](CharacterInfo& selected, std::vector<CharacterInfo>& others) {
        if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && selected.sprite->GetHitSprite()) {
            selected.name->SetEnabled(true);
            selected.sprite->spc.color = selectColor;
            // スキル表示はここのコメント解除してね！
            //selected.skill->SetEnabled(true);
            selectedCharacterId = selected.id;

            for (auto& other : others) {
                if (&other != &selected) {
                    other.name->SetEnabled(false);
                    other.skill->SetEnabled(false);
                    other.sprite->spc.color = color;
                }
            }
        }
        };

    for (size_t i = 0; i < characters.size(); ++i) {
        std::vector<CharacterInfo> others = characters;
        others.erase(others.begin() + i);
        handleCharacterSelection(characters[i], others);
    }
}

// 決定処理
void CharaPicks::DecisionButton()
{
    auto& canvas = GameObjectManager::Instance().Find("CharaPicksCanvas");
    auto& decisionButton = canvas->GetChildFind("decision");
    auto& sprite = decisionButton->GetComponent<Sprite>();

    // 決定ボタンが押され、かつキャラが選択されている場合のみ処理を実行
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (selectedCharacterId != -1 && GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown() && sprite->GetHitSprite())
    {
        decisionFlg = true;
        decisionButton->SetEnabled(false);
    }
}

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