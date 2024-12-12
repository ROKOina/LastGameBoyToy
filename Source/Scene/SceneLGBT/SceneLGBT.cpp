#include "Graphics/Graphics.h"
#include "Scene/SceneManager.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component/System/TransformCom.h"
#include "SceneLGBT.h"
#include "Component\Light\LightCom.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component/Character/RegisterChara.h"
#include "Component/Renderer/RendererCom.h"
#include <Component\Camera\FreeCameraCom.h>

#include <Component\Stage\StageEditorCom.h>

//������
void SceneLGBT::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        auto& c = freeCamera->AddComponent<FreeCameraCom>();
        c->SetFocusPos({ 1000,1000,1000 });
        freeCamera->transform_->SetWorldPosition({ 100, 500, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //LGBT
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("LGBT");
        obj->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/lgbt.ui", Sprite::SpriteShader::DISSOLVE, false);
    }

    //�L�����̃V���A�����o�^
    for (int i = 0; i< int(RegisterChara::CHARA_LIST::MAX); ++i)
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName(std::string("p" + std::to_string(i)).c_str());
        obj->transform_->SetWorldPosition({ 100,0,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST(i), obj);
        {
            std::shared_ptr<GameObject> arm = GameObjectManager::Instance().Create();
            arm->SetName("armChild");
            arm->transform_->SetScale({ 0.5f,0.5f,0.5f });
            std::shared_ptr<RendererCom> r = arm->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
            r->LoadModel("Data/Model/player_arm/player_arm.mdl");

        }
    }

    //�X�e�[�W�̃V���A�����o�^
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, -1000 });
        stageObj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        //�X�e�[�W
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //���萶��
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
        //Json����I�u�W�F�N�g�z�u
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");

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