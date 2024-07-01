#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

//コンストラクタ
RendererCom::RendererCom(SHADER_ID_MODEL id, BLENDSTATE blendmode)
{
  m_blend = static_cast<int>(blendmode);
  m_modelshader = std::make_unique<ModelShader>(id);

#ifdef _DEBUG
  shaderID = id;

#endif // _DEBUG
}

// 開始処理
void RendererCom::Start()
{
}

//描画
void RendererCom::Render()
{
  Graphics& Graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

  // 定数バッファをGPUに設定
  if (variousConstant.get() != nullptr) {
    variousConstant->UpdateConstantBuffer(dc);
  }

  //セット
  m_modelshader->Begin(dc, m_blend);

  //モデルを描画
  for (auto& mesh : model_->GetResource()->GetMeshes())
  {
    for (auto& subset : mesh.subsets)
    {
      //頂点・インデックスバッファ等設定
      m_modelshader->SetBuffer(dc, model_->GetNodes(), mesh);

      //サブセット毎で描画
      m_modelshader->SetSubset(dc, subset);
    }
  }

  //解放
  m_modelshader->End(dc);
}

// 更新処理
void RendererCom::Update(float elapsedTime)
{
  // モデルの更新
  if (model_ != nullptr)
  {
    DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
    model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));

    // 定数バッファの更新
    if (variousConstant.get() != nullptr) {
      variousConstant->Update(elapsedTime);
    }
  }
}

// GUI描画
void RendererCom::OnGUI()
{
  //デバッグ用にブレンドモード設定
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
  //ブレンドモード設定リストとのサイズが違うとエラーを出す
  static_assert(ARRAYSIZE(BlendName) != static_cast<int>(BLENDSTATE::MAX) - 1, "BlendName Size Error!");
  //ブレンドモード設定
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

// モデルの読み込み
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
  assert(model_.get() != nullptr && "モデルを読み込む前に関数を呼び出している");

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

  //マテリアル情報の表示(パターン1)
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

  //マテリアル情報の表示(パターン2)
  ImGui::Combo("Materials", &selectionMaterialIndex, &materialNames[0], materialNames.size(), 5);

  ImGui::Separator();

  // マテリアルのプロパティを表示
  ImGui::Text("Property");
  ModelResource::Material* material = GetSelectionMaterial();
  if (material != nullptr)
  {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
      // ラベル名が重複してしまうと起きる問題の対応
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

      // テクスチャ編集
      TextureGui(material);

      ImGui::ColorEdit4("Color", &material->color.x, ImGuiColorEditFlags_None);
      ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Metallic", &material->Metalness, 0.01f, 0.0f, 1.0f);
      ImGui::ColorEdit3("EmissiveColor", &material->emissivecolor.x, ImGuiColorEditFlags_None);
      ImGui::DragFloat("EmissivePower", &material->emissiveintensity, 0.1f, 0.0f, 100.0f);

      // マテリアルファイルを上書き
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

void RendererCom::TextureGui(ModelResource::Material* material)
{
  ImGui::Separator();
  ImGui::Text("TextureEdit");

  char id[12] = "ModelFile";
  ImGui::PushID(id);
  ImGui::Text("Model");
  ImGui::SameLine();
  if (ImGui::Button("...")) {
    char modelFile[256];    
    ::strncpy_s(modelFile, sizeof(modelFile), material->textureFilename[0].c_str(), sizeof(modelFile));

    const char* filter = "Texture Files(*.mdl)\0*.mdl;\0All Files(*.*)\0*.*;\0\0";
    DialogResult result = Dialog::OpenFileName(modelFile, sizeof(modelFile), filter, nullptr, Framework::GetInstance()->GetHWND());
    if (result == DialogResult::OK)
    {
      filePathDriveToModel = modelFile;
    }
  }
  ImGui::PopID();

  if (filePathDriveToModel.empty())return;

  // 適用するテクスチャを選択                
  char textureFile[256];
  const char* fileKinds[6] = { "Diffuse","Normal","Metallic","Roughness","Ao","Emissive" };
  for (int i = 0; i < 6; ++i)
  {
    ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));

    ImGui::PushID(&material->textureFilename[i]);
    if (ImGui::Button("..."))
    {
      const char* filter = "Texture Files(*.DDS;*.png;*.tga;*.jpg;*.tif)\0*.DDS;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
      DialogResult result = Dialog::OpenFileName(textureFile, sizeof(textureFile), filter, nullptr, Framework::GetInstance()->GetHWND());
      if (result == DialogResult::OK)
      {
        char drive[32], dir[256], dirname[256];
        ::_splitpath_s(filePathDriveToModel.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
        dirname[strlen(dirname) - 1] = '\0';
        char relativeTextureFile[MAX_PATH];
        PathRelativePathToA(relativeTextureFile, dirname, FILE_ATTRIBUTE_DIRECTORY, textureFile, FILE_ATTRIBUTE_ARCHIVE);

        // 読み込み
        material->textureFilename[i] = relativeTextureFile;
        material->LoadTexture(Graphics::Instance().GetDevice(), textureFile, i);
      }
    }
    ImGui::SameLine();

    ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));
    if (ImGui::InputText(fileKinds[i], textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
      material->textureFilename[i] = textureFile;

      char drive[32], dir[256], fullPath[256];
      ::_splitpath_s(filePathDriveToModel.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
      ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
      material->LoadTexture(Graphics::Instance().GetDevice(), fullPath, i);
    }
    ImGui::Text("TextureResource");
    ImGui::Image(material->shaderResourceView[i].Get(), { 156, 156 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::PopID();
  }
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