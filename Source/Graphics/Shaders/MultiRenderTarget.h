#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "Misc.h"

class MultiRenderTarget
{
public:
    MultiRenderTarget(ID3D11Device* device, UINT width, UINT height, UINT buffer_count) : m_buffercount(buffer_count)
    {
        _ASSERT_EXPR(buffer_count <= m_maxbuffercount, L"The maximum number of buffers has been exceeded.");

        HRESULT hr = S_OK;

        for (UINT buffer_index = 0; buffer_index < buffer_count; ++buffer_index)
        {
            Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;

            D3D11_TEXTURE2D_DESC texture2d_desc = {};
            texture2d_desc.Width = width;
            texture2d_desc.Height = height;
            texture2d_desc.MipLevels = 1;
            texture2d_desc.ArraySize = 1;
            texture2d_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            texture2d_desc.SampleDesc.Count = 1;
            texture2d_desc.SampleDesc.Quality = 0;
            texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
            texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            texture2d_desc.CPUAccessFlags = 0;
            texture2d_desc.MiscFlags = 0;
            hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

            D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
            render_target_view_desc.Format = texture2d_desc.Format;
            render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc, &m_rendertargetviews[buffer_index]);
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

            D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
            shader_resource_view_desc.Format = texture2d_desc.Format;
            shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shader_resource_view_desc.Texture2D.MipLevels = 1;
            hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc, &m_rendertargetshaderresourceviews[buffer_index]);
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
        D3D11_TEXTURE2D_DESC texture2d_desc = {};
        texture2d_desc.Width = width;
        texture2d_desc.Height = height;
        texture2d_desc.MipLevels = 1;
        texture2d_desc.ArraySize = 1;
        texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        texture2d_desc.SampleDesc.Count = 1;
        texture2d_desc.SampleDesc.Quality = 0;
        texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
        texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        texture2d_desc.CPUAccessFlags = 0;
        texture2d_desc.MiscFlags = 0;
        hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
        depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Flags = 0;
        hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, &m_depthstencilview);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
        shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
        shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shader_resource_view_desc.Texture2D.MipLevels = 1;
        hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, &m_depthstencilshaderresourceview);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        for (UINT buffer_index = 0; buffer_index < buffer_count; ++buffer_index)
        {
            viewports[buffer_index].Width = static_cast<float>(width);
            viewports[buffer_index].Height = static_cast<float>(height);
            viewports[buffer_index].MinDepth = 0.0f;
            viewports[buffer_index].MaxDepth = 1.0f;
            viewports[buffer_index].TopLeftX = 0.0f;
            viewports[buffer_index].TopLeftY = 0.0f;
        }
    }
    virtual ~MultiRenderTarget()
    {
        for (UINT buffer_index = 0; buffer_index < m_buffercount; ++buffer_index)
        {
            if (m_rendertargetviews[buffer_index])
            {
                m_rendertargetviews[buffer_index]->Release();
            }
            if (m_rendertargetshaderresourceviews[buffer_index])
            {
                m_rendertargetshaderresourceviews[buffer_index]->Release();
            }
        }
        if (m_depthstencilview)
        {
            m_depthstencilview->Release();
        }
        if (m_depthstencilshaderresourceview)
        {
            m_depthstencilshaderresourceview->Release();
        }
    }

    const UINT m_buffercount;
    static const UINT m_maxbuffercount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    ID3D11RenderTargetView* m_rendertargetviews[m_maxbuffercount];
    ID3D11ShaderResourceView* m_rendertargetshaderresourceviews[m_maxbuffercount];
    ID3D11DepthStencilView* m_depthstencilview;
    ID3D11ShaderResourceView* m_depthstencilshaderresourceview;
    D3D11_VIEWPORT viewports[m_maxbuffercount];

    void clear(ID3D11DeviceContext* immediate_context, float r = 0, float g = 0, float b = 0, float a = 0, float depth = 1)
    {
        float color[4]{ r, g, b, a };
        for (UINT buffer_index = 0; buffer_index < m_buffercount; ++buffer_index)
        {
            immediate_context->ClearRenderTargetView(m_rendertargetviews[buffer_index], color);
        }
        immediate_context->ClearDepthStencilView(m_depthstencilview, D3D11_CLEAR_DEPTH, depth, 0);
    }

    void activate(ID3D11DeviceContext* immediate_context)
    {
        // キャッシュされたビューポートとレンダーターゲットを取得
        m_viewportcount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        immediate_context->RSGetViewports(&m_viewportcount, m_cachedviewports);
        immediate_context->OMGetRenderTargets(m_buffercount, m_cachedrendertargetviews, &m_cacheddepthstencilview);

        // シェーダーリソースをアンバインドする
        ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
        immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

        // ビューポートとレンダーターゲットを設定
        immediate_context->RSSetViewports(m_buffercount, viewports);
        immediate_context->OMSetRenderTargets(m_buffercount, m_rendertargetviews, m_depthstencilview);
    }

    void deactivate(ID3D11DeviceContext* immediate_context)
    {
        immediate_context->RSSetViewports(m_viewportcount, m_cachedviewports);
        immediate_context->OMSetRenderTargets(m_buffercount, m_cachedrendertargetviews, m_cacheddepthstencilview);

        // シェーダーリソースをアンバインドする
        ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
        immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

        for (UINT buffer_index = 0; buffer_index < m_buffercount; ++buffer_index)
        {
            if (m_cachedrendertargetviews[buffer_index])
            {
                m_cachedrendertargetviews[buffer_index]->Release();
            }
        }
        if (m_cacheddepthstencilview)
        {
            m_cacheddepthstencilview->Release();
        }
    }

private:
    UINT m_viewportcount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT m_cachedviewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
    ID3D11RenderTargetView* m_cachedrendertargetviews[m_maxbuffercount] = {};
    ID3D11DepthStencilView* m_cacheddepthstencilview = {};
};