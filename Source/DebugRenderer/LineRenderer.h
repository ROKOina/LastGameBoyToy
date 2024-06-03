#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/constant_buffer.h"

//���C�������_��
class LineRenderer
{
public:
    LineRenderer(ID3D11Device* device, UINT vertexCount);
    ~LineRenderer() {}

public:
    // �`����s
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // ���_�ǉ�
    void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color);

private:
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4	wvp;
    };
    std::unique_ptr<constant_buffer<ConstantBuffer>>cb;

    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        DirectX::XMFLOAT4	color;
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

    std::vector<Vertex>			vertices;
    UINT						capacity = 0;
};