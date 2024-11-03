#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

class TransformCom;

class InstanceModelShader
{
public:
    InstanceModelShader(SHADER_ID_MODEL shader, int count);
    ~InstanceModelShader() {};

    //描画初期設定
    void Begin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);
    void ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);

    //描画処理
    void SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh);

    //サブセット毎の描画
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, const std::vector<ModelResource::Material>& materials);
    void ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //描画終了処理
    void End(ID3D11DeviceContext* dc);

    // 姿勢をバッファに格納
    void ReplaceBufferContents();

    //imgui
    void ImGui();

    //バッファー作成
    void CreateBuffer();

public: // by 杉
    // バッチ描画に使用する姿勢の追加
    void AddInstance(std::weak_ptr<TransformCom> transform) { iModelTransforms.push_back(transform); }
    // バッチ描画に使用する姿勢の削除
    void RemoveInstance(std::weak_ptr<TransformCom> transform);

    const int& GetInstanceCount() const { return iModelTransforms.size(); }

private:
    //サブセットのコンスタントバッファ
    struct subsetconstants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 emissivecolor = { 1.0f,1.0f,1.0f };
        float             emissiveintensity = 0;
        float             Metalness = 0;
        float             Roughness = 0;
        float             alpha = 0.0f;
        float             dummy = {};
    };

    //汎用のコンスタントバッファ
    struct m_general
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
    };

private:
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;
    std::unique_ptr<ConstantBuffer<m_general>>       m_generalconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             m_instancedata;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayoutShadow;

    // バッチ描画するオブジェクトの姿勢 by杉
    std::vector<std::weak_ptr<TransformCom>> iModelTransforms;
};