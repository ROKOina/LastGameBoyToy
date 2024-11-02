#include <stdio.h>
#include <memory>
#include "SystemStruct\Misc.h"
#include "LineRenderer.h"
#include "Graphics/Graphics.h"

LineRenderer::LineRenderer(ID3D11Device* device, UINT vertexCount)
    : capacity_(vertexCount)
{
    // ���_�V�F�[�_�[
    {
        // ���̓��C�A�E�g
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        CreateVsFromCso(device, "Shader\\LineVS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // �s�N�Z���V�F�[�_�[
    {
        CreatePsFromCso(device, "Shader\\LinePS.cso", pixelShader_.GetAddressOf());
    }

    // �萔�o�b�t�@
    {
        cb = std::make_unique<ConstantBuffer<CB>>(device);
    }

    // ���_�o�b�t�@
    {
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeof(Vertex) * vertexCount;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}

// �`��J�n
void LineRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // �V�F�[�_�[�ݒ�
    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);
    context->IASetInputLayout(inputLayout_.Get());

    // �����_�[�X�e�[�g�ݒ�
    context->OMSetBlendState(Graphics::Instance().GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 1);
    context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::WIREFRAME));

    // �v���~�e�B�u�ݒ�
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

    // �萔�o�b�t�@�X�V
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX VP = V * P;
    DirectX::XMStoreFloat4x4(&cb->data.wvp, VP);
    cb->Activate(context, 0, true, true, false, false, false, false);

    // �`��
    UINT totalVertexCount = static_cast<UINT>(vertices_.size());
    UINT start = 0;
    UINT count = (totalVertexCount < capacity_) ? totalVertexCount : capacity_;

    while (start < totalVertexCount)
    {
        D3D11_MAPPED_SUBRESOURCE mappedVB;
        HRESULT hr = context->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        memcpy(mappedVB.pData, &vertices_[start], sizeof(Vertex) * count);

        context->Unmap(vertexBuffer_.Get(), 0);

        context->Draw(count, 0);

        start += count;
        if ((start + count) > totalVertexCount)
        {
            count = totalVertexCount - start;
        }
    }
    vertices_.clear();
}

// ���_�ǉ�
void LineRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
{
    Vertex v;
    v.position = position;
    v.color = color;
    vertices_.emplace_back(v);
}