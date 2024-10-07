#pragma once

#include "Scene.h"
#include "Graphics\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Components\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Components/NodeCollsionCom.h"

#include "Components\ColliderCom.h"
#include <Windows.h>
#include <map>
#include "Netwark/Photon/BasicsApplication.h"

class StageObj;

// ゲームシーン
class SceneStageEditor : public Scene
{
public:
    SceneStageEditor() {}
    ~SceneStageEditor()override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneGame"; };

private:
    //imgui
    void ImGui();
    // モデル入力
    GameObj ImportModel(const char* filename);

    //エディターでいじるステージを選択
    void StageSelect();
    //配置するオブジェクトを登録
    void ObjectRegister();

    //ステージにオブジェクト配置
    void ObjectPlace();
    //マウスとステージの判定
    bool MouseVsStage(HitResult hit);

private:
    GameObj editorObj;
    GameObj stageObj;
    
    char registerObjName[256] = {};
    std::string selectObjName;
    std::map<std::string, std::string> gameObjcts;

    Light* mainDirectionalLight = nullptr;
};