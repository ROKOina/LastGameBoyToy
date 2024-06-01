#include <stdio.h>
#include <memory>
#include "System/Misc.h"
#include "DebugRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
    // 頂点シェーダー
    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "Shader\\DebugVS.cso", vertexShader.GetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

    // ピクセルシェーダー
    create_ps_from_cso(device, "Shader\\DebugPS.cso", pixelShader.GetAddressOf());

    // 定数バッファ
    cbmesh = std::make_unique<constant_buffer<CbMesh>>(device);

    // 球メッシュ作成
    CreateSphereMesh(device, 1.0f, 16, 16);
}

// 描画開始
void DebugRenderer::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    // シェーダー設定
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
    context->IASetInputLayout(inputLayout.Get());

    // 定数バッファ設定
    cbmesh->activate(context, 0, true, false, false, false, false, false);

    // レンダーステート設定
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    context->OMSetBlendState(Graphics::Instance().GetBlendState(BLENDSTATE::NONE), blendFactor, 0xFFFFFFFF);
    context->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 0);
    context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // ビュープロジェクション行列作成
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX VP = V * P;

    // プリミティブ設定
    UINT stride = sizeof(DirectX::XMFLOAT3);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // 球描画
    context->IASetVertexBuffers(0, 1, sphereVertexBuffer.GetAddressOf(), &stride, &offset);
    for (const Sphere& sphere : spheres)
    {
        // ワールドビュープロジェクション行列作成
        float scaling = sphere.radius * 2.0f;
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scaling, scaling, scaling);
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center.x, sphere.center.y, sphere.center.z);
        DirectX::XMMATRIX W = S * T;
        DirectX::XMMATRIX WVP = W * VP;

        // 定数バッファ更新
        cbmesh->data.color = sphere.color;
        DirectX::XMStoreFloat4x4(&cbmesh->data.wvp, WVP);

        context->Draw(sphereVertexCount, 0);
    }
    spheres.clear();
}

// 球描画
void DebugRenderer::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
    Sphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    sphere.color = color;
    spheres.emplace_back(sphere);
}

// 球メッシュ作成
void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks)
{
    sphereVertexCount = stacks * slices * 2 + slices * stacks * 2;
    std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount);

    float phiStep = DirectX::XM_PI / stacks;
    float thetaStep = DirectX::XM_2PI / slices;

    DirectX::XMFLOAT3* p = vertices.get();

    for (int i = 0; i < stacks; ++i)
    {
        float phi = i * phiStep;
        float y = radius * cosf(phi);
        float r = radius * sinf(phi);

        for (int j = 0; j < slices; ++j)
        {
            float theta = j * thetaStep;
            p->x = r * sinf(theta);
            p->y = y;
            p->z = r * cosf(theta);
            p++;

            theta += thetaStep;

            p->x = r * sinf(theta);
            p->y = y;
            p->z = r * cosf(theta);
            p++;
        }
    }

    thetaStep = DirectX::XM_2PI / stacks;
    for (int i = 0; i < slices; ++i)
    {
        DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
        for (int j = 0; j < stacks; ++j)
        {
            float theta = j * thetaStep;
            DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
            DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
            DirectX::XMStoreFloat3(p++, P1);

            int n = (j + 1) % stacks;
            theta += thetaStep;

            DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
            DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
            DirectX::XMStoreFloat3(p++, P2);
        }
    }

    // 頂点バッファ
    {
        D3D11_BUFFER_DESC desc = {};
        D3D11_SUBRESOURCE_DATA subresourceData = {};

        desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount);
        desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        subresourceData.pSysMem = vertices.get();
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;

        HRESULT hr = device->CreateBuffer(&desc, &subresourceData, sphereVertexBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }
}