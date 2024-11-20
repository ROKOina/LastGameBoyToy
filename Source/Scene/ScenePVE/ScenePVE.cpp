#include "ScenePVE.h"
#include <Graphics\Graphics.h>
#include <Component\Camera\FreeCameraCom.h>
#include <Component\Camera\EventCameraCom.h>
#include <Component\Character\RegisterChara.h>
#include <Component\Camera\FPSCameraCom.h>
#include <Component\Collsion\ColliderCom.h>
#include <Component\Camera\EventCameraManager.h>
#include <Input\Input.h>
#include <Component\Character\CharacterCom.h>
#include <Component\Animation\AnimationCom.h>
#include <StateMachine\Behaviar\BossState.h>
#include <Component\System\SpawnCom.h>
#include "PVEDirection.h"
#include <Component\Stage\StageEditorCom.h>
#include "Component\Phsix\RigidBodyCom.h"
#include <Component\Collsion\RayCollisionCom.h>
#include <Component\MoveSystem\EasingMoveCom.h>
#include "Component\Collsion\PushBackCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Component\Enemy\BossCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"
#include <Component/System/HitProcessCom.h>
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include <Component/UI/UiSystem.h>
#include <Component/UI/PlayerUI.h>
#include <Component/UI/UiFlag.h>
#include <Component/UI/UiGauge.h>

void ScenePVE::Initialize()
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
        }
        //アタックウルトのエフェクト
        {
            std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
            attackUltEff->SetName("attackUltEFF");
            attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
            attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
            std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 500);
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
            armChild->transform_->SetLocalPosition({ 1.67f,-6.74f,0.20f });
            std::shared_ptr<RendererCom> r = armChild->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");
            auto& anim = armChild->AddComponent<AnimationCom>();
            anim->PlayAnimation(0, false);
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
        boss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
        boss->transform_->SetScale({ 0.23f, 0.23f, 0.23f });
        t = boss->transform_;
        boss->AddComponent<MovementCom>();
        boss->AddComponent<NodeCollsionCom>("Data/Model/Boss/boss.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = boss->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        boss->AddComponent<AnimationCom>();
        boss->AddComponent<CharaStatusCom>();
        boss->AddComponent<BossCom>();
        boss->AddComponent<AimIKCom>(nullptr, "Boss_spine_up");
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

    Audio::Instance().RegisterAudioSources(AUDIOID::SceneGame1, "Data/AudioData/SceneGameBGM/BossBattle_start.wav");
    Audio::Instance().RegisterAudioSources(AUDIOID::SceneGame2, "Data/AudioData/SceneGameBGM/BossBattle_clymax.wav");
    bgmSource_start.SetAudio((int)AUDIOID::SceneGame1);
    bgmSource_clymax.SetAudio((int)AUDIOID::SceneGame2);


    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);

    PVEDirection::Instance().DirectionStart();
}

void ScenePVE::Finalize()
{
    photonNet->close();
}

static float v = 5.0f;
void ScenePVE::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    photonNet->run(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
    PVEDirection::Instance().Update(elapsedTime);

    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);


    v -= elapsedTime;
    bgmSource_start.Play(true, v);

    //bgmSource_start.GetSourceVoice()->SetVolume(rand() % 10);

    GameObj boss = GameObjectManager::Instance().Find("BOSS");
    if (boss != nullptr && *(boss->GetComponent<CharaStatusCom>()->GetHitPoint()) < 20.0f)
    {
        bgmSource_start.Stop();
        bgmSource_clymax.Play(true,10.0f);
    }
}

void ScenePVE::Render(float elapsedTime)
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

    photonNet->ImGui();

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();

    ImGui::Begin("Effect");

    //EffectNew();
    ImGui::End();
}

void ScenePVE::CreateUiObject()
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
            gauge->SetMaxValue(200);
            float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }

        ////BoostFrame
        //{
        //    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        //    std::shared_ptr<GameObject> boostFrame = canvas->AddChildObject();
        //    boostFrame->SetName("BoostFrame");
        //    boostFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_01.ui", Sprite::SpriteShader::DEFALT, false);
        //}

        ////BoostFrame2
        //{
        //    std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Find("BoostFrame");
        //    std::shared_ptr<GameObject> BoostFrame2 = BoostFrame->AddChildObject();
        //    BoostFrame2->SetName("BoostFrame2");
        //    BoostFrame2->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_02.ui", Sprite::SpriteShader::DEFALT, false);
        //}

        ////BoostGauge
        //{
        //    std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Find("BoostFrame");
        //    std::shared_ptr<GameObject> BoostGauge = BoostFrame->AddChildObject();
        //    BoostGauge->SetName("BoostGauge");
        //    BoostGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGauge.ui", Sprite::SpriteShader::DEFALT, false);
        //}

        //UltFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltFrame");
            std::shared_ptr<UiSystem> fade = hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltFrame.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //HideUltGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> hideUlt = ultFrame->AddChildObject();
            hideUlt->SetName("HideUltGauge");
            std::shared_ptr<UiGauge>gauge = hideUlt->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HideUltGauge.ui", Sprite::SpriteShader::DEFALT, false, UiSystem::Y_ONLY_SUB);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            gauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
            float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
            gauge->SetVariableValue(i);
        }

        //UltGauge
        {
            std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
            std::shared_ptr<GameObject> ultGauge = ultFrame->AddChildObject();
            ultGauge->SetName("UltGauge");
            ultGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
            skillFrame->SetName("SkillFrame2");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_01.ui", Sprite::SpriteShader::DEFALT, false);
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
            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
            float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
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


        //Skill_E
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame2");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_E");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
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