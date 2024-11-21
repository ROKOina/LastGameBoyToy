//一番上でインクルード（ネットワーク）
//#include <winsock2.h>
#include <ws2tcpip.h>

//#pragma comment(lib, "Ws2_32.lib")

#include "Graphics/Graphics.h"
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
#include "Component\UI\PlayerUI.h"
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
#include "Component\Renderer\TrailCom.h"
#include "Component\Light\LightCom.h"
#include "Component\Character\Prop\SetNodeWorldPosCom.h"
#include "Component\Audio\AudioCom.h"
#include "Setting/Setting.h"

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

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //オーディオ
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("Audio");
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
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.932f, 1.000f });
        r->SetOutlineIntensity(5.5f);
        obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
        StageEditorCom* stageEdit = obj->AddComponent<StageEditorCom>().get();
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/StageGimic.json");
        RigidBodyCom* rigid = obj->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Complex).get();
        rigid->SetUseResourcePath("Data/Model/MatuokaStage/StageJson/ColliderStage.mdl");
        rigid->SetNormalizeScale(1);
    }

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,-1,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);

        //ウルト関係Obj追加
        {
            //アタック系ウルト
            std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
            ultAttckChild->SetName("UltAttackChild");
            //位置をカメラと一緒にする
            ultAttckChild->transform_->SetWorldPosition({ 0, 8.0821f, 3.3050f });

            std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
            rayCol->SetEnabled(false);

            //ダメージ処理用
            std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
            hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
            hitDamage->SetValue(100);

            //キャラクターに登録
            obj->GetComponent<CharacterCom>()->SetAttackUltRayObj(ultAttckChild);
        }
        //アタックウルトのエフェクト
        {
            std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
            attackUltEff->SetName("attackUltEFF");
            std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 100);
            attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
            attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
            eff->Play();
        }
    }

    //カメラをプレイヤーの子どもにして制御する
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico位置
        cameraPost->transform_->SetWorldPosition({ 0, 12.086f, 3.3050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //腕
        {
            std::shared_ptr<GameObject> armChild = cameraPost->AddChildObject();
            armChild->SetName("armChild");
            armChild->transform_->SetScale({ 0.5f,0.5f,0.5f });
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,1.8f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");
            auto& anim = armChild->AddComponent<AnimationCom>();
            anim->PlayAnimation(0, false);

            //Eskill中エフェクト
            {
                std::shared_ptr<GameObject> eSkillEff = armChild->AddChildObject();
                eSkillEff->SetName("eSkillEff");
                std::shared_ptr<GPUParticle> eff = eSkillEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/InaESkill.gpuparticle", 100);
                eSkillEff->transform_->SetEulerRotation({ -7,-3,-80 });
                eSkillEff->transform_->SetLocalPosition({-0.35f,9.84f,-0.58f});
                eff->SetLoop(false);
            }
            //攻撃ため
            {
                std::shared_ptr<GameObject> chargeEff = armChild->AddChildObject();
                chargeEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeEff->SetName("chargeEff");
                std::shared_ptr<GPUParticle> eff = chargeEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playercharge.gpuparticle", 300);
                eff->SetLoop(false);
                //銃口にくっ付ける
                chargeEff->AddComponent<SetNodeWorldPosCom>();
            }
            //攻撃ためマックス
            {
                std::shared_ptr<GameObject> chargeMaxEff = armChild->AddChildObject();
                chargeMaxEff->transform_->SetLocalPosition({ 0.98f,12.44f,6.96f });
                chargeMaxEff->SetName("chargeMaxEff");
                std::shared_ptr<GPUParticle> eff = chargeMaxEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/playerchargeFull.gpuparticle", 300);
                eff->SetLoop(false);
                //銃口にくっ付ける
                chargeMaxEff->AddComponent<SetNodeWorldPosCom>();
            }
            //ウルトマズルフラッシュ
            {
                std::shared_ptr<GameObject> attackUltMuzzleEff = armChild->AddChildObject();
                attackUltMuzzleEff->transform_->SetLocalPosition({ -3.1f,12.94f,1.69f });
                attackUltMuzzleEff->SetName("attackUltMuzzleEff");
                std::shared_ptr<GPUParticle> eff = attackUltMuzzleEff->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltMuzzleF.gpuparticle", 20);
                eff->SetLoop(false);
            }
        }
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //BOSS
#if(1)
    {
        auto& boss = GameObjectManager::Instance().Create();
        boss->SetName("BOSS");
        std::shared_ptr<RendererCom> r = boss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/Boss/boss_ver2.mdl");
        r->SetOutlineColor({ 1,0,0 });
        r->SetOutlineIntensity(10.0f);
        boss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
        boss->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        t = boss->transform_;
        boss->AddComponent<MovementCom>();
        boss->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = boss->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        boss->AddComponent<AnimationCom>();
        auto& charaStatusCom = boss->AddComponent<CharaStatusCom>();
        charaStatusCom->SetInvincibleTime(0.1f);
        charaStatusCom->SetHitPoint(1000);
        charaStatusCom->SetMaxHitPoint(1000);
        boss->AddComponent<BossCom>();
        boss->AddComponent<AudioCom>();
        boss->AddComponent<AimIKCom>(nullptr, "Boss_spine_up");
        boss->AddComponent<AudioCom>();
        std::shared_ptr<PushBackCom>pushBack = boss->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);


        //右足の煙エフェクト
        {
            std::shared_ptr<GameObject>rightfootsmokeobject = boss->AddChildObject();
            rightfootsmokeobject->SetName("rightfootsmokeeffect");
            std::shared_ptr<CPUParticle>rightfootsmokeeffect = rightfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 600);
            rightfootsmokeeffect->SetActive(false);
        }

        //左足の煙エフェクト
        {
            std::shared_ptr<GameObject>leftfootsmokeobject = boss->AddChildObject();
            leftfootsmokeobject->SetName("leftfootsmokeeffect");
            std::shared_ptr<CPUParticle>leftfootsmokeeffect = leftfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 600);
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
            std::shared_ptr<GPUParticle>gpuparticle = spawnobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/gathermiddle.gpuparticle", 4000);
            gpuparticle->SetStop(true);
            std::shared_ptr<CPUParticle>shotsmoke = spawnobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/upshotsmoke.cpuparticle", 800);
            shotsmoke->SetActive(false);

            std::shared_ptr<GameObject> muzzleflashobject = spawnobject->AddChildObject();
            muzzleflashobject->SetName("muzzleflashleft");
            std::shared_ptr<CPUParticle>muzzleflash = muzzleflashobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/muzzleflash.cpuparticle", 500);
            muzzleflash->SetActive(false);
        }

        //チャージ攻撃
        {
            std::shared_ptr<GameObject> chargeobject = boss->AddChildObject();
            chargeobject->SetName("charge");
            std::shared_ptr<GPUParticle>gpuparticle = chargeobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/gathermiddle.gpuparticle", 4000);
            gpuparticle->SetStop(true);
            chargeobject->AddComponent<SpawnCom>("Data/SerializeData/SpawnData/beem.spawn");
            std::shared_ptr<CPUParticle>shotsmoke = chargeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/strateshotsmoke.cpuparticle", 800);
            shotsmoke->SetActive(false);

            std::shared_ptr<GameObject> muzzleflashobject = chargeobject->AddChildObject();
            muzzleflashobject->SetName("muzzleflash");
            std::shared_ptr<CPUParticle>muzzleflash = muzzleflashobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/muzzleflash.cpuparticle", 500);
            muzzleflash->SetActive(false);
        }

        //地面を叩き付ける攻撃
        {
            std::shared_ptr<GameObject> groundobject = boss->AddChildObject();
            groundobject->SetName("groundsmoke");
            std::shared_ptr<CPUParticle>smoke = groundobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/groundsmoke.cpuparticle", 1000);
            smoke->SetActive(false);
        }
    }

    //大技的なやつ
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("bomberexplosion");
        obj->transform_->SetWorldPosition({ -1.917f,23.375f,-32.530f });
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/bomberexplosion.gpuparticle", 6000);
        obj->AddComponent<SpawnCom>("Data/SerializeData/SpawnData/energyspawn.spawn");
    }

