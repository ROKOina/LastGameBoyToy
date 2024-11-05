#include "InstanceModelShader.h"
#include "Component/System/TransformCom.h"

#define MAX_STATIC_BATCH (300) // バッチ描画する最大数 by杉

//コンストラクタ
InstanceModelShader::InstanceModelShader(SHADER_ID_MODEL shader, int count)
{
    Graphics& Graphics = Graphics::Instance();

    //ファイル名
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;
    const char* GSPath = nullptr;

    //選択されたのが指定される
    switch (shader)
    {
    case SHADER_ID_MODEL::DEFERRED:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DeferredSetupPS.cso" };
        break;
    case SHADER_ID_MODEL::AREA_EFFECT_CIRCLE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\AreaEffectCirclePS.cso" };
        break;
    case SHADER_ID_MODEL::FAKE_DEPTH:
        VSPath = { "Shader\\FakeDepthVS.cso" };
        PSPath = { "Shader\\FakeDepthPS.cso" };
        break;
    case SHADER_ID_MODEL::SCI_FI_GATE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\SciFiGatePS.cso" };
        break;
    case SHADER_ID_MODEL::SILHOUETTE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\SilhouettePS.cso" };
        break;

    default:
        assert(!"引数のShaderに想定されていない値が入れられている");
    }

    // 頂点シェーダー
    {
        //入力レイアウト
        D3D11_INPUT_ELEMENT_DESC IED[] =
        {
            // 入力要素の設定
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            // 各インスタンスの姿勢行列
            // 4x4のワールド行列
            { "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
        CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));
    }
    // 頂点シェーダー ( 影用 )
    {
        //入力レイアウト
        D3D11_INPUT_ELEMENT_DESC IED[] =
        {
            // 入力要素の設定
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            // 各インスタンスの姿勢行列
            // 4x4のワールド行列
            { "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
        };
        VSPath = { "Shader\\InstanceShadowVS.cso" };
        CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshaderShadow.GetAddressOf(), m_inputlayoutShadow.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));
    }

    // ピクセルシェーダーとジオメトリシェーダー
    if (shader != SHADER_ID_MODEL::SHADOW)
    {
        CreatePsFromCso(Graphics.GetDevice(), PSPath, m_pixelshader.GetAddressOf());
    }
    {
        GSPath = { "Shader\\ShadowGS.cso" };
        CreateGsFromCso(Graphics.GetDevice(), GSPath, m_geometryshaderShadow.GetAddressOf());
    }

    // 定数バッファ
    {
        // オブジェクト用のコンスタントバッファ,サブセット用
        m_subsetconstants = std::make_unique<ConstantBuffer<subsetconstants>>(Graphics.GetDevice());
        m_generalconstants = std::make_unique<ConstantBuffer<m_general>>(Graphics.GetDevice());
    }

    // インスタンシングのデータをバッファに格納
    CreateBuffer();
}

//描画設定
void InstanceModelShader::Begin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //シェーダーのセット
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //レンダーステートの設定（ループの外で1回だけ設定）
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));

    // インスタンシングのデータを格納
    ReplaceBufferContents();
}
void InstanceModelShader::ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //シェーダーのセット
    dc->VSSetShader(m_vertexshaderShadow.Get(), nullptr, 0);
    dc->GSSetShader(m_geometryshaderShadow.Get(), nullptr, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->IASetInputLayout(m_inputlayoutShadow.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //レンダーステートの設定（ループの外で1回だけ設定）
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));

    // インスタンシングのデータを格納
    ReplaceBufferContents();
}

void InstanceModelShader::SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
    UINT strides[] = { sizeof(ModelResource::Vertex), sizeof(DirectX::XMFLOAT4X4) };
    UINT offsets[] = { 0, 0 };
    ID3D11Buffer* Buffers[] = { mesh.vertexBuffer.Get(), m_instancedata.Get() };
    dc->IASetVertexBuffers(0, _countof(strides), Buffers, strides, offsets);
    dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//サブセット毎の描画
void InstanceModelShader::SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    //コンスタントバッファの情報
    m_subsetconstants->data.color = subset.material->color;
    m_subsetconstants->data.emissivecolor = subset.material->emissivecolor;
    m_subsetconstants->data.emissiveintensity = subset.material->emissiveintensity;
    m_subsetconstants->data.Metalness = subset.material->Metalness;
    m_subsetconstants->data.Roughness = subset.material->Roughness;
    m_subsetconstants->data.alpha = subset.material->alpha;
    m_subsetconstants->Activate(dc, (int)CB_INDEX::SUBSET, true, true, false, false, false, false);

    //オブジェクト毎に使いたい定数バッファ
    m_generalconstants->data.outlineColor = subset.material->outlineColor;
    m_generalconstants->data.outlineintensity = subset.material->outlineintensity;
    m_generalconstants->Activate(dc, (int)CB_INDEX::GENERAL, false, true, false, false, false, false);

    //シェーダーリソースビュー設定
    dc->PSSetShaderResources(0, std::size(subset.material->shaderResourceView), subset.material->shaderResourceView[0].GetAddressOf());

    //インスタンスを使う時専用のDraw
    dc->DrawIndexedInstanced(subset.indexCount, iModelTransforms.size(), subset.startIndex, 0, 0);
}
void InstanceModelShader::ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    dc->DrawIndexedInstanced(subset.indexCount, iModelTransforms.size() * 4, subset.startIndex, 0, 0);
}

//描画終了処理
void InstanceModelShader::End(ID3D11DeviceContext* dc)
{
    dc->IASetInputLayout(nullptr);

    //解放してあげる
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
}

//imgui
void InstanceModelShader::ImGui()
{
}

// インスタンシングのデータをバッファに格納
void InstanceModelShader::CreateBuffer()
{
    Graphics& Graphics = Graphics::Instance();

    float instanceCount = MAX_STATIC_BATCH;
    // インスタンシングのデータをバッファに格納
    CD3D11_BUFFER_DESC bufferDesc(sizeof(DirectX::XMFLOAT4X4) * instanceCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);
    HRESULT hr = Graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_instancedata.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        // エラー処理
        throw std::runtime_error("Failed to create instance buffer");
    }
}

void InstanceModelShader::RemoveInstance(std::weak_ptr<TransformCom> transform)
{
    std::vector<std::weak_ptr<TransformCom>>::iterator it = std::find_if(iModelTransforms.begin(), iModelTransforms.end(),
        [&](const std::weak_ptr<TransformCom>& ptr1) {
            return ptr1.lock() == transform.lock();
        });

    assert(it != iModelTransforms.end());

    iModelTransforms.erase(it);
}

//位置、姿勢、大きさをバッファに格納
void InstanceModelShader::ReplaceBufferContents()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    HRESULT hr = dc->Map(m_instancedata.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    DirectX::XMFLOAT4X4* data = reinterpret_cast<DirectX::XMFLOAT4X4*>(mapped_subresource.pData);

    assert(iModelTransforms.size() <= MAX_STATIC_BATCH);
    for (int i = 0; i < iModelTransforms.size(); ++i)
    {
        data[i] = iModelTransforms[i].lock()->GetWorldTransform();
    }
    dc->Unmap(m_instancedata.Get(), 0);
}