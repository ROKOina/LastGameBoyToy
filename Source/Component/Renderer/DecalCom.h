#pragma once

#include "Component\System\Component.h"
#include "Graphics/ConstantBuffer.h"

class Decal :public Component
{
public:

    Decal(const char* filename);
    ~Decal() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override {};

    //�`��
    void Render();

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Decal"; }

    //�����֐�
    void Add(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, float scale)
    {
        spots.push_back({ position, normal, scale });
    }

private:

    //�������
    struct Spot
    {
        DirectX::XMFLOAT3 position = { 0, 0, 0 };
        DirectX::XMFLOAT3 normal = { 1, 1, 1 };
        float scale = 1;
    };
    std::vector<Spot> spots;

    //�萔�o�b�t�@
    struct DecalConstantBuffer
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4X4 decalinverseprojection;
    };
    std::unique_ptr<ConstantBuffer<DecalConstantBuffer>>DCB;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>decalmap;
    Microsoft::WRL::ComPtr<ID3D11Buffer>vertexbuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>indexbuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>pixelshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>inpulayout;
};