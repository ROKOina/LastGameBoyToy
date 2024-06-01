#pragma once

#include "Shader.h"
#include "constant_buffer.h"
#include "Graphics.h"
#include "Model/Model.h"

#define MAX_BONES 256

enum MODELSHADER
{
    Defalt,
    Unity,
    SHADERMAX
};

class ModelShader
{
public:
    ModelShader(int shader);
    ~ModelShader() {};

    //ï`âÊèàóù
    void Render(ID3D11DeviceContext* dc, Model* model);

public:

    struct object_constants
    {
        DirectX::XMFLOAT4X4 BoneTransforms[MAX_BONES] = {};
    };
    std::unique_ptr<constant_buffer<object_constants>> ObjectConstants;

    struct subset_constants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3   emissivecolor = { 1.0f,1.0f,1.0f };
        float               emissiveintensity = 0;
        float               Metalness = 0;
        float               Roughness = 0;
        DirectX::XMFLOAT2 dummy = {};
    };
    std::unique_ptr<constant_buffer<subset_constants>> SubsetConstants;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>    VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>     PixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>     InputLayout;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skybox;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseiem;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularpmrem;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lutggx;
};
