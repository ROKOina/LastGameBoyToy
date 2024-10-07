#pragma once
#include "./System/Component.h"
#include "./ColliderCom.h"
#include <List>

class StageEditorCom : public Component
{
public:

    // 名前取得
    const char* GetName() const override { return "StageEditor"; }

    // 開始処理
    void Start() {}

    // 更新処理
    void Update(float elapsedTime);

    // 破棄処理
    void OnDestroy() {}

    // GUI描画
    void OnGUI();

private:
    //エディターでいじるステージを選択
    void StageSelect();
    //配置するオブジェクトを登録
    void ObjectRegister();

    //ステージにオブジェクト配置
    void ObjectPlace();
    //マウスとステージの判定
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:
    bool onImGui = false;//カーソルがGui上にあるかどうか
    bool nowEdit = false;//編集中フラグ

    std::list<GameObj> objList;
};