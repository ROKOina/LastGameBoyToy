#pragma once
#include "./System/Component.h"
#include "./ColliderCom.h"
#include <List>
#include <map>

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

    //配置するオブジェクトを登録
    void ObjectRegister();

    //マウスとステージの判定
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:

    struct PlaceObject
    {
        std::string filePath;
        std::list<GameObj> objList;
    };

private:
    bool onImGui = false;//カーソルがGui上にあるかどうか
    bool nowEdit = false;//編集中フラグ

    std::map<std::string, PlaceObject> placeObjcts;//オブジェクトの名前とModleのファイルパスを紐づけて保存
    char registerObjName[256] = {};
    std::string objType;
    std::list<GameObj> objList;
};