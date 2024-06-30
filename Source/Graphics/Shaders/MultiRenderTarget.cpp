#include "MultiRenderTarget.h"
#include "ImGui.h"

MultiRenderTarget::MultiRenderTarget(ID3D11Device* device, UINT width, UINT height, UINT buffer_count) : bufferCount(buffer_count)
{
  _ASSERT_EXPR(buffer_count <= m_maxbuffercount, L"The maximum number of buffers has been exceeded.");

  // ビューポートの設定
  viewport.Width = static_cast<float>(width);
  viewport.Height = static_cast<float>(height);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  CreateRTV(device);

  CreateDSV(device);
}

void MultiRenderTarget::clear(ID3D11DeviceContext* immediate_context, float r, float g, float b, float a, float depth)
{
  float color[4]{ r, g, b, a };
  for (UINT buffer_index = 0; buffer_index < bufferCount; ++buffer_index)
  {
    immediate_context->ClearRenderTargetView(renderTargetViews[buffer_index].Get(), color);
  }
  immediate_context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void MultiRenderTarget::activate(ID3D11DeviceContext* immediate_context)
{
  // キャッシュされたビューポートとレンダーターゲットを取得
  viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
  immediate_context->RSGetViewports(&viewPortCount, cachedViewPorts);
  immediate_context->OMGetRenderTargets(bufferCount, cachedRenderTargetViews, &cachedDepthStenciView);

  // シェーダーリソースをアンバインドする
  ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
  immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

  // ビューポートとレンダーターゲットを設定
  immediate_context->RSSetViewports(1, &viewport);
  immediate_context->OMSetRenderTargets(bufferCount, renderTargetViews[0].GetAddressOf(), depthStencilView.Get());
}

void MultiRenderTarget::LiberationRenderTarget(ID3D11DeviceContext* immediate_context)
{
  immediate_context->RSSetViewports(viewPortCount, cachedViewPorts);
  immediate_context->OMSetRenderTargets(bufferCount, cachedRenderTargetViews, depthStencilView.Get());

  // シェーダーリソースをアンバインドする
  ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
  immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);
}

void MultiRenderTarget::LiberationDepthStencil(ID3D11DeviceContext* immediate_context)
{
  immediate_context->RSSetViewports(viewPortCount, cachedViewPorts);
  immediate_context->OMSetRenderTargets(bufferCount, cachedRenderTargetViews, cachedDepthStenciView);

  // シェーダーリソースをアンバインドする
  ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
  immediate_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);
}

void MultiRenderTarget::ReleaseCache(ID3D11DeviceContext* immediate_context)
{
  for (UINT buffer_index = 0; buffer_index < bufferCount; ++buffer_index)
  {
    if (cachedRenderTargetViews[buffer_index])
    {
      cachedRenderTargetViews[buffer_index]->Release();
    }
  }
  if (cachedDepthStenciView)
  {
    cachedDepthStenciView->Release();
  }
}

void MultiRenderTarget::DrawImGui()
{    
  //RenderTarget
  if (ImGui::TreeNode("rendertarget"))
  {
    ImGui::Text("Color");
    ImGui::Image(shaderResourceViews[0].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Text("Normal");
    ImGui::Image(shaderResourceViews[1].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Text("Position");
    ImGui::Image(shaderResourceViews[2].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Text("Roughness,Metallic,AO");
    ImGui::Image(shaderResourceViews[3].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Text("Emission");
    ImGui::Image(shaderResourceViews[4].Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Text("Depth");
    ImGui::Image(depthStencilSRV.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    ImGui::TreePop();
  }
}

void MultiRenderTarget::CreateRTV(ID3D11Device* device)
{
  // テクスチャフォーマット
  D3D11_TEXTURE2D_DESC texture2d_desc = {};
  texture2d_desc.Width = viewport.Width;
  texture2d_desc.Height = viewport.Height;
  texture2d_desc.MipLevels = 1;
  texture2d_desc.ArraySize = 1;
  texture2d_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  texture2d_desc.SampleDesc.Count = 1;
  texture2d_desc.SampleDesc.Quality = 0;
  texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
  texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  texture2d_desc.CPUAccessFlags = 0;
  texture2d_desc.MiscFlags = 0;

  // レンダーターゲットフォーマット
  D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
  render_target_view_desc.Format = texture2d_desc.Format;
  render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

  // SRVフォーマット
  D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
  shader_resource_view_desc.Format = texture2d_desc.Format;
  shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  shader_resource_view_desc.Texture2D.MipLevels = 1;

  HRESULT hr = S_OK;
  for (UINT buffer_index = 0; buffer_index < bufferCount; ++buffer_index)
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;

    hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc, renderTargetViews[buffer_index].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc, shaderResourceViews[buffer_index].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }
}

void MultiRenderTarget::CreateDSV(ID3D11Device* device)
{
  Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
  D3D11_TEXTURE2D_DESC texture2d_desc = {};
  texture2d_desc.Width = viewport.Width;
  texture2d_desc.Height = viewport.Height;
  texture2d_desc.MipLevels = 1;
  texture2d_desc.ArraySize = 1;
  texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
  texture2d_desc.SampleDesc.Count = 1;
  texture2d_desc.SampleDesc.Quality = 0;
  texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
  texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
  texture2d_desc.CPUAccessFlags = 0;
  texture2d_desc.MiscFlags = 0;
  HRESULT hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
  depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
  depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depth_stencil_view_desc.Flags = 0;
  hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depthStencilView.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

  D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
  shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
  shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  shader_resource_view_desc.Texture2D.MipLevels = 1;
  hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, depthStencilSRV.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
