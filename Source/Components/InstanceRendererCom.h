#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/InstanceModelShader.h"

class InstanceRenderer :public Component
{
  // コンポーネントオーバーライド
public:
  InstanceRenderer(SHADER_ID_MODEL id, int maxinstance, BLENDSTATE blendmode, DEPTHSTATE depthmode = DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE rasterizermode = RASTERIZERSTATE::SOLID_CULL_BACK, bool shadowrender = true);
  ~InstanceRenderer() {}

  // 名前取得
  const char* GetName() const override { return "InstanceRenderer"; }

  // 開始処理
  void Start() override {};

  // 描画
  void Render();

  // 更新処理
  void Update(float elapsedTime) override;

  //影描画
  void ShadowRender();

  // GUI描画
  void OnGUI() override;

  // モデルの読み込み
  void LoadModel(const char* filename);

  /// <summary>
  /// バッチ描画オブジェクトの生成 by杉
  /// </summary>
  /// <param name="isChildObject"> 子オブジェクトとして生成するか</param>
  /// <param name=""> TRUE = 子オブジェクト</param>
  /// <param name=""> FALSE = 親子関係なしのオブジェクト</param>
  GameObj CreateInstance(bool isChildObject);

public:
  //id取得
  SHADER_ID_MODEL GetShaderMode() { return shaderID; }

  // モデルの取得
  Model* GetModel() const { return model_.get(); }

private:
  std::unique_ptr<Model>	model_;
  std::unique_ptr<InstanceModelShader>m_instancemodelshader;
  RASTERIZERSTATE m_rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK;
  BLENDSTATE m_blend = BLENDSTATE::MULTIPLERENDERTARGETS;
  DEPTHSTATE m_depth = DEPTHSTATE::ZT_ON_ZW_ON;
  SHADER_ID_MODEL     shaderID = SHADER_ID_MODEL::DEFAULT;
  bool m_shadowrender = true;

};