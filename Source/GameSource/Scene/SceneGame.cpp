//��ԏ�ŃC���N���[�h�i�l�b�g���[�N�j
#include "Netwark/Client.h"
#include "Netwark/Server.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\RayCollisionCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\AimIKCom.h"
#include "Components\MovementCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\UenoCharacterCom.h"
#include "Components\Character\NomuraCharacterCom.h"
#include "Components\Character\HaveAllAttackCharacter.h"
#include "Components\Character\RegisterChara.h"
#include "Components/CPUParticle.h"
#include "Components\FootIKcom.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "GameSource/GameScript/FPSCameraCom.h"
#include "GameSource/GameScript/EventCameraCom.h"
#include "Components/CPUParticle.h"
#include "Components/GPUParticle.h"
#include "Graphics/Sprite/Sprite.h"
#include "Components/StageEditorCom.h"
#include "Components/SpawnCom.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components/InstanceRendererCom.h"
#include "Components\EasingMoveCom.h"

#include "Components\Character\Generate\TestCharacterGenerate.h"

#include "Netwark/Photon/StdIO_UIListener.h"

#include "GameSource/GameScript/EventCameraManager.h"
#include "Netwark/Photon/StaticSendDataManager.h"
#include <Components/Character/CharaStatusCom.h>

#include "Phsix\Physxlib.h"
#include "Components\RigidBodyCom.h"

#include "Audio/AudioSource.h"

// ������
void SceneGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region �Q�[���I�u�W�F�N�g�̐ݒ�

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

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
        obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
        obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/canyon/stage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
        obj->AddComponent<StageEditorCom>();
    }

    //�����蔻��p
    PhysXLib::Instance().Initialize();
    std::shared_ptr<GameObject> roboobj = GameObjectManager::Instance().Create();
    {
        roboobj->SetName("robo");
        roboobj->transform_->SetWorldPosition({ 0, 10, 0 });
        roboobj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = roboobj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = roboobj->AddComponent<AnimationCom>();
        a->PlayAnimation(0, true, false, 0.001f);

        //std::shared_ptr<SphereColliderCom> sphere = roboobj->AddComponent<SphereColliderCom>();
        //sphere->SetRadius(2.0f);
        //sphere->SetMyTag(COLLIDER_TAG::Enemy);
        //sphere->SetJudgeTag(COLLIDER_TAG::Player);

        roboobj->AddComponent<NodeCollsionCom>("Data/OneCoin/OneCoin.nodecollsion");
        roboobj->AddComponent<RigidBodyCom>(false, NodeCollsionCom::CollsionType::SPHER);
    }

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::HAVE_ALL_ATTACK, obj);
    }

    //�J�������v���C���[�̎q�ǂ��ɂ��Đ��䂷��
    {
        std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        cameraPost->SetName("cameraPostPlayer");
        std::shared_ptr<FPSCameraCom>fpscamera = cameraPost->AddComponent<FPSCameraCom>();

        //pico�ʒu
        cameraPost->transform_->SetWorldPosition({ 0, 80.821f, 33.050f });
        playerObj->GetComponent<CharacterCom>()->SetCameraObj(cameraPost.get());
    }

    //BOSS
