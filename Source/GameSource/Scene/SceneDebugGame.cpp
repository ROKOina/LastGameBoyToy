//��ԏ�ŃC���N���[�h�i�l�b�g���[�N�j
#include "Netwark/Client.h"
#include "Netwark/Server.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneDebugGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\RayCollisionCom.h"
#include "Components\TransformCom.h"
//#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\Character\UenoCharacterCom.h"
#include "Components\Character\NomuraCharacterCom.h"
#include "Components/CPUParticle.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "Components/CPUParticle.h"
#include "Components/GPUParticle.h"

std::shared_ptr<CameraCom> freeC;
std::shared_ptr<CameraCom> follow;

// ������
void SceneDebugGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

#pragma region �Q�[���I�u�W�F�N�g�̐ݒ�

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        std::shared_ptr<FreeCameraCom> f = freeCamera->AddComponent<FreeCameraCom>();
        freeC = f;
        f->SetPerspectiveFov
        (
            DirectX::XMConvertToRadians(45),
            graphics.GetScreenWidth() / graphics.GetScreenHeight(),
            0.1f, 1000.0f
        );
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
        f->SetActiveInitialize();
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        a->PlayAnimation(0, true, false, 0.001f);
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        //std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
        std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();
        //std::shared_ptr<UenoCharacterCom> c = obj->AddComponent<UenoCharacterCom>();
        //std::shared_ptr<NomuraCharacterCom> c = obj->AddComponent<NomuraCharacterCom>();
    }

    //���ʂ̃J����(�v���C���[�̎q�ɂ���)
    {
        ////�J�����𓮂����x��
        //std::shared_ptr<GameObject> playerObj = GameObjectManager::Instance().Find("player");
        //std::shared_ptr<GameObject> cameraPost = playerObj->AddChildObject();
        ////std::shared_ptr<GameObject> cameraPost = GameObjectManager::Instance().Create();
        //cameraPost->SetName("cameraPostPlayer");

        ////�J�����{��
        ////std::shared_ptr<GameObject> camera = GameObjectManager::Instance().Create();
        //std::shared_ptr<GameObject> camera = cameraPost->AddChildObject();
        //camera->SetName("normalcamera");

        //std::shared_ptr<CameraCom> c = camera->AddComponent<CameraCom>();
        //follow = c;
        //c->SetPerspectiveFov
        //(
        //    DirectX::XMConvertToRadians(45),
        //    graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        //    0.1f, 1000.0f
        //);

        //camera->transform_->SetWorldPosition({ 0, 950, 300 });

        //playerObj->GetComponent<CharacterCom>()->SetCameraObj(camera.get());
    }

    //�X�e�[�W
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, -0.4f, 0 });
        obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/Stage/Stage.mdl");
        //obj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");
    }

    //�o���A
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("barrier");
        obj->transform_->SetWorldPosition({ -2.0f,1.4f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFAULT, BLENDSTATE::ADD);
        r->LoadModel("Data/Ball/b.mdl");
    }

    //��������
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("blackball");
        obj->transform_->SetWorldPosition({ 2.0f,1.4f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::BLACK, BLENDSTATE::ALPHA);
        r->LoadModel("Data/Ball/t.mdl");
    }

    //�e�X�g
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("SciFiGate");
        obj->transform_->SetWorldPosition({ 0, 1.8f, 5 });
        obj->transform_->SetScale({ 0.06f,0.0001f,0.02f });
        obj->transform_->SetEulerRotation({ 90,0,0 });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::SCI_FI_GATE, BLENDSTATE::ADD, RASTERIZERSTATE::SOLID_CULL_NONE);
        r->LoadModel("Data/UtilityModels/plane.mdl");
        r->LoadMaterial("Data/UtilityModels/SciFiGate.Material");

        auto& cb = r->SetVariousConstant<SciFiGateConstants>();
        cb->simulateSpeed1 = 1.1f;
        cb->simulateSpeed2 = -0.3f;
        cb->uvScrollDir1 = { 1.0f,1.0f };
        cb->uvScrollDir2 = { 0.0f,1.0f };
        cb->uvScale1 = { 2.0f,1.0f };
        cb->intensity1 = 0.8f;
        cb->intensity2 = 1.6f;
        cb->effectColor1 = { 1.0f,0.4f,0.0f,1.0f };
        cb->effectColor2 = { 1.0f,0.2f,0.0f,1.0f };
        cb->contourIntensity = 1.5f;
    }

    //IK�e�X�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("IKTest");
        obj->transform_->SetWorldPosition({ -5, 0, 0 });
        obj->transform_->SetScale({ 0.01f, 0.01f, 0.01f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/IKTestModel/IKTest.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<CapsuleColliderCom> c = obj->AddComponent<CapsuleColliderCom>();
        c->SetPosition1({ 0,10,0 });
        c->SetMyTag(COLLIDER_TAG::Enemy);
        c->SetJudgeTag(COLLIDER_TAG::PlayerAttack);
    }

    ////cpuparticletest
    //{
    //  auto& obj = GameObjectManager::Instance().Create();
    //  obj->SetName("cpuparticle");
    //  obj->AddComponent<CPUParticle>("Data\\Effect\\test.cpuparticle", 1000);
    //}

    //gpuparticletest
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("gpuparticle");
        obj->AddComponent<GPUParticle>("Data\\Effect\\test.gpuparticle", 4000);
    }

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

