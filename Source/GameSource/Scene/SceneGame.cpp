#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/Light/Light.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "imgui.h"

#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\AnimationCom.h"
#include "Components\ColliderCom.h"
#include "Components\MovementCom.h"
#include "Components\ParticleSystemCom.h"
#include "Components\CharacterCom.h"


#include "GameSource/GameScript/FreeCameraCom.h"

#include "Components/ParticleComManager.h"

#include "Netwark/Client.h"
#include "Netwark/Server.h"


// ������
void SceneGame::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        std::shared_ptr<FreeCameraCom> f = freeCamera->AddComponent<FreeCameraCom>();
        f->SetPerspectiveFov
        (
            DirectX::XMConvertToRadians(45),
            graphics.GetScreenWidth() / graphics.GetScreenHeight(),
            0.1f, 1000.0f
        );
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
        f->SetActiveInitialize();
    }

    //���ʂ̃J����
    {
        std::shared_ptr<GameObject> camera = GameObjectManager::Instance().Create();
        camera->SetName("normalcamera");

        std::shared_ptr<CameraCom> c = camera->AddComponent<CameraCom>();
        c->SetPerspectiveFov
        (
            DirectX::XMConvertToRadians(45),
            graphics.GetScreenWidth() / graphics.GetScreenHeight(),
            0.1f, 1000.0f
        );

        camera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    //�v���C���[
    {
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(static_cast<int>(SHADERMODE::DEFERRED), static_cast<int>(BLENDSTATE::MULTIPLERENDERTARGETS));
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        a->PlayAnimation(0, true,false, 0.001f);
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<CharacterCom> c = obj->AddComponent<CharacterCom>();
    }

    //�X�e�[�W
    {
        obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, -0.4f, 0 });
        obj->transform_->SetScale({ 0.05f, 0.05f, 0.05f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(static_cast<int>(SHADERMODE::DEFERRED), static_cast<int>(BLENDSTATE::MULTIPLERENDERTARGETS));
        r->LoadModel("Data/Stage/Stage.mdl");
    }

    //�o���A
    {
        obj = GameObjectManager::Instance().Create();
        obj->SetName("barrier");
        obj->transform_->SetWorldPosition({ -2.0f,1.4f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(static_cast<int>(SHADERMODE::DEFALT), static_cast<int>(BLENDSTATE::ADD));
        r->LoadModel("Data/Ball/b.mdl");
    }

    //��������
    {
        obj = GameObjectManager::Instance().Create();
        obj->SetName("blackball");
        obj->transform_->SetWorldPosition({ 2.0f,1.4f,0.0f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(static_cast<int>(SHADERMODE::BLACK), static_cast<int>(BLENDSTATE::ALPHA));
        r->LoadModel("Data/Ball/t.mdl");
    }

    //�p�[�e�B�N��
    {
        obj = GameObjectManager::Instance().Create();
        obj->SetName("testP");
        obj->AddComponent<ParticleSystemCom>(100, false);
    }

    //���s������ǉ�
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

// �I����
void SceneGame::Finalize()
{
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    if (n)
        n->Update();

    GamePad& gamePad = Input::Instance().GetGamePad();

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //�R���|�[�l���g�Q�b�g
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
    std::shared_ptr<RendererCom> r = obj->GetComponent<RendererCom>();
    std::shared_ptr<AnimationCom> a = obj->GetComponent<AnimationCom>();
    
   

   // ConstantBufferUpdate();
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
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
    GameObjectManager::Instance().Render();

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

        static std::string ip;
        char ipAdd[256];
        ::strncpy_s(ipAdd, sizeof(ipAdd), ip.c_str(), sizeof(ipAdd));
        if (ImGui::InputText("ipv4Adress", ipAdd, sizeof(ipAdd), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ip = ipAdd;
        }
        if (ImGui::Button("Client"))
        {
            if (ip.size() > 0)
            {
                n = std::make_unique<NetClient>(ip);
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