#include "InstanceModelShader.h"

//コンストラクタ
InstanceModelShader::InstanceModelShader(SHADER_ID_MODEL shader)
{
    Graphics& Graphics = Graphics::Instance();

    //ファイル名
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;
    const char* GSPath = nullptr;

    //選択されたのが指定される
    switch (shader)
    {
    case SHADER_ID_MODEL::DEFAULT:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DefaltPS.cso" };
        break;
    case SHADER_ID_MODEL::DEFERRED:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DeferredSetupPS.cso" };
        break;
    case SHADER_ID_MODEL::BLACK:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\BlackPS.cso" };
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
    case SHADER_ID_MODEL::SHADOW:
        VSPath = { "Shader\\InstanceShadowVS.cso" };
        GSPath = { "Shader\\ShadowGS.cso" };
        break;
    case SHADER_ID_MODEL::SILHOUETTE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\SilhouettePS.cso" };
        break;

    default:
        assert(!"引数のShaderに想定されていない値が入れられている");
    }

    // 頂点シェーダー
    //入力レイアウト
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        // 入力要素の設定
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "I_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "I_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "I_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));

    // ピクセルシェーダーとジオメトリシェーダー
    if (shader != SHADER_ID_MODEL::SHADOW)
    {
        CreatePsFromCso(Graphics.GetDevice(), PSPath, m_pixelshader.GetAddressOf());
    }
    else if (shader == SHADER_ID_MODEL::SHADOW)
    {
        CreateGsFromCso(Graphics.GetDevice(), GSPath, m_geometryshader.GetAddressOf());
    }

    // 定数バッファ
    {
        // オブジェクト用のコンスタントバッファ,サブセット用
        m_objectconstants = std::make_unique<ConstantBuffer<objectconstants>>(Graphics.GetDevice());
        m_subsetconstants = std::make_unique<ConstantBuffer<subsetconstants>>(Graphics.GetDevice());
        m_generalconstants = std::make_unique<ConstantBuffer<m_general>>(Graphics.GetDevice());
    }

    // インスタンシングのデータをバッファに格納
    CD3D11_BUFFER_DESC bufferDesc(sizeof(Instance) * m_instancecount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    bufferDesc.StructureByteStride = sizeof(Instance);
    HRESULT hr = Graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_instancedata.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        // エラー処理
        throw std::runtime_error("Failed to create instance buffer");
    }
    m_cpuinstancedata.reset(new Instance[m_instancecount]);
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
    ReplaceBufferContents(m_instancedata.Get(), sizeof(Instance) * m_instancecount, m_cpuinstancedata.get());
}
void InstanceModelShader::ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //シェーダーのセット
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->GSSetShader(m_geometryshader.Get(), nullptr, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //レンダーステートの設定（ループの外で1回だけ設定）
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));
}

void InstanceModelShader::SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
    //定数バッファにメッシュのtransformを送る
    DirectX::XMStoreFloat4x4(&m_objectconstants->data.transform, DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform));

    //コンスタントバッファの更新
    m_objectconstants->Activate(dc, (int)CB_INDEX::OBJECT, true, true, false, false, false, false);

    UINT strides[] = { sizeof(ModelResource::Vertex), sizeof(Instance) };
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
    m_subsetconstants->Activate(dc, (int)CB_INDEX::SUBSET, true, true, false, false, false, false);

    //オブジェクト毎に使いたい定数バッファ
    m_generalconstants->data.outlineColor = subset.material->outlineColor;
    m_generalconstants->data.outlineintensity = subset.material->outlineintensity;
    m_generalconstants->data.statictype = subset.material->statictype;
    m_generalconstants->Activate(dc, (int)CB_INDEX::GENERAL, false, true, false, false, false, false);

    //シェーダーリソースビュー設定
    dc->PSSetShaderResources(0, std::size(subset.material->shaderResourceView), subset.material->shaderResourceView[0].GetAddressOf());

    //インスタンスを使う時専用のDraw
    dc->DrawIndexedInstanced(subset.indexCount, m_instancecount, subset.startIndex, 0, 0);
}
void InstanceModelShader::ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    dc->DrawIndexedInstanced(subset.indexCount, 4, subset.startIndex, 0, 0);
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

//位置、姿勢、大きさをバッファに格納
void InstanceModelShader::ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    Graphics& graphics = Graphics::Instance();
    graphics.GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, data, bufferSize);
    graphics.GetDeviceContext()->Unmap(buffer, 0);
}