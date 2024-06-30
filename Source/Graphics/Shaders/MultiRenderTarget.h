#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "Misc.h"

class MultiRenderTarget
{
public:
  MultiRenderTarget(ID3D11Device* device, UINT width, UINT height, UINT buffer_count);

  virtual ~MultiRenderTarget() {}

  void clear(ID3D11DeviceContext* immediate_context, float r = 0, float g = 0, float b = 0, float a = 0, float depth = 1);

  void activate(ID3D11DeviceContext* immediate_context);

  void LiberationRenderTarget(ID3D11DeviceContext* immediate_context);
  void LiberationDepthStencil(ID3D11DeviceContext* immediate_context);

  void ReleaseCache(ID3D11DeviceContext* immediate_context);

  void DrawImGui();

  int BufferCount() { return bufferCount; }
  ID3D11ShaderResourceView** GetShaderResources() { return shaderResourceViews[0].GetAddressOf(); }
  ID3D11ShaderResourceView** GetDepthStencilSRV() { return depthStencilSRV.GetAddressOf(); }

private:
  void CreateRTV(ID3D11Device* device);
  void CreateDSV(ID3D11Device* device);

private:
  static const UINT m_maxbuffercount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
  const UINT bufferCount;
  D3D11_VIEWPORT viewport;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViews[m_maxbuffercount];
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[m_maxbuffercount];
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthStencilSRV;

  UINT viewPortCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
  D3D11_VIEWPORT cachedViewPorts[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
  ID3D11RenderTargetView* cachedRenderTargetViews[m_maxbuffercount] = {};
  ID3D11DepthStencilView* cachedDepthStenciView = {};
};