#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/ModelShader.h"

class RendererCom : public Component
{
  //�R���|�[�l���g�I�[�o�[���C�h
public:
  RendererCom(int shaderslot, int blendmode);
  ~RendererCom() {}

  // ���O�擾
  const char* GetName() const override { return "Renderer"; }

  // �J�n����
  void Start() override;

  //�`��
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

private:
#ifdef _DEBUG
  // �}�e���A����I������ImGui
  void MaterialSelector();

  ModelResource::Material* GetSelectionMaterial();

  // �ҏW�����}�e���A���t�@�C���������o��
  void ExportMaterialFile();

#endif

private:
  std::unique_ptr<Model>	model_;
  std::unique_ptr<ModelShader>m_modelshader;
  int m_blend = 9;

#ifdef _DEBUG
  int									selectionMaterialIndex = -1;
  bool								hiddenProperty = false;
  std::string					modelFilePath;

#endif // _DEBUG

};