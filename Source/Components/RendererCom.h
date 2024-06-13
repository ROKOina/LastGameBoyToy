#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/ModelShader.h"

class RendererCom : public Component
{
  //コンポーネントオーバーライド
public:
  RendererCom(SHADERMODE mode, BLENDSTATE blendmode);
  ~RendererCom() {}

  // 名前取得
  const char* GetName() const override { return "Renderer"; }

  // 開始処理
  void Start() override;

  //描画
  void Render();

  // 更新処理
  void Update(float elapsedTime) override;

  // GUI描画
  void OnGUI() override;

  // モデルの読み込み
  void LoadModel(const char* filename);

  // マテリアルのみ読み込む
  void LoadMaterial(const char* filename);

  // モデルの取得
  Model* GetModel() const { return model_.get(); }

private:
#ifdef _DEBUG
  // マテリアルを選択するImGui
  void MaterialSelector();

  ModelResource::Material* GetSelectionMaterial();

  // 編集したマテリアルファイルを書き出す
  void ExportMaterialFile();

  // 割り当てられているシェーダーの定数バッファを編集
  void EditConstantBuffer();

#endif

private:
  std::unique_ptr<Model>	model_;
  std::unique_ptr<ModelShader> m_modelshader;
  int m_blend = 9;

#ifdef _DEBUG
  SHADERMODE          shaderMode;
  int									selectionMaterialIndex = -1;
  bool								hiddenProperty = false;
  std::string					modelFilePath;

#endif // _DEBUG

};