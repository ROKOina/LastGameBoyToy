#pragma once
#include "Component\System/Component.h"
#include <List>
#include <map>
#include <Math\Collision.h>
#include <functional>
#include "Component\Renderer\RendererCom.h"

using GenerateFunc = std::function<void(GameObj my)>;

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

    //Jsonのデータを元にオブジェクト配置
    void PlaceJsonData(std::string filename);

private:
    //配置するオブジェクトを登録
    void ObjectRegister();

    //ステージにオブジェクト配置
    GameObj ObjectPlace(
        std::string objType,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT4 rotation,
        const char* model_filename,
        const char* collision_filename
    );

    //ファイルパス取得
    void FileRead(std::string& path);
    //マウスとステージの判定
    bool MouseVsStage(HitResult& hit);

    void ObjectSave();
    void ObjectLoad();

private:
    //ゲームオブジェクト生成関数の名前
    enum class GenerateFuncName
    {
        None = 0,//設定しない

        TestNakanisi,
        TowerGimic,
        Max,
    };

    //ゲームオブジェクト生成関数
    static void TestNakanisi(GameObj place);
    static void TowerGimic(GameObj& place);

    GenerateFunc generateFunc[(int)GenerateFuncName::Max] =
    {
        nullptr, //None

        TestNakanisi,
        TowerGimic
    };

    //保存用のデータを格納する構造体
    struct PlaceObject
    {
        bool staticFlag = false;
        std::string filePath;
        std::string collisionPath;
        std::list<GameObj> objList;

        GenerateFuncName func;
    };

private:
    bool onImGui = false;//カーソルがGui上にあるかどうか
    bool nowEdit = false;//編集中フラグ

    std::map<std::string, PlaceObject> placeObjcts;//オブジェクトの名前とModleのファイルパスを紐づけて保存
    char registerObjName[256] = {};
    std::string objType;
    std::list<GameObj> objList;
};
