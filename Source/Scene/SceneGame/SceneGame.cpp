//一番上でインクルード（ネットワーク）
//#include <winsock2.h>
#include <ws2tcpip.h>

//#pragma comment(lib, "Ws2_32.lib")

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "SceneGame.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Animation\AimIKCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Character\NomuraCharacterCom.h"
#include "Component\Character\HaveAllAttackCharacter.h"
#include "Component\Character\RegisterChara.h"
#include "Component/Particle/CPUParticle.h"
#include "Component\Animation\FootIKcom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Camera/FPSCameraCom.h"
#include "Component/Camera/EventCameraCom.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include "Component/Sprite/Sprite.h"
#include "Component/Stage/StageEditorCom.h"
#include "Component/System/SpawnCom.h"
#include "Component/Enemy/BossCom.h"
#include "Component/Renderer/InstanceRendererCom.h"
#include "Component\MoveSystem\EasingMoveCom.h"
#include "Component\Renderer\DecalCom.h"
#include "Component\Collsion\PushBackCom.h"
#include "Component\UI\UiSystem.h"
#include "Component\UI\UiGauge.h"
#include "Component\System\HitProcessCom.h"
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Component/Camera/EventCameraManager.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include <Component/Character/CharaStatusCom.h>
#include "Phsix\Physxlib.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Audio/AudioSource.h"
#include "Scene\SceneResult\SceneResult.h"
#include "Component/Collsion/FrustumCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include <Component\UI\UiFlag.h>

SceneGame::~SceneGame()
{
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

// 初期化
void SceneGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region ゲームオブジェクトの設定

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
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();
    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //ステージ
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        //obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
        //obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/canyon/stage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/Model/canyon/stage.collision");
        obj->AddComponent<StageEditorCom>();
        /*      RigidBodyCom* rigid = obj->AddComponent<RigidBodyCom>(true, NodeCollsionCom::CollsionType::SPHER).get();
              rigid->GenerateCollider(r->GetModel()->GetResource());*/
    }

    //当たり判定用
    std::shared_ptr<GameObject> roboobj = GameObjectManager::Instance().Create();
    {
        //roboobj->SetName("robo");
        //roboobj->transform_->SetWorldPosition({ 0, 10, 0 });
        //roboobj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        //std::shared_ptr<RendererCom> r = roboobj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        //r->LoadModel("Data/OneCoin/robot.mdl");
        //std::shared_ptr<AnimationCom> a = roboobj->AddComponent<AnimationCom>();
        //a->PlayAnimation(0, true, false, 0.001f);

        //std::shared_ptr<SphereColliderCom> sphere = roboobj->AddComponent<SphereColliderCom>();
        //sphere->SetRadius(2.0f);
        //sphere->SetMyTag(COLLIDER_TAG::Enemy);
        //sphere->SetJudgeTag(COLLIDER_TAG::Player);

  /*      roboobj->AddComponent<NodeCollsionCom>("Data/OneCoin/OneCoin.nodecollsion");
        roboobj->AddComponent<RigidBodyCom>(false, NodeCollsionCom::CollsionType::SPHER);*/
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);

        //ウルト関係Obj追加
        {
            //アタック系ウルト
            std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
            ultAttckChild->SetName("UltAttackChild");
            //位置をカメラと一緒にする
            ultAttckChild->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });

            std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
            rayCol->SetEnabled(false);

            //ダメージ処理用
            std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
            hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
        }
        //アタックウルトのエフェクト
        {
            std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
            attackUltEff->SetName("attackUltEFF");
            attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
            attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
            std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 10000);
        }
    }

    //カメラをプレイヤーの子どもにして制御する
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico位置
        cameraPost->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());
    }

    //BOSS
