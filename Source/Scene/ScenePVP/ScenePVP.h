#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\System\GameObject.h"

#include "PVPGameSystem/PVPGameSystem.h"
// 前方宣言
class CharaPicks;

//リザルトシーン
class ScenePVP :public Scene
{
public:
    ScenePVP() {};
    ~ScenePVP()override {};

    // 初期化
    void Initialize()override;

    //初期化１（ロビー選択）
    void InitializeLobbySelect();
    //初期化２（ロビー）
    void InitializeLobby();
    //初期化３（キャラセレクト）
    void InitializeCharaSelect();
    //初期化４（PVP）
    void InitializePVP();

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "ScenePVP"; };

private:

    //オブジェクト生成関数
    void NewObject();

    //ゲームシステム更新
    void GameSystemUpdate(float elapsedTime);

    //画面に応じた更新処理
    void TransitionUpdate(float elapsedTime);

    //ロビー選択時
    void LobbySelectFontUpdate(float elapsedTime);  //fontUpdate
    //ロビー時
    void LobbyFontUpdate(float elapsedTime);    //fontUpdate
    //キャラセレクト時
    void CharaSelectUpdate(float elapsedTime);
    //ゲーム中時
    void GameUpdate(float elapsedTime);
    //背景Uodate
    void LobbyBackSprUpdate(float elapsedTime);
private:

    std::shared_ptr<CharaPicks>          charaPicks;
    std::unique_ptr<BasicsApplication> photonNet;

    bool isGame = false;
    bool isCharaSelect = false;
    bool isLobby = false;
    int lobbyState = 0; //0:ロビー選択   1:ロビー  2:キャラ選択  3:ゲーム中

    std::vector<std::weak_ptr<GameObject>> tempRemoveObj;   //画面切り替え時に削除するオブジェクト

    //ゲームシステム
    std::unique_ptr<PVPGameSystem> pvpGameSystem;

    struct LobbySelectFont
    {
        LobbySelectFont(int id, std::wstring str, DirectX::XMFLOAT2 pos, float scale, bool col, int state)
            :id(id), str(str), pos(pos), scale(scale), collision(col), state(state) {}
        int id;
        std::wstring str;
        DirectX::XMFLOAT2 pos;
        float scale;

        bool collision;
        int state;  //切り替えよう
    };
    //ロビー選択
    std::vector<LobbySelectFont> lobbySelectFont =
    {
        LobbySelectFont(0,L"新規部屋作成",{303,320},1.5f,true,0),
        LobbySelectFont(1,L"ロビー名",{465,223},1,false,1),
        LobbySelectFont(2,L"",{784,223},1,false,1),   //ロビー名打ち込み用
        LobbySelectFont(3,L"作成",{1203,624},1.5f,true,1),
        LobbySelectFont(4,L"戻る",{100,700},1.5f,true,1),

        LobbySelectFont(5,L"ロビー選択",{76,54},2,false,0),
        LobbySelectFont(6,L"新規部屋作成",{76,54},2,false,1),

        LobbySelectFont(10,L"部屋参加",{906,320},1.5f,true,0),
        LobbySelectFont(11,L"戻る",{100,700},1.5f,true,2),
        LobbySelectFont(12,L"入室",{1203,624},1.5f,true,2),

        LobbySelectFont(13,L"部屋参加",{76,54},2,false,2),

        //ロビー名用
        LobbySelectFont(20,L"",{536,218},1.0f,true,2),
        LobbySelectFont(21,L"",{536,218},1.0f,true,2),
        LobbySelectFont(22,L"",{536,218},1.0f,true,2),
        LobbySelectFont(23,L"",{536,218},1.0f,true,2),
        LobbySelectFont(24,L"",{536,218},1.0f,true,2),
    };
    //ロビー
    std::vector<LobbySelectFont> lobbyFont =
    {
        LobbySelectFont(0,L"ロビー",{76,54},2,false,0),
        LobbySelectFont(1,L"参加者",{455,210},1,false,0),
        LobbySelectFont(2,L"プレイ",{1303,747},2.5f,true,0),

        LobbySelectFont(10,L"ゲームモード",{1260,52},1.5f,false,0),
        LobbySelectFont(11,L"チームデスマッチ",{1435,201},1,true,0),
        LobbySelectFont(12,L"王冠",{1435,281},1,true,0),

        //ネット名用
        LobbySelectFont(20,L"",{536,218},1.0f,true,2),
        LobbySelectFont(21,L"",{536,218},1.0f,true,2),
        LobbySelectFont(22,L"",{536,218},1.0f,true,2),
        LobbySelectFont(23,L"",{536,218},1.0f,true,2),
    };

    int fontState = 0;
    int joinRoomCount = -1; //部屋参加時登録用
};
