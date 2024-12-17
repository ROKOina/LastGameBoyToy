#include "SceneUeno.h"
#include <Component\UI\PlayerUI.h>
#include <Component\Light\LightCom.h>
#include <Component\Camera\FreeCameraCom.h>
#include <Component\Stage\GateGimmickCom.h>
#include <Component\Collsion\RayCollisionCom.h>
#include <Component\Stage\StageEditorCom.h>
#include "Component\PostEffect\PostEffect.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"
#include <Netwark\Photon\StdIO_UIListener.h>
#include <Graphics\SkyBoxManager\SkyBoxManager.h>

//������
void SceneUeno::Initialize()
{
    //�|�X�g�G�t�F�N�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //���C�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

#ifdef _DEBUG
    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();
#endif

    //�X�e�[�W
    {
        auto& stageObj = GameObjectManager::Instance().Create();
        stageObj->SetName("stage");
        stageObj->transform_->SetWorldPosition({ 0, 0, 0 });
        stageObj->transform_->SetScale({ 0.005f, 0.005f, 0.005f });
        std::shared_ptr<RendererCom> r = stageObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        r->LoadModel("Data/Model/MatuokaStage/StageJson/DrawStage.mdl");
        r->SetOutlineColor({ 0.000f, 0.932f, 1.000f });
        r->SetOutlineIntensity(5.5f);
        stageObj->AddComponent<RayCollisionCom>("Data/canyon/stage.collision");

        //�X�e�[�W
        StageEditorCom* stageEdit = stageObj->AddComponent<StageEditorCom>().get();
        //���萶��
        stageEdit->PlaceStageRigidCollider("Data/Model/MatuokaStage/", "StageJson/ColliderStage.mdl", "__", 0.005f);
        //Json����I�u�W�F�N�g�z�u
        stageEdit->PlaceJsonData("Data/SerializeData/StageGimic/GateGimic.json");
        //�z�u�����X�e�[�W�I�u�W�F�N�g�̒�����Gate���擾
        StageEditorCom::PlaceObject placeObj = stageEdit->GetPlaceObject("Gate");
        for (auto& obj : placeObj.objList)
        {
            DirectX::XMFLOAT3 pos = obj->transform_->GetWorldPosition();

            GateGimmick* gate = obj->GetComponent<GateGimmick>().get();
            gate->SetDownPos(pos);
            gate->SetUpPos({ pos.x, 1.85f, pos.z });
            gate->SetMoveSpeed(0.1f);
        }
    }

    //�v���C���[
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("player");
        obj->transform_->SetWorldPosition({ 0,-1,0 });
        RegisterChara::Instance().SetCharaComponet(RegisterChara::CHARA_LIST::INAZAWA, obj);
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    // �X�J�C�{�b�N�X�̐ݒ�
    std::array<const char*, 4> filepath = {
      "Data\\Texture\\CosmicCoolCloudBottom.DDS",
      "Data\\Texture\\diffuse_iem.dds",
      "Data\\Texture\\specular_pmrem.dds",
      "Data\\Texture\\lut_ggx.DDS"
    };
    SkyBoxManager::Instance().LoadSkyBoxTextures(filepath);

    //�l�b�g�厖
    StdIO_UIListener* l = new StdIO_UIListener();
    photonNet = std::make_unique<BasicsApplication>(l);
}

// �I����
void SceneUeno::Finalize()
{
    photonNet->close();
}

// �X�V����
void SceneUeno::Update(float elapsedTime)
{
    //�l�b�g�X�V
    photonNet->run(elapsedTime);

    //UI����
    PlayerUIManager::Instance().UIUpdate(elapsedTime);

    //�Q�[���I�u�W�F�N�g�̍X�V
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneUeno::Render(float elapsedTime)
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

    //�I�u�W�F�N�g�����֐�
#ifdef _DEBUG
    NewObject();
#endif

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());

    //imgui
    photonNet->ImGui();
}

//�I�u�W�F�N�g�����֐�
void SceneUeno::NewObject()
{
    ImGui::Begin("CreateObject");

    if (ImGui::Button("gpuparticle"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testgpuparticle");
        obj->AddComponent<GPUParticle>(nullptr, 1000);
    }
    ImGui::SameLine();
    if (ImGui::Button("cpuparticle"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testcpuparticle");
        obj->AddComponent<CPUParticle>(nullptr, 10000);
    }
    ImGui::SameLine();
    if (ImGui::Button("ui"))
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("testui");
        obj->AddComponent<Sprite>(nullptr, Sprite::SpriteShader::DEFALT, true);
    }
    if (ImGui::Button("light"))
    {
        std::shared_ptr<GameObject>obj = GameObjectManager::Instance().Create();
        obj->SetName("testlight");
        obj->AddComponent<Light>(nullptr);
    }

    ImGui::End();
}