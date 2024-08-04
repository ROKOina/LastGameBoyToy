#include "FrameBuffer.h"
#include "Graphics/Graphics.h"
#include "Misc.h"
#include "Shader.h"

//コンストラクタ
FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format/*format of render target*/, bool use_depth, bool use_stencil)
{
    HRESULT hr{ S_OK };

    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = width;
    texture2d_desc.Height = height;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = format; // DXGI_FORMAT_R8G8B8A8_UNORM
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
    hr = device->CreateTexture2D(&texture2d_desc, 0, m_rendertargetbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
    render_target_view_desc.Format = texture2d_desc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = device->CreateRenderTargetView(m_rendertargetbuffer.Get(), &render_target_view_desc, m_rendertargetview.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(m_rendertargetbuffer.Get(), &shader_resource_view_desc, m_shaderresourceviews[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    if (use_depth)
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
        texture2d_desc.Format = use_stencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
        texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
        depth_stencil_view_desc.Format = use_stencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Flags = 0;
        hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, m_depthstencilview.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        shader_resource_view_desc.Format = use_stencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
        shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, m_shaderresourceviews[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
}

//クリアして書き込めるようにする
void FrameBuffer::Clear(ID3D11DeviceContext* immediate_context, float r, float g, float b, float a, float depth)
{
    float color[4]{ r, g, b, a };

    immediate_context->ClearRenderTargetView(m_rendertargetview.Get(), color);

    if (m_depthstencilview)
    {
        immediate_context->ClearDepthStencilView(m_depthstencilview.Get(), D3D11_CLEAR_DEPTH, depth, 0);
    }
}

//viewportの設定
void FrameBuffer::Activate(ID3D11DeviceContext* immediate_context)
{
    // キャッシュされたビューポートとレンダーターゲットを取得
    m_viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    immediate_context->RSGetViewports(&m_viewport_count, m_cached_viewports);
    immediate_context->OMGetRenderTargets(1, m_cachedrendertargetview.ReleaseAndGetAddressOf(), m_cacheddepthstencilview.ReleaseAndGetAddressOf());

    // シェーダーリソースをアンバインドする
    ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

    // ビューポートとレンダーターゲットを設定
    immediate_context->RSSetViewports(1, &m_viewport);
    immediate_context->OMSetRenderTargets(1, m_rendertargetview.GetAddressOf(), m_depthstencilview.Get());
}

//保存
void FrameBuffer::Deactivate(ID3D11DeviceContext* immediate_context)
{
    // シェーダーリソースをアンバインドする
    ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
    immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

    // キャッシュされたビューポートとレンダーターゲットを復元
    immediate_context->RSSetViewports(m_viewport_count, m_cached_viewports);
    immediate_context->OMSetRenderTargets(1, m_cachedrendertargetview.GetAddressOf(), m_cacheddepthstencilview.Get());
}