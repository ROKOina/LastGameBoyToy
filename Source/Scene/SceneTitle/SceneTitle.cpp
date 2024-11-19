#include "Graphics/Graphics.h"
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
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "SceneTitle.h"
#include "Scene\SceneGame\SceneGame.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"
#include "Component\Character\RegisterChara.h"
#include "Component\Stage\StageEditorCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include <Component\Camera\FPSCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Camera\EventCameraManager.h>

SceneTitle::~SceneTitle()
{
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

void SceneTitle::Initialize()
{
    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        std::shared_ptr<PostEffect>posteffect = obj->AddComponent<PostEffect>();
        posteffect->SetIsMist(false);
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->transform_->SetWorldPosition({ -1.471f, 2.519f, -2.190f });
        freeCamera->transform_->SetEulerRotation({ 3.681f,391.512f,0.0f });
        std::shared_ptr<FreeCameraCom> camera = freeCamera->AddComponent<FreeCameraCom>();
        camera->SetFocusPos({ -0.114f,2.352f,0.023f });
        camera->SetFocus({ -0.427f,2.391f,-0.488f });
        camera->SetEye({ -1.471f,2.519f,-2.190f });
        camera->SetDistance(2.601f);
        camera->SetUpdate(false);
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>("Data/SerializeData/LightData/title.light");
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        StageEditorCom* stageEdit = obj->AddComponent<StageEditorCom>().get();
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/StageGimic.json");
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/player_True/player.mdl");
        std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
        anim->PlayAnimation(5, true);
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //キャンバス
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //タイトル
        {
            auto& title = obj->AddChildObject();
            title->SetName("title");
            title->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/title.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //プレイ
        {
            auto& next = obj->AddChildObject();
            next->SetName("next");
            next->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/next.ui", Sprite::SpriteShader::BLUR, true);
        }
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    // スカイボックスの設定
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\CosmicCoolCloudBottom.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);
}

void SceneTitle::Finalize()
{
}

void SceneTitle::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    UIUpdate(elapsedTime);

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneTitle::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();
}

void SceneTitle::UIUpdate(float elapsedTime)
{
    auto& canvas = GameObjectManager::Instance().Find("Canvas");
    if (!canvas)return;

    GamePad& gamePad = Input::Instance().GetGamePad();

    //ゲームシーンへ
    {
        auto& next = canvas->GetChildFind("next");
        auto& sprite = next->GetComponent<Sprite>();
        if (sprite->GetHitSprite())
        {
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
            {
                if (!SceneManager::Instance().GetTransitionFlag())
                {
                    //暗転
                    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, PostEffect::PostEffectParameter::Exposure);
                    SceneManager::Instance().ChangeSceneDelay(new SceneGame, 2);
                }
            }
        }
    }
}