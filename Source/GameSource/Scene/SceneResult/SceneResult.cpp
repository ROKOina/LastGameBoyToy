#include "../Source/GameSource/Scene/SceneResult/SceneResult.h"
#include "../Source/GameSource/Scene/SceneTitle/SceneTitle.h"

#include "Graphics/Light/LightManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "../Source/GameSource/Scene/SceneManager.h"
#include "../Source/GameSource/Scene/SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\FootIKcom.h"
#include "Components/CPUParticle.h"
#include "Components\RayCollisionCom.h"

#include "GameSource/GameScript/FreeCameraCom.h"

#include "Graphics/Sprite/Sprite.h"

void SceneResult::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
        std::shared_ptr<FootIKCom> f = obj->AddComponent<FootIKCom>();
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0.0f, 0 });
        obj->transform_->SetScale({ 0.6f, 0.6f, 0.6f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/IKTestStage/ExampleStage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/IKTestStage/ExampleStage.collision");
    }

    //キャンバス
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        {
            auto& modoru = obj->AddChildObject();
            modoru->SetName("modoru");
            modoru->AddComponent<Sprite>("Data/resultScene/UI/modoru.ui", Sprite::SpriteShader::DEFALT, true);
        }
    }

    //平行光源を追加
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

void SceneResult::Finalize()
{
}

void SceneResult::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    UIUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneResult::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());
}

void SceneResult::UIUpdate(float elapsedTime)
{
    auto& canvas = GameObjectManager::Instance().Find("Canvas");
    if (!canvas)return;

    //戻る
    {
        auto& modoru = canvas->GetChildFind("modoru");
        auto& sprite = modoru->GetComponent<Sprite>();
        if (sprite->GetHitSprite())
        {
            GamePad& gamePad = Input::Instance().GetGamePad();
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
            {
                SceneManager::Instance().ChangeScene(new SceneTitle);
            }
        }
    }
}