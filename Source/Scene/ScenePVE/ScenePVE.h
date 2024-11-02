#pragma once

#include "Scene/Scene.h"
#include "Component\Sprite\Sprite.h"
#include "Audio\AudioSource.h"
#include "Audio\Audio.h"
#include "Component\System\GameObject.h"
#include "Netwark/NetwarkPost.h"
#include "Graphics/Light/Light.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Netwark/Photon/BasicsApplication.h"

//リザルトシーン
class ScenePVE :public Scene
{
public:
    ScenePVE() {};
    ~ScenePVE()override {};

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    //名前取得
    std::string GetName() const override { return "ScenePVE"; };
private:
    Light* mainDirectionalLight = nullptr;
    std::unique_ptr<BasicsApplication> photonNet;
    std::shared_ptr<TransformCom>t;
};
