#pragma once
#include <memory>
#include <DirectXMath.h>
#include <string>

// 前方宣言
class GameObject;
class Sprite;

class CharaPicks
{
public:
    CharaPicks();
    ~CharaPicks() {}

    // キャラピックUI生成
    void CreateCharaPicksUiObject();
    // 更新処理
    void CharaPicksUpdate(float elapsedTime);

    // キャラピック表示設定
    void SetViewCharaPicks(bool flg);

    // 選択されたキャラのIDを取得
    int GetSelectedCharacterId() const { return selectedCharacterId; }
    //確定したか
    bool IsDecisionFlg() const { return decisionFlg; }
    //チームが選んだキャラをセット
    void SetTeamPick(int pick) { teamPick = pick; }
private:
    // キャラ詳細（スキル表示、キャラ名、選択キャラ表示）
    void CharaDetails();
    // 決定処理
    void DecisionButton();

    //キャラのUI追加関数
    void AddCharacterUI(std::shared_ptr<GameObject> parent, const char* charaName,
        const char* iconPath, const char* namePath,
        const char* skillIconPath1, const char* skillIconPath2,
        const char* ultIconPath, const char* modelPath,
        const char* video1, const char* video2, const char* video3);
private:

    struct CharacterInfo
    {
        std::shared_ptr<GameObject> chara;
        std::shared_ptr<Sprite> sprite;
        std::shared_ptr<GameObject> name;
        std::shared_ptr<GameObject> charamodel;
        int id;
    };

    // 選択されたキャラのID
    int selectedCharacterId = -1;
    // 決定フラグ
    bool decisionFlg = false;

    //チームがピックしているキャラ
    int teamPick = -1;

    DirectX::XMFLOAT4 color, selectColor = {};
};
