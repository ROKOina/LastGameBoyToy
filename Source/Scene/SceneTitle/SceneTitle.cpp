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
#include "Scene\SceneSelect\SceneSelect.h"
#include "Scene/ScenePVE/ScenePVE.h"
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
#include "Component\Audio\AudioCom.h"

SceneTitle::~SceneTitle()
{
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

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.282f, 1.000f });
        r->SetOutlineIntensity(10.0f);
        StageEditorCom* stageEdit = obj->AddComponent<StageEditorCom>().get();
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ -0.191, 0.018, 1.802 });
        obj->transform_->SetScale({ 0.2f, 0.2f, 0.2f });
        obj->transform_->SetEulerRotation({ 0.0f,209.99f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/player_True/player.mdl");
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

    //キャンバス
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //タイトル
        {
            auto& title = obj->AddChildObject();
            title->SetName("title");
            title->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/title.ui", Sprite::SpriteShader::CHROMATICABERRATION, false);
        }

        //PVE
        {
            auto& next = obj->AddChildObject();
            next->SetName("PVE");
            next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVE.ui", Sprite::SpriteShader::DEFALT, true);
        }
        //PVP
        {
            auto& next = obj->AddChildObject();
            next->SetName("PVP");
            next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVP.ui", Sprite::SpriteShader::DEFALT, true);
        }
        //トレーニング
        {
            auto& next = obj->AddChildObject();
            next->SetName("Training");
            next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/Training.ui", Sprite::SpriteShader::DEFALT, true);
        }


        //セレクト棒
        {
            auto& next = obj->AddChildObject();
            next->SetName("selectBow");
            next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/selectBow.ui", Sprite::SpriteShader::DEFALT, false);
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

    {
        GameObj audio = GameObjectManager::Instance().Create();
        audio->SetName("Audio");
        audioObj = audio->AddComponent<AudioCom>().get();
        audioObj->RegisterSource(AUDIOID::SCENE_TITLE, "Title");
        audioObj->RegisterSource(AUDIOID::CURSOR, "Cursor");
        audioObj->RegisterSource(AUDIOID::ENTER, "Enter");

        audioObj->Play("Title", true, 0.0f);
        audioObj->FeedStart("Title", 0.5f, 0.1f);
    }
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
    //遷移フラグが立っている時は入らない
    if (SceneManager::Instance().GetTransitionFlag())return;

    auto& canvas = GameObjectManager::Instance().Find("Canvas");
    if (!canvas)return;

    //シーン遷移用構造体
    struct SceneName {
        SceneName(std::string name, Scene* scene, float y)
            :name(name), y(y) {
            this->scene = scene;
        }
        std::string name;   //当たり判定オブジェ
        Scene* scene;
        float y;
    };
    
    //初期化
    std::vector<SceneName> names;
    names.emplace_back("PVE" ,new ScenePVE,560);
    names.emplace_back("PVP", new ScenePVE, 750);
    names.emplace_back("Training", new ScenePVE, 940);

    GamePad& gamePad = Input::Instance().GetGamePad();
    auto& selectB = canvas->GetChildFind("selectBow");
    selectB->SetEnabled(false);
    //シーン遷移
    for(auto& s: names)
    {
        bool deleteFlg = false;
        auto& next = canvas->GetChildFind(s.name.c_str());
        auto& sprite = next->GetComponent<Sprite>();
        if (sprite->GetHitSprite())
        {
            //クリックで遷移＆演出
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
            {
                canvas->GetChildFind("title")->GetComponent<Sprite>()->EasingPlay();
                canvas->GetChildFind("PVE")->GetComponent<Sprite>()->EasingPlay();
                canvas->GetChildFind("PVP")->GetComponent<Sprite>()->EasingPlay();
                canvas->GetChildFind("Training")->GetComponent<Sprite>()->EasingPlay();
                if (!SceneManager::Instance().GetTransitionFlag())
                {
                    audioObj->FeedStart("Title", 0.0f, elapsedTime);
                    audioObj->Play("Enter", false, 1.0f);

                    //暗転
                    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
                    GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>()->SetParameter(0.0f, 4.0f, parameters);

                    SceneManager::Instance().ChangeSceneDelay(s.scene, 2);
                    deleteFlg = true;
                }
            }
            //セレクト棒壱変更
            if (sprite->GetHitSpriteEnter()) { audioObj->Stop("Cursor"); audioObj->Play("Cursor", false, 1.0f); }
            selectB->SetEnabled(true);
            DirectX::XMFLOAT3 sP = selectB->transform_->GetWorldPosition();
            sP.y = s.y;
            selectB->transform_->SetWorldPosition(sP);
        }
        //デリート
        if (!deleteFlg)
            delete  s.scene;
    }
    //棒消す
    if (SceneManager::Instance().GetTransitionFlag())selectB->SetEnabled(false);
}