#if(1)
    {
        auto& boss = GameObjectManager::Instance().Create();
        boss->SetName("BOSS");
        std::shared_ptr<RendererCom> r = boss->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Jammo/jammo.mdl");
        boss->transform_->SetWorldPosition({ 0.0f,0.0f,14.0f });
        boss->transform_->SetScale({ 0.06f, 0.06f, 0.06f });
        t = boss->transform_;
        boss->AddComponent<MovementCom>();
        boss->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
        std::shared_ptr<SphereColliderCom> collider = boss->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        boss->AddComponent<AnimationCom>();
        boss->AddComponent<BossCom>();
        boss->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
        boss->AddComponent<CharaStatusCom>();

        //�{���v�N
        {
            std::shared_ptr<GameObject> bompspawn = boss->AddChildObject();
            bompspawn->SetName("bomp");
            bompspawn->AddComponent<SpawnCom>();
        }

        //����R���W����
        {
            std::shared_ptr<GameObject> lefthand = boss->AddChildObject();
            lefthand->SetName("lefthandcollsion");
            std::shared_ptr<SphereColliderCom> lefthandcollider = lefthand->AddComponent<SphereColliderCom>();
            lefthandcollider->SetEnabled(false);
            lefthandcollider->SetMyTag(COLLIDER_TAG::Enemy);
            lefthandcollider->SetJudgeTag(COLLIDER_TAG::Player);
            lefthandcollider->SetRadius(1.0f);
        }

        //�E���R���W����
        {
            std::shared_ptr<GameObject> rightlegs = boss->AddChildObject();
            rightlegs->SetName("rightlegscollsion");
            std::shared_ptr<SphereColliderCom> rightlegscollider = rightlegs->AddComponent<SphereColliderCom>();
            rightlegscollider->SetEnabled(false);
            rightlegscollider->SetMyTag(COLLIDER_TAG::Enemy);
            rightlegscollider->SetJudgeTag(COLLIDER_TAG::Player);
            rightlegscollider->SetRadius(1.0f);
        }

        //��ɕt����΂̃G�t�F�N�g
        {
            std::shared_ptr<GameObject>cpufireeffect = boss->AddChildObject();
            cpufireeffect->SetName("cpufireeffect");
            std::shared_ptr<CPUParticle>cpufire = cpufireeffect->AddComponent<CPUParticle>("Data/Effect/fire.cpuparticle", 1000);
            cpufire->SetActive(false);
        }

        ////gpu�̉�
        //{
        //    std::shared_ptr<GameObject>gpufireeffect = boss->AddChildObject();
        //    gpufireeffect->SetName("gpufireeffect");
        //    std::shared_ptr<GPUParticle>gpufire = gpufireeffect->AddComponent<GPUParticle>("Data/Effect/fire.gpuparticle", 10000);
        //    gpufire->SetLoop(false);
        //}

        //���n���̉��G�t�F�N�g
        {
            std::shared_ptr<GameObject>landsmokeeffect = boss->AddChildObject();
            landsmokeeffect->SetName("cpulandsmokeeffect");
            landsmokeeffect->transform_->SetWorldPosition({ 0,1.7f,0 });
            std::shared_ptr<CPUParticle>landsmoke = landsmokeeffect->AddComponent<CPUParticle>("Data/Effect/landsmoke.cpuparticle", 1000);
            landsmoke->SetActive(false);
        }

        //�����̃G�t�F�N�g
        {
            std::shared_ptr<GameObject>cycloneffect = boss->AddChildObject();
            cycloneffect->SetName("cycloncpueffect");
            std::shared_ptr<CPUParticle>cpuparticle = cycloneffect->AddComponent<CPUParticle>("Data/Effect/cyclon.cpuparticle", 1000);
            cpuparticle->SetActive(false);
        }

        ////gpu�̗����̃G�t�F�N�g
        //{
        //    std::shared_ptr<GameObject>gpucycloneffect = boss->AddChildObject();
        //    gpucycloneffect->SetName("cyclongpueffect");
        //    std::shared_ptr<GPUParticle>gpufire = gpucycloneffect->AddComponent<GPUParticle>("Data/Effect/cyclon.gpuparticle", 10000);
        //    gpufire->SetLoop(false);
        //}

        //�΋�
        {
            std::shared_ptr<GameObject>cpufireeffect = boss->AddChildObject();
            cpufireeffect->SetName("fireball");
            std::shared_ptr<CPUParticle>cpufire = cpufireeffect->AddComponent<CPUParticle>("Data/Effect/fireball.cpuparticle", 1000);
            cpufire->SetActive(false);
            cpufireeffect->AddComponent<EasingMoveCom>(nullptr);
            std::shared_ptr<SphereColliderCom> fireballcollider = cpufireeffect->AddComponent<SphereColliderCom>();
            fireballcollider->SetEnabled(false);
            fireballcollider->SetMyTag(COLLIDER_TAG::Enemy);
            fireballcollider->SetJudgeTag(COLLIDER_TAG::Player);
            fireballcollider->SetRadius(1.0f);
        }

        ////gpu�̗����̃G�t�F�N�g
        //{
        //    std::shared_ptr<GameObject>gpucycloneffect = boss->AddChildObject();
        //    gpucycloneffect->SetName("cyclongpueffect2");
        //    std::shared_ptr<GPUParticle>gpufire = gpucycloneffect->AddComponent<GPUParticle>("Data/Effect/cyclon.gpuparticle", 10000);
        //    gpufire->SetLoop(true);
        //}

        ////gpu�̉�
        //{
        //    std::shared_ptr<GameObject>gpufireeffect = boss->AddChildObject();
        //    gpufireeffect->SetName("gpufireeffect4");
        //    std::shared_ptr<GPUParticle>gpufire = gpufireeffect->AddComponent<GPUParticle>("Data/Effect/fire.gpuparticle", 10000);
        //    gpufire->SetLoop(true);
        //}

        ////gpu�̗����̃G�t�F�N�g
        //{
        //    std::shared_ptr<GameObject>gpucycloneffect = boss->AddChildObject();
        //    gpucycloneffect->SetName("cyclongpueffect3");
        //    std::shared_ptr<GPUParticle>gpufire = gpucycloneffect->AddComponent<GPUParticle>("Data/Effect/cyclon.gpuparticle", 10000);
        //    gpufire->SetLoop(true);
        //}
    }
