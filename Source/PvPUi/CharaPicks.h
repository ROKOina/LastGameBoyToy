#pragma once

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

private:
    // キャラ詳細（スキル表示、キャラ名、選択キャラ表示）
    void CharaDetails();

    // キャラアイコン、決定処理
    

    // 時間制限
    void TimeLimitSystem(float elapsedTime);

};

