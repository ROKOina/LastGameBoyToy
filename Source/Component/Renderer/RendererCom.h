#pragma once

#include "Component\System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/3DShader/ModelShader.h"
#include "Graphics/3DShader/VariousShaderConstants.h"

class RendererCom : public Component
{
    // コンポーネントオーバーライド
public:
    RendererCom(SHADER_ID_MODEL id, BLENDSTATE blendmode, DEPTHSTATE depthmode = DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE rasterizermode = RASTERIZERSTATE::SOLID_CULL_BACK, bool shadowrender = true, bool silhoutterender = false);
    ~RendererCom() {}

    // 名前取得
    const char* GetName() const override { return "Renderer"; }

    // 開始処理
    void Start() override;

    // 描画
    void Render();

    // 更新処理
    void Update(float elapsedTime) override;

    //影描画
    void ShadowRender();

    //シルエット描画
    void SilhoutteRender();

    //guizmo
    void BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // GUI描画
    void OnGUI() override;

    // モデルの読み込み
    void LoadModel(const char* filename);

    // マテリアルのみ読み込む
    void LoadMaterial(const char* filename);

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

private:

    //ModelResourceで使われているマテリアルをの値を保存
    struct MaterialParameter
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
        float dissolveThreshold = 0.0f; // ディゾルブ閾値
        DirectX::XMFLOAT3 dissolveEdgeColor = { 1,1,1 }; // エッジの色
        float alpha = 1.0f;
    };
    MaterialParameter p;

public:

    // モデルの取得
    Model* GetModel() const { return model_.get(); }
    std::string GetModelPath() { return modelFilePath; }

    //バウンディングボックス
    DirectX::XMFLOAT3 GetBounds() { return bounds; }
    DirectX::XMFLOAT3 GetBoundsMin() { return BoundsMin; }

    //シェーダー取得
    SHADER_ID_MODEL GetShaderMode() { return shaderID; }

    //スレッド取得
    void JoinThred() { future.get(); }

    //マテリアルパラメータの設定関数
    void SetOutlineColor(const DirectX::XMFLOAT3& color) { p.outlineColor = color; }
    void SetOutlineIntensity(float intensity) { p.outlineintensity = intensity; }
    void SetDissolveThreshold(float threshold) { p.dissolveThreshold = threshold; }
    void SetDissolveEdgeColor(const DirectX::XMFLOAT3& color) { p.dissolveEdgeColor = color; }
    void SetAlpha(float a) { p.alpha = a; }

private:
    void ModelInitialize(const char* filename);

    // マテリアルを選択するImGui
    void MaterialSelector();

    //マテリアル取得関数
    ModelResource::Material* GetSelectionMaterial();
    std::vector<ModelResource::Material*> RendererCom::GetAllMaterials();

    void TextureGui(ModelResource::Material*);

    // 編集したマテリアルファイルを書き出す
    void ExportMaterialFile();

    //マテリアルの変更
    void ChangeMaterialParameter();

private:
    std::unique_ptr<Model>	model_;
    std::unique_ptr<ModelShader> m_modelshader;
    std::unique_ptr<ModelShader>m_shadow;
    std::unique_ptr<ModelShader>m_silhoutte;
    bool m_shadowrender = true;
    bool m_silhoutterender = true;
    RASTERIZERSTATE m_rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK;
    BLENDSTATE m_blend = BLENDSTATE::MULTIPLERENDERTARGETS;
    DEPTHSTATE m_depth = DEPTHSTATE::ZT_ON_ZW_ON;
    std::shared_ptr<BaseConstants> variousConstant = nullptr;
    DirectX::XMFLOAT3       BoundsMin;
    DirectX::XMFLOAT3       BoundsMax;
    DirectX::XMFLOAT3 bounds;
    SHADER_ID_MODEL     shaderID = SHADER_ID_MODEL::DEFERRED;
    int							selectionMaterialIndex = -1;
    bool						hiddenProperty = false;
    std::string					modelFilePath;
    std::string					filePathDriveToModel = "";

    //モデル読み込みをスレッド化
    std::future<void> future;
};