#endif

    //UIゲームオブジェクト生成
    CreateUiObject();

#pragma endregion

#pragma region グラフィック系の設定

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

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

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();

#ifdef _DEBUG
    ImGui::Begin("Effect");

    EffectNew();
    ImGui::End();
#endif
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
        obj->AddComponent<NodeCollsionCom>(nullptr);
        std::shared_ptr<Trail>trailcom = obj->AddComponent<Trail>("Data/SerializeData/TrailData/trajectory.trail");
        trailcom->SetTransform(obj->transform_->GetWorldTransform());
    }
    ImGui::SameLine();
    if (ImGui::Button("UI"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testui");
        obj->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
    }
    if (ImGui::Button("Light"))
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("testlight");
        obj->AddComponent<Light>(nullptr);
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
            std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
            std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
            hpGauge->SetName("HpGauge");
            std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
            gauge->SetMaxValue(GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
            float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }

        ////BoostFrame
        //{
        //    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        //    std::shared_ptr<GameObject> boostFrame = canvas->AddChildObject();
        //    boostFrame->SetName("BoostFrame");
        //    boostFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame.ui", Sprite::SpriteShader::DEFALT, false);
        //}

        ////BoostGauge
        //{
        //    std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Find("BoostFrame");
        //    std::shared_ptr<GameObject> BoostGauge = BoostFrame->AddChildObject();
        //    BoostGauge->SetName("BoostGauge");
        //    BoostGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame.ui", Sprite::SpriteShader::DEFALT, false);
        //}

        //UltFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltFrame");
            std::shared_ptr<UiSystem> fade = hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltHideGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> ultHideGauge = ultFrame->AddChildObject();
            ultHideGauge->SetName("UltHideGauge");
            ultHideGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltHideGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //UltGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> ultGauge = ultFrame->AddChildObject();
            ultGauge->SetName("UltGauge");

            std::shared_ptr<UI_Skill>ultGaugeCmp = ultGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false, 1084, 890);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            ultGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
            float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
            ultGaugeCmp->SetVariableValue(i);
        }

        ////////////<SKill_E>/////////////////////////////

      //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
            skillFrame->SetName("SkillFrame");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_01.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillMask
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGaugeHide");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrameHide1.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGauge");
            std::shared_ptr<UI_Skill>skillGauge = skillFrame->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge1.ui", Sprite::SpriteShader::DEFALT, false, 1084, 997);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");

            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetESkillCoolTime());
            float* i = player->GetComponent<CharacterCom>()->GetESkillCoolTimer();
            skillGauge->SetVariableValue(i);
        }

        //Skill_E
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_E");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        }

        ////////////////<Skill_Space>/////////////////////////////////////

             //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
            skillFrame->SetName("SkillFrame2");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillMask
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGaugeHide");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrameHide2.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGauge");
            std::shared_ptr<UI_Skill>skillGauge = skillFrame->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge2.ui", Sprite::SpriteShader::DEFALT, false, 1030, 937);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetSpaceSkillCoolTime());
            float* i = player->GetComponent<CharacterCom>()->GetSpaceSkillCoolTimer();
            skillGauge->SetVariableValue(i);
        }

        //Skill_Space
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_SPACE");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_SPACE.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //Boost
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("boostGauge2");

            hpMemori->AddComponent<UI_BoosGauge>(2);
        }
        //BossHpFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
            hpFrame->SetName("BossHpFrame");
            hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BossHpFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //BossHpGauge
        {
            std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("BossHpFrame");
            std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
            hpGauge->SetName("BossHpGauge");
            std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/BossHpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
            gauge->SetMaxValue(GameObjectManager::Instance().Find("BOSS")->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
            float* i = GameObjectManager::Instance().Find("BOSS")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }

        //LockOn
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("lockOn");

            hpMemori->AddComponent<UI_LockOn>(4);
        }
        //decoration
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Decoration");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Decoration.ui", Sprite::SpriteShader::DEFALT, false);
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