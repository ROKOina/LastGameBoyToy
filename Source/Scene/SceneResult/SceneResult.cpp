#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Animation\FootIKcom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "SceneResult.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component\PostEffect\PostEffect.h"

void SceneResult::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

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
        r->LoadModel("Data/Model/OneCoin/robot.mdl");
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
        r->LoadModel("Data/Model/IKTestStage/ExampleStage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/Model/IKTestStage/ExampleStage.collision");
    }

    //キャンバス
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        {
            auto& modoru = obj->AddChildObject();
            modoru->SetName("modoru");
            modoru->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/modoru.ui", Sprite::SpriteShader::DEFALT, true);
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