#pragma endregion
}

// �I����
void SceneDebugGame::Finalize()
{
}

// �X�V����
void SceneDebugGame::Update(float elapsedTime)
{
    if (n)
        n->Update();

    // �L�[�̓��͏����e�L�����N�^�[�Ɋ��蓖�Ă�
    SetUserInputs();

    // �Q�[���I�u�W�F�N�g�̍X�V
    GameObjectManager::Instance().Update(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();

    ////�R���|�[�l���g�Q�b�g
    //std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("zombie");
    //std::shared_ptr<RendererCom> r = obj->GetComponent<RendererCom>();
    //std::shared_ptr<AnimationCom> a = obj->GetComponent<AnimationCom>();
    //DirectX::XMFLOAT3 pos = obj->transform_->GetWorldPosition();
    //a->updateRootMotion(pos);
}

// �`�揈��
void SceneDebugGame::Render(float elapsedTime)
{
    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate(elapsedTime);

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    // ���C�g�̒萔�o�b�t�@���X�V
    LightManager::Instance().UpdateConstatBuffer();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, mainDirectionalLight->GetDirection());

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().DrawGuizmo(sc->data.view, sc->data.projection);

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
}

void SceneDebugGame::SetUserInputs()
{
    // �v���C���[�̓��͏��
    SetPlayerInput();

    // ���̃v���C���[�̓��͏��
    SetOnlineInput();
}

void SceneDebugGame::SetPlayerInput()
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

void SceneDebugGame::SetOnlineInput()
{
    if (!n)return;

    for (auto& client : n->GetNetDatas())
    {
        ////�������g�̏ꍇ�͓��͏����X�V
        //if (client.id == n->GetNetId())
        //{
        //    GamePad& gamePad = Input::Instance().GetGamePad();

        //    client.input |= gamePad.GetButton();
        //    client.inputDown |= gamePad.GetButtonDown();
        //    client.inputUp |= gamePad.GetButtonUp();
        //}

        std::string name = "Net" + std::to_string(client.id);
        std::shared_ptr<GameObject> clientObj = GameObjectManager::Instance().Find(name.c_str());

        if (clientObj)
        {
            std::shared_ptr<CharacterCom> chara = clientObj->GetComponent<CharacterCom>();

            if (!chara)continue;

            // ���͏����v���C���[�L�����N�^�[�ɑ��M
            chara->SetUserInput(client.input);
            chara->SetUserInputDown(client.inputDown);
            chara->SetUserInputUp(client.inputUp);

            //chara->SetLeftStick(gamePad.GetAxisL());
            //chara->SetRightStick(gamePad.GetAxisR());
        }
    }
}