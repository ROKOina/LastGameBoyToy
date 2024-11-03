//��ԏ�ŃC���N���[�h�i�l�b�g���[�N�j
#include "Netwark/Photon/StdIO_UIListener.h"
#include "Netwark/Photon/StaticSendDataManager.h"

#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"

#include "imgui.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneSugimoto.h"
#include "Scene\SceneResult\SceneResult.h"
#include "../SceneManager.h"
#include "../SceneLoading/SceneLoading.h"

#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include "Component/Collsion/RayCollisionCom.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Camera/FPSCameraCom.h"
#include "Component/Camera/EventCameraCom.h"
#include "Component/Camera/EventCameraManager.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/Renderer/InstanceRendererCom.h"

#include "Phsix\Physxlib.h"

#include "Audio/AudioSource.h"


SceneSugimoto::~SceneSugimoto()
{
  GameObjectManager::Instance().AllRemove();
  GameObjectManager::Instance().RemoveGameObjects();
}

// ������
void SceneSugimoto::Initialize()
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

  // �R�_�b�N
  {

    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("codack");
    obj->transform_->SetWorldPosition({ 0.0f,-5.25f,-6.0f });
    obj->transform_->SetScale({ 0.01f,0.000001f,0.01f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::FAKE_DEPTH, BLENDSTATE::MULTIPLERENDERTARGETS);
    r->LoadModel("Data/cube/cube.mdl");
    r->LoadMaterial("Data/cube/asobi_cube.material");
    r->SetRenderShadow(false);
    r->SetRenderSilhoutte(false);
  }

  // Fake Interior
  {
    auto& instanceOwner = GameObjectManager::Instance().Create();
    instanceOwner->SetName("Cube Instance Owner");
    instanceOwner->transform_->SetWorldPosition({ -8.0f,0.2f,-6.0f });
    instanceOwner->transform_->SetScale({ 0.02f,0.02f,0.02f });
    std::shared_ptr<InstanceRenderer> ir = instanceOwner->AddComponent<InstanceRenderer>(SHADER_ID_MODEL::FAKE_INTERIOR, 10, BLENDSTATE::MULTIPLERENDERTARGETS);
    ir->LoadModel("Data/cube/cube.mdl");
    ir->LoadMaterial("Data/cube/interior.material");

    auto& cb = ir->SetVariousConstant<FakeInteriorConstants>();
    cb->reflectionAmount = 0.0f;
    cb->offset = 1.08f;

    //���̍쐬
    float startZ = -140;
    float offsetZ = 140;
    for (int i = 0; i < 6; ++i)
    {
      auto& obj = ir->CreateInstance(true);
      obj->SetName(("window " + std::to_string(i)).c_str());
      obj->transform_->SetLocalPosition({ 0.0f,(i / 3) * 140.0f, startZ + (i % 3) * offsetZ });
    }

    // �r���{��
    {
      auto& obj = GameObjectManager::Instance().Create();
      obj->SetName("House");
      instanceOwner->AddChildObject(obj);
      obj->transform_->SetLocalPosition({ 0.0f,67.0f,0.0f });
      obj->transform_->SetScale({ 0.999f,3.0f,4.5f });

      std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
      r->LoadModel("Data/cube/cube.mdl");
      r->LoadMaterial("Data/cube/House.material");
      r->SetRenderSilhoutte(false);
    }
  }


  //�X�e�[�W
  {
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("stage");
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel("Data/canyon/stage.mdl");
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

  //�R���X�^���g�o�b�t�@�̏�����
  ConstantBufferInitialize();

#pragma endregion
}

// �I����
void SceneSugimoto::Finalize()
{
}

// �X�V����
void SceneSugimoto::Update(float elapsedTime)
{
  //�C�x���g�J�����p
  EventCameraManager::Instance().EventUpdate(elapsedTime);

  // �Q�[���I�u�W�F�N�g�̍X�V
  GameObjectManager::Instance().UpdateTransform();
  GameObjectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneSugimoto::Render(float elapsedTime)
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

}

void SceneSugimoto::SetUserInputs()
{
  // �v���C���[�̓��͏��
  SetPlayerInput();

  // ���̃v���C���[�̓��͏��
  SetOnlineInput();
}

void SceneSugimoto::SetPlayerInput()
{
}

void SceneSugimoto::SetOnlineInput()
{
}

void SceneSugimoto::DelayOnlineInput()
{
}

void SceneSugimoto::CreateUiObject()
{
}

void SceneSugimoto::TransitionPVEFromResult()
{
}