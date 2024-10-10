#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

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
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);
    void ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //描画終了処理
    void End(ID3D11DeviceContext* dc);

    //位置、姿勢、大きさをバッファに格納
    void ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data);

    //imgui
    void ImGui();

    //バッファー作成
    void CreateBuffer();

public:

    //生成数
    void SetCount(const int& index) { m_instancecount = index; }
    const int& GetCount() const { return m_instancecount; }

private:

    //インスタンシングの情報
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };
    };
    std::unique_ptr<Instance[]> m_cpuinstancedata;

    //オブジェクトのコンスタントバッファ
    struct objectconstants
    {
        DirectX::XMFLOAT4X4 transform = {};
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

    //汎用のコンスタントバッファ
    struct m_general
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
        int statictype = 0;
        DirectX::XMFLOAT3 generaldummy = {};
    };

private:
    std::unique_ptr<ConstantBuffer<objectconstants>> m_objectconstants;
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;
    std::unique_ptr<ConstantBuffer<m_general>> m_generalconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_instancedata;

public:
    int m_instancecount = 0;
};