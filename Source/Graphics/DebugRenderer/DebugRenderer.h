#pragma once

#include <vector>
#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/ConstantBuffer.h"
#include "Math\QuaternionStruct.h"
#include "PxPhysicsAPI.h"

class DebugRenderer
{
public:
    DebugRenderer(ID3D11Device* device);
    ~DebugRenderer() {}

public:
    // ï`âÊé¿çs
    void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // ãÖï`âÊ
    void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

    // î†ï`âÊ
    void DrawBox(const DirectX::XMFLOAT3& center, DirectX::XMFLOAT3	scale, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& rotato = { 0,0,0,1 });

    // â~íåï`âÊ
    void DrawCylinder(const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2, float radius, float height, const DirectX::XMFLOAT4& color);

private:
    // ãÖÉÅÉbÉVÉÖçÏê¨
    void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

    // î†ÉÅÉbÉVÉÖçÏê¨
    void CreateBoxMesh(ID3D11Device* device, DirectX::XMFLOAT3 scale);

    // â~íåÉÅÉbÉVÉÖçÏê¨
    void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

private:
    struct CbMesh
    {
        DirectX::XMFLOAT4X4	wvp;
        DirectX::XMFLOAT4	color;
    };
    std::unique_ptr<ConstantBuffer<CbMesh>>cbmesh;

    struct Sphere
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	center;
        float				radius;
    };

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
    };
    struct Box
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	center;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotato;
    };

    struct Cylinder
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	position1;
        DirectX::XMFLOAT3	position2;
        float				radius;
        float				height;
    };

    struct Mesh
    {
        DirectX::XMFLOAT4	color = { 0,0,0,1 };
        DirectX::XMFLOAT3	position = {};
        DirectX::XMFLOAT4	rotation = {};
        DirectX::XMFLOAT3   scale = {};

        UINT points_count;
        UINT point_stride;
        void* points_data = nullptr;

        UINT triangles_count;
        UINT triangles_stride;
        void* triangles_data = nullptr;

        Microsoft::WRL::ComPtr<ID3D11Buffer> meshVertexBuffer_;
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>			boxVertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>			boxIndexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout_;

    std::vector<Sphere>		spheres_;
    std::vector<Box>		boxes_;
    std::vector<Cylinder>	cylinders_;
    std::vector<Mesh>       meshs_;

    UINT	sphereVertexCount_ = 0;
    UINT	boxVertexCount_ = 0;
    UINT	boxIndicesCount_ = 0;
    UINT	cylinderVertexCount_ = 0;
};
