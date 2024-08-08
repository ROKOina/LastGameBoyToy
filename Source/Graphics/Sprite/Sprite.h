#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Components/System/Component.h"

// �X�v���C�g
class Sprite :public Component
{
public:

    Sprite(const char* filename);
    ~Sprite() {}

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override {};

    //�`��
    void Render();

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Sprite"; }

private:

    //�V���A���C�Y
    void Serialize();

    //�f�V���A���C�Y
    void Desirialize(const char* filename);

    //�ǂݍ���
    void LoadDesirialize();

public:

    //�ۑ�����p�����[�^
    struct SaveParameterCPU
    {
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT2 position = {};
        DirectX::XMFLOAT2 scale = {};
        float angle = {};
        std::string	filename;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc;

private:

    //���_�\����
    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT2	texcoord;
    };

private:

    Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
    D3D11_TEXTURE2D_DESC texture2ddesc_ = {};
};