#endif

    //UI�Q�[���I�u�W�F�N�g����
    CreateUiObject();

#pragma endregion

#pragma region �O���t�B�b�N�n�̐ݒ�
    //���s������ǉ�
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);

    // �X�J�C�{�b�N�X�̐ݒ�
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\snowy_hillside_4k.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

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
    if (n)
    {
        n->Update();

        if (!n->IsNextFrame())
        {
            return;
        }
    }

    photonNet->run(elapsedTime);

    //�C�x���g�J�����p
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //�{�X�̈ʒu�擾
    //sc->data.bossposiotn = t->GetLocalPosition();

    // �Q�[���I�u�W�F�N�g�̍X�V
    PhysXLib::Instance().Update(elapsedTime);
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

    // ���C�g�̒萔�o�b�t�@���X�V
    LightManager::Instance().UpdateConstatBuffer();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());

    if (n)
        n->ImGui();
    else
    {
        //�l�b�g���[�N���艼�{�^��
        ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("NetSelect", nullptr, ImGuiWindowFlags_None);

        static int ClientID = 0;
        static std::string ip;
        char ipAdd[256];

        ImGui::InputInt("id", &ClientID);
        ::strncpy_s(ipAdd, sizeof(ipAdd), ip.c_str(), sizeof(ipAdd));
        if (ImGui::InputText("ipv4Adress", ipAdd, sizeof(ipAdd), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ip = ipAdd;
        }
        if (ImGui::Button("Client"))
        {
            if (ip.size() > 0)
            {
                n = std::make_unique<NetClient>(ip, ClientID);
                n->Initialize();
            }
        }

        if (ImGui::Button("Server"))
        {
            n = std::make_unique<NetServer>();
            n->Initialize();
        }

        ImGui::End();
    }

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
}

void SceneGame::SetUserInputs()
{
    // �v���C���[�̓��͏��
    SetPlayerInput();

    // ���̃v���C���[�̓��͏��
    SetOnlineInput();
}

void SceneGame::SetPlayerInput()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
    if (obj.use_count() == 0)return;

    std::shared_ptr<CharacterCom> chara = obj->GetComponent<CharacterCom>();
    if (chara.use_count() == 0) return;

    // ���͏����v���C���[�L�����N�^�[�ɑ��M
    chara->SetUserInput(gamePad.GetButton());
    chara->SetUserInputDown(gamePad.GetButtonDown());
    chara->SetUserInputUp(gamePad.GetButtonUp());

    chara->SetLeftStick(gamePad.GetAxisL());
    chara->SetRightStick(gamePad.GetAxisR());
}

void SceneGame::SetOnlineInput()
{
    if (!n)return;

    for (auto& client : n->GetNetDatas())
    {
        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        if (clientObj)
        {
            std::shared_ptr<CharacterCom> chara = clientObj->GetComponent<CharacterCom>();

            if (!chara)continue;
        }
    }
}

void SceneGame::DelayOnlineInput()
{
    if (!n)return;
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
            reticle->AddComponent<Sprite>("Data/UIData/Reticle.ui", false);
        }
        //HpFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
            hpFrame->SetName("HpFrame");
            hpFrame->AddComponent<Sprite>("Data/UIData/HpFrame.ui", false);
        }
        //HpGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpGauge = canvas->AddChildObject();
            hpGauge->SetName("HpGauge");
            hpGauge->AddComponent<Sprite>("Data/UIData/HpGauge.ui", false);
        }
        //HpMemori
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HpMemori");
            hpMemori->AddComponent<Sprite>("Data/UIData/HpMemori.ui", false);
        }

        //BoostFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostFrame");
            hpMemori->AddComponent<Sprite>("Data/UIData/BoostFrame_01.ui", false);
        }

        //BoostFrame2
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostFrame2");
            hpMemori->AddComponent<Sprite>("Data/UIData/BoostFrame_02.ui", false);
        }

        //BoostGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("BoostGauge");
            hpMemori->AddComponent<Sprite>("Data/UIData/BoostGauge.ui", false);
        }

        //UltFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltFrame");
            hpMemori->AddComponent<Sprite>("Data/UIData/UltFrame.ui", false);
        }

        //HideUltGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("HideUltGauge");
            hpMemori->AddComponent<Sprite>("Data/UIData/UltGauge.ui", false);
        }

        //UltGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("UltGauge");
            hpMemori->AddComponent<Sprite>("Data/UIData/UltGauge.ui", false);
        }

        //SkillFrame
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("SkillFrame");
            hpMemori->AddComponent<Sprite>("Data/UIData/SkillFrame_01.ui", false);
        }

        //SkillGauge
        {
            std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
            std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
            hpMemori->SetName("SkillGauge");
            hpMemori->AddComponent<Sprite>("Data/UIData/SkillFrame_02.ui", false);
        }
    }
}