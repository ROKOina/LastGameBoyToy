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

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName()const override { return "BulletHole"; }

private:

    //特殊エフェクトのコンスタントバッファ
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