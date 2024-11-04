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

    // モデルの取得
    Model* GetModel() const { return model_.get(); }

    DirectX::XMFLOAT3 GetBounds() { return bounds; }
    DirectX::XMFLOAT3 GetBoundsMin() { return BoundsMin; }

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

    void SetRenderShadow(const bool& flag) { m_shadowrender = flag; }
    void SetRenderSilhoutte(const bool& flag) { m_silhoutterender = flag; }
    
    SHADER_ID_MODEL GetShaderMode() { return shaderID; }

    void JoinThred() { future.get(); }

private:
    void ModelInitialize(const char* filename);

#ifdef _DEBUG
    // マテリアルを選択するImGui
    void MaterialSelector();

    ModelResource::Material* GetSelectionMaterial();

    void TextureGui(ModelResource::Material*);

    // 編集したマテリアルファイルを書き出す
    void ExportMaterialFile();

#endif

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
#ifdef _DEBUG
    int							selectionMaterialIndex = -1;
    bool						hiddenProperty = false;
    std::string					modelFilePath;
    std::string					filePathDriveToModel = "";

#endif // _DEBUG

    //モデル読み込みをスレッド化
    std::future<void> future;
};