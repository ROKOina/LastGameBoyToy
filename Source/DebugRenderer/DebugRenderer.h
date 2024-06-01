#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/constant_buffer.h"

//�f�o�b�O�v���~�e�B�u
class DebugRenderer
{
public:
    DebugRenderer(ID3D11Device* device);
    ~DebugRenderer() {}

public:
    // �`����s
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // ���`��
    void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

private:
    // �����b�V���쐬
    void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

private:
    struct CbMesh
    {
        DirectX::XMFLOAT4X4	wvp;
        DirectX::XMFLOAT4	color;
    };
    std::unique_ptr<constant_buffer<CbMesh>>cbmesh;

    struct Sphere
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	center;
        float				radius;
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

    std::vector<Sphere>		spheres;

    UINT	sphereVertexCount = 0;
};
