#include "Graphics/Graphics.h"

#include "Graphics/Light/LightManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"

#include "SceneIKTest.h"
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
#include "Components\Character\TestCharacterCom.h"
#include "Components\Character\InazawaCharacterCom.h"
#include "Components\FootIKcom.h"
#include "Components/CPUParticle.h"

#include "GameSource/GameScript/FreeCameraCom.h"

// ������
void SceneIKTest::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0, 0, 0 });
        obj->transform_->SetScale({ 0.01f, 0.01f, 0.01f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/IKTestModel/IKTest.mdl");
        std::shared_ptr<AnimationCom> a = obj->AddComponent<AnimationCom>();
        std::shared_ptr<MovementCom> m = obj->AddComponent<MovementCom>();
        std::shared_ptr<TestCharacterCom> c = obj->AddComponent<TestCharacterCom>();
        std::shared_ptr<FootIKCom> f = obj->AddComponent<FootIKCom>();
    }

    //�X�e�[�W
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("stage");
        obj->transform_->SetWorldPosition({ 0, 0.0f, 0 });
        obj->transform_->SetScale({ 0.6f, 0.6f, 0.6f });
        std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS);
        r->LoadModel("Data/IKTestStage/ExampleStage.mdl");
    }

    //���s������ǉ�
    mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

//�X�V����
void SceneIKTest::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //�R���|�[�l���g�Q�b�g
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Find("player");
    std::shared_ptr<RendererCom> r = obj->GetComponent<RendererCom>();
    std::shared_ptr<AnimationCom> a = obj->GetComponent<AnimationCom>();
}

void SceneIKTest::Render(float elapsedTime)
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
}