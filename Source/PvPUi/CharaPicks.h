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

    //ステージピック表示設定
    void SetViewStagePicks(bool flg);

    // 選択されたキャラのIDを取得
    int GetSelectedCharacterId() const { return selectedCharacterId; }

    //確定したか
    bool IsDecisionFlg() const { return decisionFlg; }

    //チームが選んだキャラをセット
    void SetTeamPick(int pick) { teamPick = pick; }

    //ステージセレクト
    void StageSelect();

    //stageselectのenum
    enum class SelectStageKind
    {
        FIRST,
        SECOND,
        MAX
    };

    //ステージをのenumを中に格納する
    int getStagePick() const { return stagepick; }
    bool EndStagePick() const { return isStagePick; }
    void SetClientStagePick(int id) {   //マスターからの送信が来たら次に行く
        if (endStagePickClient)return;

        endStagePickClient = true;
        kind = SelectStageKind(id);
        stagepick = id;
    }
    SelectStageKind kind = SelectStageKind::FIRST;

    void SetMasterPlayer() { isMasterPlayer = true; }

private:

    // キャラ詳細（スキル表示、キャラ名、選択キャラ表示）
    void CharaDetails(float elapsedTime);

    //スプライトのヘルパー関数
    void UpdateSprite(Sprite* sprite, const DirectX::XMFLOAT2& defaultScale);

    // 決定処理
    void DecisionButton();

    //キャラのUI追加関数
    void AddCharacterUI(std::shared_ptr<GameObject> parent, const char* charaName,
        const char* iconPath, const char* namePath,
        const char* skillIconPath1, const char* skillIconPath2,
        const char* ultIconPath, const char* modelPath,
        const char* video1, const char* video2, const char* video3,
        const char* videoname1, const char* videoname2, const char* ultvideoname);
private:

    struct CharacterInfo
    {
        std::shared_ptr<GameObject> chara;
        std::shared_ptr<Sprite> sprite;
        std::shared_ptr<GameObject> name;
        std::shared_ptr<GameObject> charamodel;
        std::shared_ptr<GameObject> video1;
        std::shared_ptr<GameObject> video2;
        std::shared_ptr<GameObject> ultvideo;
        int id;
    };

    // 選択されたキャラのID
    int selectedCharacterId = -1;
    // 決定フラグ
    bool decisionFlg = false;

    //チームがピックしているキャラ
    int teamPick = -1;

    //ステージが選ばれていたら
    int stagepick = -1;
    bool isStagePick = false;

    bool endStagePickClient = false;
    bool isMasterPlayer = false;

    //経過時間
    float plustime = 0.0f;
    bool triger = false;

    DirectX::XMFLOAT4 color, selectColor = {};
};
