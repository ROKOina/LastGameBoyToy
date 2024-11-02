#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <stdint.h>

//フレームバッファー
class FrameBuffer
{
public:
    FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format/*format of render target*/, bool use_depth, bool use_stencil = false);
    virtual ~FrameBuffer() = default;

    void Clear(ID3D11DeviceContext* immediate_context, float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
    void Activate(ID3D11DeviceContext* immediate_context);
    void Deactivate(ID3D11DeviceContext* immediate_context);

    //シェーダーリソース
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderresourceviews[2];  //0がシーン情報、1が深度情報

    //depthstencilview
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthstencilview;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_cachedrendertargetview;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_cacheddepthstencilview;

private:
    UINT m_viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT m_cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rendertargetview;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_rendertargetbuffer;
    D3D11_VIEWPORT m_viewport;
};