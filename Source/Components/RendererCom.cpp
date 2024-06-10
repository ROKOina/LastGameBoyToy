#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

//�R���X�g���N�^
RendererCom::RendererCom(int shaderslot, int blendmode)
{
    m_blend = blendmode;
    m_modelshader = std::make_unique<ModelShader>(shaderslot);
}

// �J�n����
void RendererCom::Start()
{
}

//�`��
void RendererCom::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //�Z�b�g
    m_modelshader->Begin(dc, m_blend);

    //���f����`��
    for (auto& mesh : model_->GetResource()->GetMeshes())
    {
        for (auto& subset : mesh.subsets)
        {
            //���_�E�C���f�b�N�X�o�b�t�@���ݒ�
            m_modelshader->SetBuffer(dc, model_->GetNodes(), mesh);

            //�T�u�Z�b�g���ŕ`��
            m_modelshader->SetSubset(dc, subset);
        }
    }

    //���
    m_modelshader->End(dc);
}

// �X�V����
void RendererCom::Update(float elapsedTime)
{
    // ���f���̍X�V
    if (model_ != nullptr)
    {
        DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
        model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
    }
}

// GUI�`��
void RendererCom::OnGUI()
{
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
    ImGui::Combo("BlendMode", &m_blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);
}

// ���f���̓ǂݍ���
void RendererCom::LoadModel(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();
    m->Load(device, filename);

    model_ = std::make_unique<Model>(m);
}