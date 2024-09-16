#pragma once

#include "System\Component.h"
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/Shaders/ConstantBuffer.h"

class BulletHole :public Component
{
public:
    BulletHole(const char* filename);
    ~BulletHole() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��
    void Render();

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName()const override { return "BulletHole"; }

private:

    //����G�t�F�N�g�̃R���X�^���g�o�b�t�@
    struct SPECIALFFECT
    {
        DirectX::XMFLOAT4X4 projectionmappingtransform = {};
    };
    std::unique_ptr<ConstantBuffer<SPECIALFFECT>>m_special;

private:

    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelshaders;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexshaders;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderresourceview;

    DirectX::XMFLOAT3 eye = { 0, 3.0f, 0 };
    DirectX::XMFLOAT3 focus = { 4, 0, 0 };
    float fovy = 10.0f;
};