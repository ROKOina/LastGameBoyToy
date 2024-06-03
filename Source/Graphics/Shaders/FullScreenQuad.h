#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <stdint.h>

//フルスクリーンクアッド
class FullScreenQuad
{
public:
    FullScreenQuad(ID3D11Device* device);
    virtual ~FullScreenQuad() = default;

public:
    //元々のピクセルシェーダを参照してキャッシュする関数
    void Blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);

    //skymap
    void SkyMap();

    //スカイボックスのシェーダーリソース
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_skymap;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_embeddedvertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_embeddedpixelshader;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_depthstencilview;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_skymappixelshader;
};