#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Animation\FootIKcom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "ScenePVP.h"
#include "Component\Light\LightCom.h"

void ScenePVP::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //���C�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
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
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<InazawaCharacterCom> c = obj->AddComponent<InazawaCharacterCom>();
        std::shared_ptr<FootIKCom> f = obj->AddComponent<FootIKCom>();
    }

    //�X�e�[�W
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0.0f, 0 });
        obj->transform_->SetScale({ 0.6f, 0.6f, 0.6f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/IKTestStage/ExampleStage.mdl");
        obj->AddComponent<RayCollisionCom>("Data/IKTestStage/ExampleStage.collision");
    }
}

void ScenePVP::Finalize()
{
}

void ScenePVP::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void ScenePVP::Render(float elapsedTime)
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

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());
}