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

    //インスタンス取得
    static FullScreenQuad& Instance();

public:
    //元々のピクセルシェーダを参照してキャッシュする関数
    void blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);

    //skymap
    void SkyMap();

    //スカイマップのシェーダーリソースビュー
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& sky_map() { return skymap; }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> skymappixelshader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;
};