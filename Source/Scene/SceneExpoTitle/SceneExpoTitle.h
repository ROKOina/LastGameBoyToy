#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"

#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"
#include "Audio/Audio3D.h"

//タイトルシーン
class SceneExpoTitle :public Scene
{
public:
    SceneExpoTitle() {};
    ~SceneExpoTitle()override;

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "SceneExpoTitle"; };

private:
    AUDIOID2D titleAudioID = AUDIOID2D::SCENE_TITLE;
};