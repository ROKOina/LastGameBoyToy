#include "InstanceRendererCom.h"
#include "TransformCom.h"
#include <Graphics/Model/ResourceManager.h>
#include "GameSource/Math/Mathf.h"
#include <random>

//�R���X�g���N�^
InstanceRenderer::InstanceRenderer(SHADER_ID_MODEL id, int maxinstance, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode, bool shadowrender)
{
    m_blend = blendmode;
    m_instancemodelshader = std::make_unique<InstanceModelShader>(id, maxinstance);
    count = maxinstance;
    m_shadowrender = shadowrender;
    shaderID = id;
}

//�`��
void InstanceRenderer::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //�Z�b�g
    m_instancemodelshader->Begin(dc, m_blend, m_depth, m_rasterizerState);

    //���f����`��
    for (auto& mesh : model_->GetResource()->GetMeshes())
    {
        for (auto& subset : mesh.subsets)
        {
            //���_�E�C���f�b�N�X�o�b�t�@���ݒ�
            m_instancemodelshader->SetBuffer(dc, model_->GetNodes(), mesh);

            //�T�u�Z�b�g���ŕ`��
            m_instancemodelshader->SetSubset(dc, subset);
        }
    }

    //���
    m_instancemodelshader->End(dc);
}

//�X�V����
void InstanceRenderer::Update(float elapsedTime)
{
    // ���f���̍X�V
    if (model_ != nullptr)
    {
        DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
        model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
    }
}

//�e�`��
void InstanceRenderer::ShadowRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //�e��`�悷�邩�̃t���O
    if (m_shadowrender)
    {
        //�Z�b�g
        m_instancemodelshader->ShadowBegin(dc, BLENDSTATE::REPLACE, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK);

        //���f����`��
        for (auto& mesh : model_->GetResource()->GetMeshes())
        {
            for (auto& subset : mesh.subsets)
            {
                //���_�E�C���f�b�N�X�o�b�t�@���ݒ�
                m_instancemodelshader->SetBuffer(dc, model_->GetNodes(), mesh);

                //�T�u�Z�b�g���ŕ`��
                m_instancemodelshader->ShadowSetSubset(dc, subset);
            }
        }

        //���
        m_instancemodelshader->End(dc);
    }
}

//GUI
void InstanceRenderer::OnGUI()
{
    ImGui::SameLine();
    ImGui::Checkbox("shadow", &m_shadowrender);

    //�f�o�b�O�p�Ƀu�����h���[�h�ݒ�
    constexpr const char* BlendName[] =
    {
      "NONE",
      "ALPHA",
      "ADD",
      "SUBTRACT",
      "REPLACE",
      "MULTIPLY",
      "LIGHTEN",
      "DARKEN",
      "SCREEN",
      "MULTIPLERENDERTARGETS",
    };
    //�u�����h���[�h�ݒ胊�X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
    static_assert(ARRAYSIZE(BlendName) != static_cast<int>(BLENDSTATE::MAX) - 1, "BlendName Size Error!");

    //�u�����h���[�h�ݒ�
    int blendMode = static_cast<int>(m_blend);
    ImGui::Combo("BlendMode", &blendMode, BlendName, static_cast<int>(BLENDSTATE::MAX), static_cast<int>(BLENDSTATE::MAX));
    m_blend = static_cast<BLENDSTATE>(blendMode);

    //�f�o�b�O�p�Ƀf�v�X���[�h�ݒ�
    constexpr const char* DepthName[] =
    {
      "NONE",
      "ZT_ON_ZW_ON",
      "ZT_ON_ZW_OFF",
      "ZT_OFF_ZW_ON",
      "ZT_OFF_ZW_OFF",
      "SILHOUETTE",
      "MASK",
      "APPLY_MASK",
      "EXCLUSIVE",
    };
    //�f�v�X���[�h�ݒ胊�X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
    static_assert(ARRAYSIZE(DepthName) != static_cast<int>(DEPTHSTATE::MAX) - 1, "DepthName Size Error!");

    //�f�v�X�X�e���V�����[�h�ݒ�
    int depthMode = static_cast<int>(m_depth);
    ImGui::Combo("DepthMode", &depthMode, DepthName, static_cast<int>(DEPTHSTATE::MAX), static_cast<int>(DEPTHSTATE::MAX));
    m_depth = static_cast<DEPTHSTATE>(depthMode);

    //�f�o�b�O�p�Ƀ��X�^���C�Y���[�h�ݒ�
    constexpr const char* RasterizerName[] =
    {
      "SOLID_CULL_NONE",
      "SOLID_CULL_BACK",
      "SOLID_CULL_FRONT",
      "WIREFRAME",
    };
    //���X�^���C�Y���[�h�ݒ胊�X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
    static_assert(ARRAYSIZE(RasterizerName) != static_cast<int>(RASTERIZERSTATE::MAX) - 1, "RasterizerName Size Error!");

    //���X�^���C�Y���[�h�ݒ�
    int rasMode = static_cast<int>(m_rasterizerState);
    ImGui::Combo("RasterizerMode", &rasMode, RasterizerName, static_cast<int>(RASTERIZERSTATE::MAX), static_cast<int>(RASTERIZERSTATE::MAX));
    m_rasterizerState = static_cast<RASTERIZERSTATE>(rasMode);

    if (ImGui::DragInt((char*)u8"������", &count, 1.0f, 1, 100))
    {
        m_instancemodelshader->m_instancecount = count;
        m_instancemodelshader->CreateBuffer();
    }
    m_instancemodelshader->ImGui();
}

//���f���ǂݍ���
void InstanceRenderer::LoadModel(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();

    //���\�[�X�}�l�[�W���[�ɓo�^����Ă��邩
    if (!ResourceManager::Instance().JudgeModelFilename(filename))
    {
        m->Load(device, filename);
        ResourceManager::Instance().RegisterModel(filename, m);	//���\�[�X�}�l�[�W���[�ɒǉ�����
    }
    else
    {
        m = ResourceManager::Instance().LoadModelResource(filename);	//���[�h����
    }

    model_ = std::make_unique<Model>(m);
}