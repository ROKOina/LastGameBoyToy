#include "CharaPicks.h"
#include "Component\System\GameObject.h"
#include "Component/Sprite/Sprite.h"

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
        }
        // FARAH
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("FARAH");
        }
        // JANKRAT
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("JANKRAT");
        }
        // 4
        {
            auto& chara = charaPicksCanvas->AddChildObject();
            chara->SetName("chara4");
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