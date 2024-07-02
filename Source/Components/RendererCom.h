#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/ModelShader.h"
#include "Graphics/Shaders/3D/VariousShaderConstants.h"

class RendererCom : public Component
{
  // �R���|�[�l���g�I�[�o�[���C�h
public:
  RendererCom(SHADER_ID_MODEL id, BLENDSTATE blendmode, RASTERIZERSTATE rs = RASTERIZERSTATE::SOLID_CULL_BACK);
  ~RendererCom() {}

  // ���O�擾
  const char* GetName() const override { return "Renderer"; }

  // �J�n����
  void Start() override;

  // �`��
  void Render();

  // �X�V����
  void Update(float elapsedTime) override;

  // GUI�`��
  void OnGUI() override;

  // ���f���̓ǂݍ���
  void LoadModel(const char* filename);

  // �}�e���A���̂ݓǂݍ���
  void LoadMaterial(const char* filename);

  // ���f���̎擾
  Model* GetModel() const { return model_.get(); }

  template<class T>
  std::shared_ptr<T> SetVariousConstant() {
    std::shared_ptr<T> p = std::make_shared<T>();
    variousConstant = p;
    return p;
  }

  template<class T>
  std::shared_ptr<T> GetVariousConstant() {
    std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(variousConstant);
    if (p == nullptr) return nullptr;
    return p;
  }

  SHADER_ID_MODEL GetShaderMode() { return shaderID; }

private:
#ifdef _DEBUG
  // �}�e���A����I������ImGui
  void MaterialSelector();

  ModelResource::Material* GetSelectionMaterial();

  void TextureGui(ModelResource::Material*);

  // �ҏW�����}�e���A���t�@�C���������o��
  void ExportMaterialFile();

#endif

private:
  std::unique_ptr<Model>	model_;
  std::unique_ptr<ModelShader> m_modelshader;
  int m_blend = 9;
  RASTERIZERSTATE m_rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK;

  std::shared_ptr<BaseConstants> variousConstant = nullptr;

#ifdef _DEBUG
  SHADER_ID_MODEL     shaderID;
  int									selectionMaterialIndex = -1;
  bool								hiddenProperty = false;
  std::string					modelFilePath;
  std::string					filePathDriveToModel = "";

#endif // _DEBUG

};