#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/ModelShader.h"

class RendererCom : public Component
{
    //コンポーネントオーバーライド
public:
    RendererCom(int shaderslot, int blendmode);
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

    //Rendererクラス
public:
    // モデルの読み込み
    void LoadModel(const char* filename);

    // モデルの取得
    Model* GetModel() const { return model_.get(); }

private:
    std::unique_ptr<Model>	model_;
    std::unique_ptr<ModelShader>m_modelshader;
    int m_blend = 9;
};