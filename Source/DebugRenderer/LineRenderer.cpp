#include <stdio.h>
#include <memory>
#include "System/Misc.h"
#include "LineRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"

LineRenderer::LineRenderer(ID3D11Device* device, UINT vertexCount)
    : capacity(vertexCount)
{
    // 頂点シェーダー
    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "Shader\\LineVS.cso", vertexShader.GetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

    // ピクセルシェーダー
    create_ps_from_cso(device, "Shader\\DebugPS.cso", pixelShader.GetAddressOf());

    // 定数バッファ
    cb = std::make_unique<constant_buffer<ConstantBuffer>>(device);

    // 頂点バッファ
    {
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeof(Vertex) * vertexCount;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}

// 描画開始
void LineRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // シェーダー設定
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
    context->IASetInputLayout(inputLayout.Get());

    // レンダーステート設定
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    context->OMSetBlendState(Graphics::Instance().GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    context->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 0);
    context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // プリミティブ設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    // 定数バッファ更新
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX VP = V * P;
    DirectX::XMStoreFloat4x4(&cb->data.wvp, VP);
    cb->activate(context, 0, true, false, false, false, false, false);

    // 描画
    UINT totalVertexCount = static_cast<UINT>(vertices.size());
    UINT start = 0;
    UINT count = (totalVertexCount < capacity) ? totalVertexCount : capacity;

    while (start < totalVertexCount)
    {
        D3D11_MAPPED_SUBRESOURCE mappedVB;
        HRESULT hr = context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        memcpy(mappedVB.pData, &vertices[start], sizeof(Vertex) * count);

        context->Unmap(vertexBuffer.Get(), 0);

        context->Draw(count, 0);

        start += count;
        if ((start + count) > totalVertexCount)
        {
            count = totalVertexCount - start;
        }
    }
    vertices.clear();
}

// 頂点追加
void LineRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
    Vertex v;
    v.position = position;
    v.color = color;
    vertices.emplace_back(v);
}