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
        obj->AddComponent<StageEditorCom>();
        RigidBodyCom* rigid = obj->AddComponent<RigidBodyCom>(true, RigidBodyCom::RigidType::Complex).get();
        rigid->SetUseResourcePath("Data/Model/MatuokaStage/StageJson/ColliderStage.mdl");
        rigid->SetNormalizeScale(1);
    }

    //当たり判定用
    //std::shared_ptr<GameObject> roboobj = GameObjectManager::Instance().Create();
    //{
    //    roboobj->SetName("robo");
    //    roboobj->transform_->SetWorldPosition({ 0, 3.0f, 0 });
    //    roboobj->transform_->SetScale({ 3,3,3 });
    //    std::shared_ptr<RendererCom> r = roboobj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    //    r->LoadModel("Data/OneCoin/robot.mdl");
    //    std::shared_ptr<AnimationCom> a = roboobj->AddComponent<AnimationCom>();
    //    a->PlayAnimation(0, true, false, 0.001f);

    //    std::shared_ptr<SphereColliderCom> sphere = roboobj->AddComponent<SphereColliderCom>();
    //    sphere->SetRadius(2.0f);
    //    sphere->SetMyTag(COLLIDER_TAG::Enemy);
    //    sphere->SetJudgeTag(COLLIDER_TAG::Player);

    //    roboobj->AddComponent<NodeCollsionCom>("Data/OneCoin/OneCoin.nodecollsion");
    //    RigidBodyCom* rigid = roboobj->AddComponent<RigidBodyCom>(false, RigidBodyCom::RigidType::Primitive).get();
    //    rigid->SetPrimitiveType(NodeCollsionCom::CollsionType::SPHER);
    //}

    //プレイヤー
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ -2.3,-1,-49.3 });
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
            anim->PlayAnimation(0, true);
        }
    }

    //snowparticle
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("snowparticle");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/snow.gpuparticle", 10000);
    }

    //BOSS

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

        //雑魚的生成オブジェクト
        {
            //std::shared_ptr<GameObject> spawnobject = boss->AddChildObject();
            //spawnobject->SetName("spawnenemy");
            //spawnobject->AddComponent<SpawnCom>(nullptr);
        }
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();


    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);

    PVEDirection::Instance().DirectionStart();
}

void ScenePVE::Finalize()
{
    photonNet->close();
}

void ScenePVE::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    photonNet->run(elapsedTime);

    PVEDirection::Instance().Update(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);
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