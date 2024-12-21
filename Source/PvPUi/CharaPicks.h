#pragma once
#include <memory>

// 前方宣言
class GameObject;
class Sprite;

class CharaPicks
{
public:
    CharaPicks() {}
    ~CharaPicks() {}

    // キャラピックUI生成
    void CreateCharaPicksUiObject();
    // 更新処理
    void CharaPicksUpdate(float elapsedTime);

    // キャラピック表示設定
    void SetViewCharaPicks(bool flg);

    // 選択されたキャラのIDを取得
    int GetSelectedCharacterId() const { return selectedCharacterId; }
private:
    // キャラ詳細（スキル表示、キャラ名、選択キャラ表示）
    void CharaDetails();
    // 決定処理
    void DecisionButton();
    // 時間制限
    void TimeLimitSystem(float elapsedTime);

private:

    struct CharacterInfo {
        std::shared_ptr<GameObject> chara;
        std::shared_ptr<Sprite> sprite;
        std::shared_ptr<GameObject> name;
        std::shared_ptr<GameObject> skill;
        int id;
    };

    // 選択されたキャラのID
    int selectedCharacterId = -1;
    // 決定フラグ
    bool decisionFlg = false;
};

