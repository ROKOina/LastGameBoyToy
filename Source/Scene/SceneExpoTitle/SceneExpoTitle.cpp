#include "SceneExpoTitle.h"

#include "Input\Input.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Scene/ScenePVE/ScenePVE.h"
#include "Scene\ScenePVP\ScenePVP.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"
#include "Component\Character\RegisterChara.h"
#include "Component\Stage\StageEditorCom.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Particle\GPUParticle.h"
#include <Component\Camera\FPSCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Camera\EventCameraManager.h>
#include "Scene\SceneTraining\SceneTraining.h"
#include "Component\Renderer\VideoCom.h"
#include "Component\UI\Font.h"
#include <Component\Event\EventMove.h>
#include "Component\Event\EventDirectCom.h"

SceneExpoTitle::~SceneExpoTitle()
{
}

void SceneExpoTitle::Initialize()
{
    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        std::shared_ptr<PostEffect>posteffect = obj->AddComponent<PostEffect>();
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->transform_->SetWorldPosition({ -2.394f, 0.644f, -2.916f });
        freeCamera->transform_->SetEulerRotation({ -12.959f,359.176f,0.0f });
        std::shared_ptr<FreeCameraCom> camera = freeCamera->AddComponent<FreeCameraCom>();
        camera->SetFocusPos({ -2.436f,1.322f,0.033f });
        camera->SetFocus({ -2.422f,1.092f,-0.967f });
        camera->SetEye({ -2.394f,0.644f,-2.916f });
        camera->SetDistance(3.026f);
        camera->SetUpdate(false);
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>("Data/SerializeData/LightData/title.light");
    }

    // イベント（テスト）
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("eventMove");
        obj->AddComponent<EventMove>("Data/Event/MoveParam/test.evm");
        obj->AddComponent<EventDirectCom>("Data/Event/testDirect.edm");
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.282f, 1.000f });
        r->SetOutlineIntensity(10.0f);
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
        obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/player_True/player1.mdl");
        r->SetOutlineColor({ 0.000f, 0.282f, 1.000f });
        r->SetOutlineIntensity(10.0f);
        std::shared_ptr<AnimationCom>anim = obj->AddComponent<AnimationCom>();
        anim->PlayAnimation(5, true);
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    ////キャンバス
    //{
    //    auto& obj = GameObjectManager::Instance().Create();
    //    obj->SetName("Canvas");

    //    //タイトル
    //    {
    //        auto& title = obj->AddChildObject();
    //        title->SetName("title");
    //        title->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/title.ui", Sprite::SpriteShader::CHROMATICABERRATION, false);
    //    }

    //    //PVE
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("PVE");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVE.ui", Sprite::SpriteShader::GLITCH, true);
    //    }
    //    //PVP
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("PVP");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVP.ui", Sprite::SpriteShader::GLITCH, true);
    //    }
    //    //トレーニング
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("Training");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/Training.ui", Sprite::SpriteShader::GLITCH, true);
    //    }

    //    //クレジット
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("credit");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/Credit.ui", Sprite::SpriteShader::GLITCH, true);
    //    }

    //    //ゲーム終了
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("endgame");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/endgame.ui", Sprite::SpriteShader::GLITCH, true);
    //    }

    //    //セレクト棒
    //    {
    //        auto& next = obj->AddChildObject();
    //        next->SetName("selectBow");
    //        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/selectBow.ui", Sprite::SpriteShader::DEFALT, false);
    //    }
    //}

    //クレジット
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("CreditVideo");
        auto& video = obj->AddComponent<Video>("Data/Video/StarWars.mp4");

        video->GetVidePram().pause();
        video->GetVidePram().SetisLoop(false);
        obj->transform_->SetWorldPosition({ -3.294,0.344,-1.816 });
        obj->transform_->SetScale({ 1.820,1.398,1.00 });
        obj->SetEnabled(false);
    }

    //spcaseでスキップ
    {
        auto& space = GameObjectManager::Instance().Create();
        space->SetName("Space");
        space->SetEnabled(false);
        auto& spaceFont = space->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024, Font::FontShader::DEFALT);
        spaceFont->position = { 1093.0f,940.0f };
        spaceFont->str = L"Space:スキップ";
        spaceFont->scale = 1.0f;
        spaceFont->color.w = 1.0f;

    }

    //Enterで早送り
    {
        auto& emter = GameObjectManager::Instance().Create();
        emter->SetName("Enter");
        emter->SetEnabled(false);
        auto& emterFont = emter->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024, Font::FontShader::DEFALT);
        emterFont->position = { 687.0f,940.0f };
        emterFont->str = L"Enter:早送り";
        emterFont->scale = 1.0f;
        emterFont->color.w = 1.0f;;
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    Audio2DMagaer::Instance().Audio2DPlay(titleAudioID, true);
    Audio2DMagaer::Instance().Audio2DStop(AUDIOID2D::SCENE_LOBBY);

    //暗転からはじまるように
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    auto& post = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>();
    post->SetExposureZero();    //暗転
    post->SetParameter(1.4f, 1.0f, parameters); //明転
}

void SceneExpoTitle::Finalize()
{
}

void SceneExpoTitle::Update(float elapsedTime)
{
    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneExpoTitle::Render(float elapsedTime)
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
