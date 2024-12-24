#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"

#include "Component\System\GameObject.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Component\Audio\AudioCom.h"

//タイトルシーン
class SceneTitle :public Scene
{
public:
    SceneTitle() {};
    ~SceneTitle()override;

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneTitle"; };

private:

    //uiの更新
    void UIUpdate(float elapsedTime);

    //画面エフェクト実装
    void ScreenEffect(float elapsedTime);

    AudioCom* audioObj;
};