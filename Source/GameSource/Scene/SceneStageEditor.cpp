#include "Graphics/Graphics.h"
#include "Graphics/Light/LightManager.h"
#include "Graphics/SkyBoxManager/SkyBoxManager.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "imgui.h"
#include "Components\System\GameObject.h"
#include "Components\RendererCom.h"
#include "Components\TransformCom.h"
#include "Components\CameraCom.h"
#include "Components\RendererCom.h"
#include "GameSource/GameScript/FreeCameraCom.h"
#include "Graphics/Model/ResourceManager.h"
#include "./Dialog.h"
#include "Framework.h"
#include "SceneStageEditor.h"
#include "./SceneManager.h"
#include <cstring>
#include "./Components/RayCollisionCom.h"

void SceneStageEditor::Initialize()
{
    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

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

void SceneStageEditor::Finalize()
{
    GameObjectManager::Instance().AllRemove();
}

void SceneStageEditor::Update(float elapsedTime)
{
    //�I�u�W�F�N�g�z�u
    ObjectPlace();

    // �Q�[���I�u�W�F�N�g�̍X�V
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneStageEditor::Render(float elapsedTime)
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

    //imgui
    ImGui();
}

//imgui
void SceneStageEditor::ImGui()
{
    StageSelect();
    ObjectRegister();

    //�z�u����I�u�W�F�N�g�I��
    for (auto& objName : gameObjcts)
    {
        ImGui::Text(objName.first.c_str());

        if (!objName.second.empty())
        {
            if (ImGui::Button("Place"))
            {
                selectObjName = objName.second;
            }
        }
        else
        {
            if (ImGui::Button("None"))
            {
                //�X�e�[�W���f���̐ݒ�
                static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

                char filename[256] = { 0 };
                DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
                if (result == DialogResult::OK)
                {
                    objName.second = filename;
                }
            }
        }
    }
}

// ���f������
GameObj SceneStageEditor::ImportModel(const char* filename)
{
    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("stage");
    obj->transform_->SetWorldPosition({ 0, 3.7f, 0 });
    obj->transform_->SetScale({ 0.8f, 0.8f, 0.8f });
    std::shared_ptr<RendererCom> r = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    r->LoadModel(filename);

    return obj;
}

void SceneStageEditor::StageSelect()
{
    //�{�^���ŃI�[�v��
    if (ImGui::Button("StageSelect"))
    {
        //�X�e�[�W���f���̐ݒ�
        static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

        char filename[256] = { 0 };
        DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::GetInstance()->GetHWND());
        if (result == DialogResult::OK)
        {
            stageObj = ImportModel(filename);
        }
    }
}

//�z�u����I�u�W�F�N�g�o�^
void SceneStageEditor::ObjectRegister()
{
    ImGui::InputText("RegistObjName",registerObjName, sizeof(registerObjName));
    if (ImGui::Button("Regist"))
    {
        std::string copyname;
        copyname = registerObjName;

        gameObjcts[copyname] = "";
        registerObjName[0] = '\0';
    }
}

//�X�e�[�W�ɔz�u
void SceneStageEditor::ObjectPlace()
{
    Mouse& mouse = Input::Instance().GetMouse();
    if (stageObj != nullptr && mouse.GetButtonDown() & Mouse::BTN_LEFT)
    {
        HitResult result;
        if (MouseVsStage(result))
        {
            //�z�u
            GameObj obj = GameObjectManager::Instance().Create();
            obj->transform_->SetWorldPosition(result.position);
        }
    }
}

bool SceneStageEditor::MouseVsStage(HitResult hit)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    CameraCom* cameraCom = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>().get();

    // �r���[�|�[�g
    D3D11_VIEWPORT view_port;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &view_port);

    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&cameraCom->GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&cameraCom->GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    Mouse& mouse = Input::Instance().GetMouse();

    // �}�E�X�J�[�\�����W���擾
    DirectX::XMFLOAT3 screen_position;
    screen_position.x = static_cast<float>(mouse.GetPositionX());
    screen_position.y = static_cast<float>(mouse.GetPositionY());
    screen_position.z = 0;

    // ���C�̎n�_
    DirectX::XMFLOAT3 world_start;
    DirectX::XMStoreFloat3(&world_start,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	�r���[�|�[�g����X���W
            view_port.TopLeftY,	//	�r���[�|�[�g����Y���W
            view_port.Width,	//	�r���[�|�[�g��
            view_port.Height,	//	�r���[�|�[�g����
            0.0f,	// �[�x�l�̍ŏ��l
            1.0f,	// �[�x�l�̍ő�l
            Projection,	//	�v���W�F�N�V�����s��
            View,	//	�r���[�s��
            World	//	���[���h�s��
        )
    );

    screen_position.z = 1;

    // ���C�̏I�_
    DirectX::XMFLOAT3 world_end;
    DirectX::XMStoreFloat3(&world_end,
        DirectX::XMVector3Unproject(
            DirectX::XMLoadFloat3(&screen_position),
            view_port.TopLeftX,	//	�r���[�|�[�g����X���W
            view_port.TopLeftY,	//	�r���[�|�[�g����Y���W
            view_port.Width,	//	�r���[�|�[�g��
            view_port.Height,	//	�r���[�|�[�g����
            0.0f,	// �[�x�l�̍ŏ��l
            1.0f,	// �[�x�l�̍ő�l
            Projection,	//	�v���W�F�N�V�����s��
            View,	//	�r���[�s��
            World	//	���[���h�s��
        )
    );

    return Collision::IntersectRayVsModel(world_start, world_end, stageObj->GetComponent<RendererCom>()->GetModel(), hit);
}
