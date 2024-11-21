#include "Graphics/Graphics.h"
#include "Scene/SceneManager.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "SceneLGBT.h"
#include "Component\Light\LightCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include <Component\Camera\FreeCameraCom.h>

//������
void SceneLGBT::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //LGBT
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("LGBT");
        obj->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/lgbt.ui", Sprite::SpriteShader::DISSOLVE, false);
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();
}

//�X�V
void SceneLGBT::Update(float elapsedTime)
{
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);

    //�V�[���J�ڂ�A�C�R���̏���
    SceneTransition(elapsedTime);
}

//�`��
void SceneLGBT::Render(float elapsedTime)
{
    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate(elapsedTime);

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, { 0,0,0 });
}

//�V�[���J��
void SceneLGBT::SceneTransition(float elapsedTime)
{
    const auto& lgbt = GameObjectManager::Instance().Find("LGBT")->GetComponent<Sprite>();
    easingtime += elapsedTime;
    if (easingtime > 2.0f && easingtime < 2.1f)
    {
        lgbt->EasingPlay();
    }

    if (easingtime > 7.5f)
    {
        dissolvetime += elapsedTime / 3;
        lgbt->SetClipTime(dissolvetime);

        if (lgbt->GetClipTime() > 1.0f)
        {
            SceneManager::Instance().ChangeScene(new SceneTitle);
        }
    }
}