//一番上でインクルード（ネットワーク）
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Netwark/Photon/StaticSendDataManager.h"

#include "Graphics/Graphics.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"

#include "imgui.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneSugimoto.h"
#include "Scene\SceneResult\SceneResult.h"
#include "../SceneManager.h"
#include "../SceneLoading/SceneLoading.h"

#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include "Component/PostEffect/PostEffect.h"
#include "Component/Collsion/RayCollisionCom.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Camera/FPSCameraCom.h"
#include "Component/Camera/EventCameraCom.h"
#include "Component/Camera/EventCameraManager.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/Renderer/InstanceRendererCom.h"
#include "Component\Light\LightCom.h"

#include "Phsix\Physxlib.h"

#include "Audio/AudioSource.h"

SceneSugimoto::~SceneSugimoto()
{
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

// 初期化
void SceneSugimoto::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region ゲームオブジェクトの設定

    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        auto& p = obj->AddComponent<PostEffect>();
        p->SetParameter(3.0f, 0.8f, PostEffect::PostEffectParameter::Exposure);
    }

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    // ムツキ
    {
        //auto& obj = GameObjectManager::Instance().Create();
        //obj->SetName("Mutsuki");
        //obj->transform_->SetWorldPosition({ -18.138f,-5.9f,7.0f });
        //obj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        //obj->transform_->SetEulerRotation({ 0.0f,48.0f, 0.0f });

        //std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::GHOST_BLUR_TOON, BLENDSTATE::MULTIPLERENDERTARGETS);
        //r->LoadModel("Data/Model/Mutsuki/Mutsuki.mdl");
        //r->SetRenderShadow(true);
        //r->SetRenderSilhoutte(false);

        //auto& cb = r->SetVariousConstant<GhostBlurConstants>();
        //cb->SetRendererCom(r);
        //cb->blurThreshold = 0.85f;
        //cb->samplingRate = 0.01f;

        //auto& anim = obj->AddComponent<AnimationCom>();
        //anim->PlayAnimation(0, true);
    }

    // コダック
    {
        //auto& obj = GameObjectManager::Instance().Create();
        //obj->SetName("codack");
        //obj->transform_->SetWorldPosition({ -14.138f,-5.9f,0.505f });
        //obj->transform_->SetScale({ 0.03f,0.000001f,0.027f });
        //obj->transform_->SetEulerRotation({ 90.0f,0.0f,0.0f });
        //std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::FAKE_DEPTH, BLENDSTATE::MULTIPLERENDERTARGETS);
        //r->LoadModel("Data/Model/cube/cube.mdl");
        //r->LoadMaterial("Data/Model/cube/Smile.material");
        //r->SetRenderShadow(false);
        //r->SetRenderSilhoutte(false);
    }

    // Fake Interior
    {
        //auto& instanceOwner = GameObjectManager::Instance().Create();
        //instanceOwner->SetName("Cube Instance Owner");
        //instanceOwner->transform_->SetWorldPosition({ -15.0f,-4.6f,-4.0f });
        //instanceOwner->transform_->SetScale({ 0.07f,0.02f,0.02f });
        //std::shared_ptr<InstanceRenderer> ir = instanceOwner->AddComponent<InstanceRenderer>(SHADER_ID_MODEL::FAKE_INTERIOR, 10, BLENDSTATE::MULTIPLERENDERTARGETS);
        //ir->LoadModel("Data/Model/cube/cube.mdl");
        //ir->LoadMaterial("Data/Model/cube/interior.material");

        //auto& cb = ir->SetVariousConstant<FakeInteriorConstants>();
        //cb->reflectionAmount = 0.0f;
        //cb->offset = 1.08f;

        ////窓の作成
        //float startZ = -140;
        //float offsetZ = 140;
        //for (int i = 0; i < 6; ++i)
        //{
        //  auto& obj = ir->CreateInstance(true);
        //  obj->SetName(("window " + std::to_string(i)).c_str());
        //  obj->transform_->SetLocalPosition({ 0.0f,(i / 3) * 140.0f, startZ + (i % 3) * offsetZ });
        //}

        // ビル本体
        //{
        //  auto& obj = GameObjectManager::Instance().Create();
        //  obj->SetName("House");
        //  instanceOwner->AddChildObject(obj);
        //  obj->transform_->SetLocalPosition({ 0.0f,67.0f,0.0f });
        //  obj->transform_->SetScale({ 0.999f,3.0f,4.5f });

        //  std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        //  r->LoadModel("Data/Model/cube/cube.mdl");
        //  r->LoadMaterial("Data/Model/cube/House.material");
        //  r->SetRenderSilhoutte(false);
        //}
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/canyon/stage.mdl");
    }

#pragma endregion

#pragma region グラフィック系の設定

    // スカイボックスの設定
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\CosmicCoolCloudBottom.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

#pragma endregion
}

// 終了化
void SceneSugimoto::Finalize()
{
}

// 更新処理
void SceneSugimoto::Update(float elapsedTime)
{
    // ゲームオブジェクトの更新
    GameObjectManager::Instance().Update(elapsedTime);
    GameObjectManager::Instance().UpdateTransform();
}

// 描画処理
void SceneSugimoto::Render(float elapsedTime)
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
}

void SceneSugimoto::SetUserInputs()
{
    // プレイヤーの入力情報
    SetPlayerInput();

    // 他のプレイヤーの入力情報
    SetOnlineInput();
}

void SceneSugimoto::SetPlayerInput()
{
}

void SceneSugimoto::SetOnlineInput()
{
}

void SceneSugimoto::DelayOnlineInput()
{
}

void SceneSugimoto::CreateUiObject()
{
}

void SceneSugimoto::TransitionPVEFromResult()
{
}