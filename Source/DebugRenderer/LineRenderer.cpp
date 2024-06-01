#include <stdio.h>
#include <memory>
#include "System/Misc.h"
#include "LineRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"

LineRenderer::LineRenderer(ID3D11Device* device, UINT vertexCount)
    : capacity(vertexCount)
{
    // ���_�V�F�[�_�[
    // ���̓��C�A�E�g
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "Shader\\LineVS.cso", vertexShader.GetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

    // �s�N�Z���V�F�[�_�[
    create_ps_from_cso(device, "Shader\\DebugPS.cso", pixelShader.GetAddressOf());

    // �萔�o�b�t�@
    cb = std::make_unique<constant_buffer<ConstantBuffer>>(device);

    // ���_�o�b�t�@
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

// �`��J�n
void LineRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // �V�F�[�_�[�ݒ�
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
    context->IASetInputLayout(inputLayout.Get());

    // �����_�[�X�e�[�g�ݒ�
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    context->OMSetBlendState(Graphics::Instance().GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    context->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 0);
    context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // �v���~�e�B�u�ݒ�
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    // �萔�o�b�t�@�X�V
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX VP = V * P;
    DirectX::XMStoreFloat4x4(&cb->data.wvp, VP);
    cb->activate(context, 0, true, false, false, false, false, false);

    // �`��
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

// ���_�ǉ�
void LineRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
    Vertex v;
    v.position = position;
    v.color = color;
    vertices.emplace_back(v);
}