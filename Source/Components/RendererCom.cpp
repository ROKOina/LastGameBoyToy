#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

//�R���X�g���N�^
RendererCom::RendererCom(SHADERMODE mode, BLENDSTATE blendmode)
{
  m_blend = static_cast<int>(blendmode);
  m_modelshader = std::make_unique<ModelShader>(static_cast<int>(mode));

#ifdef _DEBUG
  shaderMode = mode;

#endif // _DEBUG
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

  // �萔�o�b�t�@��GPU�ɐݒ�
  if (variousConstant.get() != nullptr) {
    variousConstant->UpdateConstantBuffer(dc);
  }

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

    // �萔�o�b�t�@�̍X�V
    if (variousConstant.get() != nullptr) {
      variousConstant->Update(elapsedTime);
    }
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

#ifdef _DEBUG
  MaterialSelector();

#endif // _DEBUG

  if (variousConstant.get() != nullptr) {
    ImGui::Separator();
    ImGui::Text("ConstantBuffer");

    variousConstant->DrawGui();
  }
}

// ���f���̓ǂݍ���
void RendererCom::LoadModel(const char* filename)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();
  std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();
  m->Load(device, filename);

  model_ = std::make_unique<Model>(m);

#ifdef _DEBUG
  modelFilePath = filename;

#endif // _DEBUG
}

void RendererCom::LoadMaterial(const char* filename)
{
  assert(model_.get() != nullptr && "���f����ǂݍ��ޑO�Ɋ֐����Ăяo���Ă���");

  ID3D11Device* device = Graphics::Instance().GetDevice();
  model_->GetResource()->LoadMaterial(device, filename);
}

#ifdef _DEBUG
#include "../Framework.h"
#include <shlwapi.h>
#include "../Graphics/Shaders/Texture.h"
#include "../Dialog.h"

void RendererCom::MaterialSelector()
{
  ImGui::Separator();

  std::vector<const char*> materialNames = {};

  //�}�e���A�����̕\��(�p�^�[��1)
  ImGui::Text("Materials");
  if (model_->GetResource() != nullptr)
  {
    int materialIndex = 0;
    for (const ModelResource::Material& material : model_->GetResource()->GetMaterials())
    {
      ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

      if (selectionMaterialIndex == materialIndex)
      {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
      }

      materialNames.push_back(material.name.c_str());

      ImGui::TreeNodeEx(&material, nodeFlags, material.name.c_str());

      if (ImGui::IsItemClicked())
      {
        selectionMaterialIndex = materialIndex;
      }

      ImGui::TreePop();

      ++materialIndex;
    }
  }

  //�}�e���A�����̕\��(�p�^�[��2)
  ImGui::Combo("Materials", &selectionMaterialIndex, &materialNames[0], materialNames.size(), 5);

  ImGui::Separator();

  // �}�e���A���̃v���p�e�B��\��
  ImGui::Text("Property");
  ModelResource::Material* material = GetSelectionMaterial();
  if (material != nullptr)
  {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
      // ���x�������d�����Ă��܂��ƋN������̑Ή�
      ImGui::PushID(0);
      {
        char name[256];
        ::strncpy_s(name, sizeof(name), material->name.c_str(), sizeof(name));
        if (ImGui::InputText("Name", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
        {
          material->name = name;
        }
      }
      ImGui::PopID();

      ImGui::ColorEdit4("Color", &material->color.x, ImGuiColorEditFlags_None);
      ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Metallic", &material->Metalness, 0.01f, 0.0f, 1.0f);
      ImGui::ColorEdit3("EmissiveColor", &material->emissivecolor.x, ImGuiColorEditFlags_None);
      ImGui::DragFloat("EmissivePower", &material->emissiveintensity, 0.1f, 0.0f, 100.0f);

      // �}�e���A���t�@�C�����㏑��
      if (ImGui::Button("Save")) {
        ExportMaterialFile();
      }
    }
  }
}

ModelResource::Material* RendererCom::GetSelectionMaterial()
{
  if (model_.get() != nullptr)
  {
    const std::vector<ModelResource::Material>& materials = model_->GetResource()->GetMaterials();
    if (selectionMaterialIndex >= 0 && selectionMaterialIndex < static_cast<int>(materials.size()))
    {
      return const_cast<ModelResource::Material*>(&materials.at(selectionMaterialIndex));
    }
  }
  return nullptr;
}

void RendererCom::ExportMaterialFile()
{
  static const char* filter = "Material Files(*.Material)\0*.Material;\0All Files(*.*)\0*.*;\0\0";

  char filename[256] = { 0 };
  DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "mdl", Framework::GetInstance()->GetHWND());
  if (result == DialogResult::OK)
  {
    model_->GetResource()->MaterialSerialize(filename);
  }
}

#endif // _DEBUG