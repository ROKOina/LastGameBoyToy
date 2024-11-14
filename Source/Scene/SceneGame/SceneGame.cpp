//��ԏ�ŃC���N���[�h�i�l�b�g���[�N�j
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

#include "Setting/Setting.h"

SceneGame::~SceneGame()
{
    GameObjectManager::Instance().AllRemove();
    GameObjectManager::Instance().RemoveGameObjects();
}

// ������
void SceneGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region �Q�[���I�u�W�F�N�g�̐ݒ�

    //�|�X�g�G�t�F�N�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //���C�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();
    //�C�x���g�p�J����
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        eventCamera->AddComponent<EventCameraCom>();
        eventCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //�X�e�[�W
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

    //�����蔻��p
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

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,-1,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);

        //�E���g�֌WObj�ǉ�
        {
            //�A�^�b�N�n�E���g
            std::shared_ptr<GameObject> ultAttckChild = obj->AddChildObject();
            ultAttckChild->SetName("UltAttackChild");
            //�ʒu���J�����ƈꏏ�ɂ���
            ultAttckChild->transform_->SetWorldPosition({ 0, 8.0821f, 3.3050f });

            std::shared_ptr<RayColliderCom> rayCol = ultAttckChild->AddComponent<RayColliderCom>();
            rayCol->SetMyTag(COLLIDER_TAG::Player);
            rayCol->SetJudgeTag(COLLIDER_TAG::Enemy);
            rayCol->SetEnabled(false);

            //�_���[�W�����p
            std::shared_ptr<HitProcessCom> hitDamage = ultAttckChild->AddComponent<HitProcessCom>(obj);
            hitDamage->SetHitType(HitProcessCom::HIT_TYPE::DAMAGE);
        }
        //�A�^�b�N�E���g�̃G�t�F�N�g
        {
            std::shared_ptr<GameObject> attackUltEff = obj->AddChildObject();
            attackUltEff->SetName("attackUltEFF");
            attackUltEff->transform_->SetRotation(obj->transform_->GetRotation());
            attackUltEff->transform_->SetWorldPosition(obj->transform_->GetWorldPosition());
            std::shared_ptr<GPUParticle> eff = attackUltEff->AddComponent<GPUParticle>(nullptr, 500);
        }
    }

    //�J�������v���C���[�̎q�ǂ��ɂ��Đ��䂷��
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico�ʒu
        cameraPost->transform_->SetWorldPosition({ 0, 8.0821f, 3.3050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());

        //�r
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
        boss->AddComponent<CharaStatusCom>();
        boss->AddComponent<BossCom>();
        boss->AddComponent<AimIKCom>(nullptr, "Boss_spine_up");
        std::shared_ptr<PushBackCom>pushBack = boss->AddComponent<PushBackCom>();
        pushBack->SetRadius(1.5f);
        pushBack->SetWeight(600.0f);

        //�E���̉��G�t�F�N�g
        {
            std::shared_ptr<GameObject>rightfootsmokeobject = boss->AddChildObject();
            rightfootsmokeobject->SetName("rightfootsmokeeffect");
            std::shared_ptr<CPUParticle>rightfootsmokeeffect = rightfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 600);
            rightfootsmokeeffect->SetActive(false);
        }

        //�����̉��G�t�F�N�g
        {
            std::shared_ptr<GameObject>leftfootsmokeobject = boss->AddChildObject();
            leftfootsmokeobject->SetName("leftfootsmokeeffect");
            std::shared_ptr<CPUParticle>leftfootsmokeeffect = leftfootsmokeobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/enemyfootsmoke.cpuparticle", 600);
            leftfootsmokeeffect->SetActive(false);
        }

        //����̃Q�[���I�u�W�F�N�g�B
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

        //�E��̃Q�[���I�u�W�F�N�g�B
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

        //�����I�u�W�F�N�g
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

        //�`���[�W�U��
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

        //�n�ʂ�@���t����U��
        {
            std::shared_ptr<GameObject> groundobject = boss->AddChildObject();
            groundobject->SetName("groundsmoke");
            std::shared_ptr<CPUParticle>smoke = groundobject->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/groundsmoke.cpuparticle", 1000);
            smoke->SetActive(false);
        }

        //�G���I�����I�u�W�F�N�g
        {
            //std::shared_ptr<GameObject> spawnobject = boss->AddChildObject();
            //spawnobject->SetName("spawnenemy");
            //spawnobject->AddComponent<SpawnCom>(nullptr);
        }
    }

#endif

    //UI�Q�[���I�u�W�F�N�g����
    CreateUiObject();

#pragma endregion

#pragma region �O���t�B�b�N�n�̐ݒ�

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    // �X�J�C�{�b�N�X�̐ݒ�
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\CosmicCoolCloudBottom.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

#pragma endregion

#pragma region �I�[�f�B�I�n�̐ݒ�
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

// �I����
void SceneGame::Finalize()
{
    photonNet->close();
}

// �X�V����
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

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //�{�X�̈ʒu�擾
    //sc->data.bossposiotn = t->GetLocalPosition();

    //�J�ڊ֌W
    TransitionPVEFromResult();

    // �Q�[���I�u�W�F�N�g�̍X�V
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate(elapsedTime);

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    photonNet->ImGui();

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventCameraImGui();

    ImGui::Begin("Effect");

    EffectNew();
    ImGui::End();
}

//�G�t�F�N�g����
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
}

void SceneGame::CreateUiObject()
{
    //UI
    {
        //�L�����o�X
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        //���e�B�N��
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
            std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, false, UiSystem::X_ONLY_ADD);
            gauge->SetMaxValue(200);
            float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
            gauge->SetVariableValue(i);
        }
        //HpMemori
        {
            std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
            std::shared_ptr<GameObject> hpMemori = hpFrame->AddChildObject();
            hpMemori->SetName("HpMemori");
            hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpMemori.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> boostFrame = canvas->AddChildObject();
            boostFrame->SetName("BoostFrame");
            boostFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostFrame2
        {
            std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Find("BoostFrame");
            std::shared_ptr<GameObject> BoostFrame2 = BoostFrame->AddChildObject();
            BoostFrame2->SetName("BoostFrame2");
            BoostFrame2->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame_02.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //BoostGauge
        {
            std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Find("BoostFrame");
            std::shared_ptr<GameObject> BoostGauge = BoostFrame->AddChildObject();
            BoostGauge->SetName("BoostGauge");
            BoostGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGauge.ui", Sprite::SpriteShader::DEFALT, false);
        }

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
            skillFrame->SetName("SkillFrame");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillFrame2
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skill_Q = SkillFrame->AddChildObject();
            skill_Q->SetName("Skill_Frame2");
            skill_Q->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame2_01.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillMask
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGaugeHide");
            skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrameHide.ui", Sprite::SpriteShader::DEFALT, false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillFrame = SkillFrame->AddChildObject();
            skillFrame->SetName("SkillGauge");
            std::shared_ptr<UI_Skill>skillGauge = skillFrame->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge.ui", Sprite::SpriteShader::DEFALT, false, 1030, 937);
            std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
            skillGauge->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
            float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
            skillGauge->SetVariableValue(i);
        }

        //Skill_Q
        {
            std::shared_ptr<GameObject> SkillFrame = GameObjectManager::Instance().Find("SkillFrame");
            std::shared_ptr<GameObject> skillGauge = SkillFrame->AddChildObject();
            skillGauge->SetName("Skill_Q");
            skillGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_Q.ui", Sprite::SpriteShader::DEFALT, false);
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