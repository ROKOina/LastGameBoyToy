#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

#define MAX_BONES 256

class ModelShader
{
public:
    ModelShader(SHADER_ID_MODEL shader);
    ~ModelShader() {};

    //描画初期設定
    void Begin(ID3D11DeviceContext* dc, int blendmode, RASTERIZERSTATE rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK);
    void ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);

    //描画処理
    void SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh);

    //サブセット毎の描画
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);
    void ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //描画終了処理
    void End(ID3D11DeviceContext* dc);

public:
    //オブジェクトのコンスタントバッファ
    struct objectconstants
    {
        DirectX::XMFLOAT4X4 BoneTransforms[MAX_BONES] = {};
    };

    //サブセットのコンスタントバッファ
    struct subsetconstants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 emissivecolor = { 1.0f,1.0f,1.0f };
        float             emissiveintensity = 0;
        float             Metalness = 0;
        float             Roughness = 0;
        DirectX::XMFLOAT2 dummy = {};
    };

private:
    std::unique_ptr<ConstantBuffer<objectconstants>> m_objectconstants;
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
};
