#pragma once

#include "Scene/Scene.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\System\GameObject.h"

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

    //UIのゲームオブジェクト生成
    void CreateUiObject();

private:

    //オブジェクト生成関数
    void NewObject();

private:

    std::unique_ptr<BasicsApplication> photonNet;

    bool isCharaSelect = false;
    bool isLobby = false;
    //bool is = false;

    std::vector<std::weak_ptr<GameObject>> tempRemoveObj;   //画面切り替え時に削除するオブジェクト
};

void TransitionRemove();