#if(1)
    {
        auto& boss = GameObjectManager::Instance().Create();
        boss->SetName("BOSS");
        std::shared_ptr<RendererCom> r = boss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss.mdl");
        boss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
        boss->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        t = boss->transform_;
        boss->AddComponent<MovementCom>();
        boss->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = boss->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        boss->AddComponent<AnimationCom>();
        boss->AddComponent<BossCom>();
        boss->AddComponent<AimIKCom>(nullptr, "Boss_head");
        boss->AddComponent<CharaStatusCom>();
        std::shared_ptr<PushBackCom>pushBack = boss->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);

        //右足の煙エフェクト
        {
            std::shared_ptr<GameObject>rightfootsmokeobject = boss->AddChildObject();
            rightfootsmokeobject->SetName("rightfootsmokeeffect");
            std::shared_ptr<CPUParticle>rightfootsmokeeffect = rightfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 1000);
            rightfootsmokeeffect->SetActive(false);
        }

        //左足の煙エフェクト
        {
            std::shared_ptr<GameObject>leftfootsmokeobject = boss->AddChildObject();
            leftfootsmokeobject->SetName("leftfootsmokeeffect");
            std::shared_ptr<CPUParticle>leftfootsmokeeffect = leftfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 1000);
            leftfootsmokeeffect->SetActive(false);
        }

        //左手のゲームオブジェクト達
        {
            std::shared_ptr<GameObject> lefthandobject = boss->AddChildObject();
            lefthandobject->SetName("lefthand");
            std::shared_ptr<SphereColliderCom> lefthandcollider = lefthandobject->AddComponent<SphereColliderCom>();
            lefthandcollider->SetEnabled(false);
            lefthandcollider->SetMyTag(COLLIDER_TAG::Enemy);
            lefthandcollider->SetJudgeTag(COLLIDER_TAG::Player);
            lefthandcollider->SetRadius(1.0f);
            std::shared_ptr<CPUParticle>fireeffect = lefthandobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fire.cpuparticle", 500);
            fireeffect->SetActive(false);
            std::shared_ptr<GPUParticle>gpufireeffect = lefthandobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/firespark.gpuparticle", 4000);
            gpufireeffect->SetStop(true);
        }

        //右手のゲームオブジェクト達
        {
            std::shared_ptr<GameObject> righthandobject = boss->AddChildObject();
            righthandobject->SetName("righthand");
            std::shared_ptr<SphereColliderCom> righthandcollider = righthandobject->AddComponent<SphereColliderCom>();
            righthandcollider->SetEnabled(false);
            righthandcollider->SetMyTag(COLLIDER_TAG::Enemy);
            righthandcollider->SetJudgeTag(COLLIDER_TAG::Player);
            righthandcollider->SetRadius(1.0f);
            std::shared_ptr<CPUParticle>fireeffect = righthandobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fire.cpuparticle", 500);
            fireeffect->SetActive(false);
            std::shared_ptr<GPUParticle>gpufireeffect = righthandobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/firespark.gpuparticle", 4000);
            gpufireeffect->SetStop(true);
        }

        //生成オブジェクト
        {
            std::shared_ptr<GameObject> spawnobject = boss->AddChildObject();
            spawnobject->SetName("spawn");
            spawnobject->AddComponent<SpawnCom>("Data/SerializeData/SpawnData/missile.spawn");
            std::shared_ptr<GPUParticle>gpuparticle = spawnobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/gathermiddle.gpuparticle", 6000);
            gpuparticle->SetStop(true);
            std::shared_ptr<CPUParticle>shotsmoke = spawnobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/upshotsmoke.cpuparticle", 1000);
            shotsmoke->SetActive(false);
        }

        //チャージ攻撃
        {
            std::shared_ptr<GameObject> chargeobject = boss->AddChildObject();
            chargeobject->SetName("charge");
            std::shared_ptr<GPUParticle>gpuparticle = chargeobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beemgathermiddle.gpuparticle", 6000);
            gpuparticle->SetStop(true);
            chargeobject->AddComponent<SpawnCom>(nullptr);
        }
    }

#endif

    //UIゲームオブジェクト生成
    CreateUiObject();

#pragma endregion

#pragma region グラフィック系の設定
    //平行光源を追加
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    // スカイボックスの設定
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\snowy_hillside_4k.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

#pragma endregion

#pragma region オーディオ系の設定
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("Audio Test SE");
    std::shared_ptr<AudioSource> audio = obj->AddComponent<AudioSource>();
    audio->SetAudio(static_cast<int>(AUDIOID::SE));
    audio->Play(false, 0.5f);
    audio->SetAudioName("Test");

#pragma endregion

    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

// 終了化
void SceneGame::Finalize()
{
    photonNet->close();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    //if (n)
    //{
    //    n->Update();

    //    if (!n->IsNextFrame())
    //    {
    //        return;
    //    }
    //}

    photonNet->run(elapsedTime);

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //ボスの位置取得
    //sc->data.bossposiotn = t->GetLocalPosition();

    //遷移関係
    TransitionPVEFromResult();

    // ゲームオブジェクトの更新
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0〜1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());

    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();

    ImGui::Begin("Effect");
    EffectNew();
    ImGui::End();
}

//エフェクト生成
void SceneGame::EffectNew()
{
    if (ImGui::Button("cpuparticlenew"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testcpueffect");
        obj->AddComponent<CPUParticle>(nullptr, 1000);
    }
    ImGui::SameLine();
    if (ImGui::Button("gpuparticlenew"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testgpueffect");
        obj->AddComponent<GPUParticle>(nullptr, 10000);
    }

    if (ImGui::Button("EasingMoveObject"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testeasingobject");
        std::shared_ptr<CPUParticle>cpuparticle = obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fireball.cpuparticle", 1000);
        cpuparticle->SetActive(true);
        obj->AddComponent<EasingMoveCom>(nullptr);
    }
    ImGui::SameLine();
    if (ImGui::Button("UI"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testui");
        obj->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
    }
}

void SceneGame::CreateUiObject()
{
    //UI
    {
        //キャンバス
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //レティクル
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> reticle = canvas->AddChildObject();
            reticle->SetName("reticle");
            reticle->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Reticle.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //HpFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
            hpFrame->SetName("HpFrame");
            hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //HpGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpGauge = canvas->AddChildObject();
            hpGauge->SetName("HpGauge");
            std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, false, UiSystem::X_ONLY);
            gauge->SetMaxValue(200);
            float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }
        //HpMemori
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HpMemori");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpMemori.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostFrame");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostFrame2
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostFrame2");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostGauge");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltFrame");
            std::shared_ptr<UiSystem> fade = hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltFrame.ui", Sprite::SpriteShader::DEFALT, false);
            fade->SetFadeInFlag(true);
            fade->SetFadeTimer(10.0f);
        }

        //HideUltGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HideUltGauge");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HideUltGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltGauge");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("SkillFrame");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("SkillGauge");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //HitEffect
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HitEffect");

            bool* flag = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetIsHitAttack();
            hpMemori->AddComponent<UiFlag>("Data/SerializeData/UIData/Player/HitEffect.ui", Sprite::SpriteShader::DEFALT, false, flag);
        }
    }
}

void SceneGame::TransitionPVEFromResult()
{
    auto& boss = GameObjectManager::Instance().Find("BOSS");
    if (!boss)return;

    float hp = *boss->GetComponent<CharaStatusCom>()->GetHitPoint();

    if (hp <= 0)
    {
        if (!SceneManager::Instance().GetTransitionFlag())
            SceneManager::Instance().ChangeSceneDelay(new SceneResult, 5);
    }
}