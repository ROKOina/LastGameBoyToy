//��ԏ�ŃC���N���[�h�i�l�b�g���[�N�j
#include "Netwark/Client.h"
#include "Netwark/Server.h"

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
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components/CPUParticle.h"

#include "GameSource/GameScript/FreeCameraCom.h"

#include "Components/ParticleComManager.h"


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

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.002f, 0.002f, 0.002f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/OneCoin/robot.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();

        auto& oo=obj->AddChildObject();
        oo->SetName("barrier1");
        oo->transform_->SetScale({ 500,500,500 });
        oo->transform_->SetLocalPosition({ 1000,0,0 });
        std::shared_ptr<RendererCom> ro = oo->AddComponent<RendererCom>(SHADERMODE::DEFALT, BLENDSTATE::ADD);
        ro->LoadModel("Data/Ball/b.mdl");

        auto& oo1=oo->AddChildObject();
        oo1->SetName("barrier2");
        oo1->transform_->SetScale({ 1,1,1 });
        oo1->transform_->SetLocalPosition({ 3,0,0 });
        std::shared_ptr<RendererCom> ro1 = oo1->AddComponent<RendererCom>(SHADERMODE::DEFALT, BLENDSTATE::ADD);
        ro1->LoadModel("Data/Ball/b.mdl");

    }
    //IK�e�X�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("IKTest");
        obj->transform_->SetWorldPosition({ -5, 0, 0 });
        obj->transform_->SetScale({ 0.01f, 0.01f, 0.01f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/IKTestModel/IKTest.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    }

    //test
    /*{
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("zombie");
        obj->transform_->SetWorldPosition({ 0, -0.4f, 0 });
        obj->transform_->SetScale({ 0.05f, 0.05f, 0.05f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/zombie/Zombie.mdl");
       
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        a->PlayAnimation(1, true, false, 0.05);
        a->PlayUpperBodyOnlyAnimation(0, true, 0.05f);
        a->SetUpAnimationUpdate(1);
    }*/


  //�X�e�[�W
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("stage");
    obj->transform_->SetWorldPosition({ 0, -0.4f, 0 });
    obj->transform_->SetScale({ 0.05f, 0.05f, 0.05f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/Stage/Stage.mdl");
  }

  //�e�X�g
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("plane");
    obj->transform_->SetWorldPosition({ 0, 0.1f, 3 });
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::AREA_EFFECT_CIRCLE, BLENDSTATE::ALPHA);
    r->LoadModel("Data/UtilityModels/plane.mdl");
    r->LoadMaterial("Data/UtilityModels/Kaho.Material");
    auto& cb = r->SetVariousConstant<EffectConstants>();
    cb->simulateSpeed1 = 1.6f;
    cb->simulateSpeed2 = -2.4f;
    cb->waveEffectRange = 0.97f;
    cb->waveEffectColor = { 1.0f,0.3f,0.0f,0.1f };
    cb->waveEffectIntensity = 5.0f;
  }

  //�e�X�g
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("plane");
    obj->transform_->SetWorldPosition({ 0, 0.1f, 0 });
    obj->transform_->SetScale({ 0.01f,0.01f,0.01f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::AREA_EFFECT_CIRCLE, BLENDSTATE::ALPHA);
    r->LoadModel("Data/UtilityModels/plane.mdl");
    r->LoadMaterial("Data/UtilityModels/SkillEffect_Circle.Material");
    auto& cb = r->SetVariousConstant<EffectConstants>();
    cb->simulateSpeed1 = 1.6f;
    cb->simulateSpeed2 = -2.4f;
    cb->waveEffectRange = 0.97f;
    cb->waveEffectColor = { 1.0f,0.3f,0.0f,0.1f };
    cb->waveEffectIntensity = 5.0f;
  }

  //�o���A
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("barrier");
    obj->transform_->SetWorldPosition({ -2.0f,1.4f,0.0f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::DEFALT, BLENDSTATE::ADD);
    r->LoadModel("Data/Ball/b.mdl");
  }

  //��������
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("blackball");
    obj->transform_->SetWorldPosition({ 2.0f,1.4f,0.0f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADERMODE::BLACK, BLENDSTATE::ALPHA);
    r->LoadModel("Data/Ball/t.mdl");
  }

  //�p�[�e�B�N��
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("testP");
    obj->AddComponent<ParticleSystemCom>(100, false);
  }

  //cpu�p�[�e�B�N���ݒ�
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("testparticle");
    obj->AddComponent<CPUParticle>(100);
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