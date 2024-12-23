#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/ConstantBuffer.h"

class LineRenderer
{
public:
    LineRenderer(ID3D11Device* device, UINT vertexCount);
    ~LineRenderer() {}

public:
    // 描画実行
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // 頂点追加
    void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color);

private:
    struct CB
    {
        DirectX::XMFLOAT4X4	wvp;
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
    };
    std::unique_ptr<ConstantBuffer<CB>>cb;

    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        float dummy = 1.0f;
        DirectX::XMFLOAT4	color;
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;

    std::vector<Vertex>			vertices_;
    UINT						capacity_ = 0;
};
