#pragma once

#include "Scene\Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Graphics/Light/Light.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

class SettingScreen;

// ゲームシーン
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override;

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //エフェクト生成
    void EffectNew();

    //名前取得
    std::string GetName() const override { return "SceneGame"; };

private:

    //UIのゲームオブジェクト生成
    void CreateUiObject();

    //PVEゲームからリザルトに遷移
    void TransitionPVEFromResult();
private:
    //std::unique_ptr<NetwarkPost> n;
    std::unique_ptr<BasicsApplication> photonNet;
    Light* mainDirectionalLight = nullptr;

private:

    std::shared_ptr<TransformCom>t;

    //設定画面
    std::shared_ptr<SettingScreen> ss;
};