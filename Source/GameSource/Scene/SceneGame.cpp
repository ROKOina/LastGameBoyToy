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

#include "GameSource/GameScript/FreeCameraCom.h"

#include "Components/ParticleComManager.h"


// ������
void SceneGame::Initialize()
{
    netC.Initialize();

    Graphics& graphics = Graphics::Instance();

    std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
    freeCamera->SetName("camera");

    std::shared_ptr<FreeCameraCom> f = freeCamera->AddComponent<FreeCameraCom>();
    f->SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        0.1f, 1000.0f
    );
    freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    f->SetActiveInitialize();

    std::shared_ptr<GameObject> camera = GameObjectManager::Instance().Create();
    camera->SetName("camera1");

    std::shared_ptr<CameraCom> c = camera->AddComponent<CameraCom>();
    c->SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        0.1f, 1000.0f
    );

    camera->transform_->SetWorldPosition({ 0, 5, -10 });
    
   
    //camera->AddComponent<FreeCameraCom>();

    ConstantBufferInitialize();

    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    obj->SetName("test");
    obj->transform_->SetWorldPosition({ 0, 0, 0 });
    obj->transform_->SetScale({ 0.01f, 0.01f, 0.01f });

    //const char* filename = "Data/picola/pi.mdl";
    const char* filename = "Data/OneCoin/robot.mdl";
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>();
    r->LoadModel(filename);
    
    std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
    a->PlayAnimation(0, true, 0.001f);

    //�|�X�g�G�t�F�N�g�ǉ�
    m_posteffect = std::make_unique<PostEffect>();

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
    netC.Update();

    GamePad& gamePad = Input::Instance().GetGamePad();

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //�R���|�[�l���g�Q�b�g
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("test");
    std::shared_ptr<RendererCom> r = obj->GetComponent<RendererCom>();
    std::shared_ptr<AnimationCom> a = obj->GetComponent<AnimationCom>();
    
   

    ConstantBufferUpdate();
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

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    // ���C�g�̒萔�o�b�t�@���X�V
    LightManager::Instance().UpdateConstatBuffer();

    //�f�t�@�[�h�̐ݒ�
    m_posteffect->DeferredFirstSet();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render();

    //�f�t�@�[�h�̃��\�[�X�ݒ�
    m_posteffect->DeferredResourceSet();

    //�|�X�g�G�t�F�N�g
    m_posteffect->PostEffectRender();

    //imgui�`��
    m_posteffect->PostEffectImGui();

    netC.ImGui();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().DrawGuizmo(sc->data.view, sc->data.projection);
}