#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Shader.h"
#include "DebugRenderer/DebugRenderer.h"
#include "DebugRenderer/LineRenderer.h"

//ブレンドステート
enum class BLENDSTATE
{
    NONE,
    ALPHA,
    ADD,
    SUBTRACT,
    REPLACE,
    MULTIPLY,
    LIGHTEN,
    DARKEN,
    SCREEN,

    MAX
};

// ラスタライザ
enum class RASTERIZERSTATE
{
    SOLID_CULL_NONE,
    SOLID_CULL_BACK,
    SOLID_CULL_FRONT,
    WIREFRAME,

    MAX
};

// 深度ステート
enum class DEPTHSTATE
{
    NONE, // 深度テストしない
    ZT_ON_ZW_ON,    // テスト有、書き込み有
    ZT_ON_ZW_OFF,   // テスト有、書き込み無
    ZT_OFF_ZW_ON,   // テスト無、書き込み有
    ZT_OFF_ZW_OFF,  // テスト無、書き込み無

    // 以降ステンシルマスク用
    MASK,
    APPLY_MASK,
    EXCLUSIVE,

    MAX
};

// サンプラーステート
enum SAMPLEMODE
{
    WRAP_POINT,
    WRAP_LINEAR,
    WRAP_ANISOTROPIC,

    BORDER_POINT,
    BORDER_LINEAR,
    BORDER_ANISOTROPIC,

    SHADOW,

    MAX
};

class ModelShader;

// グラフィックス
class Graphics
{
public:
    Graphics(HWND hWnd);
    ~Graphics();

    // インスタンス取得
    static Graphics& Instance() { return *instance; }

    // デバイス取得
    ID3D11Device* GetDevice() const { return device.Get(); }

    // デバイスコンテキスト取得
    ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

    // スワップチェーン取得
    IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

    // レンダーターゲットビュー取得
    ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

    // デプスステンシルビュー取得
    ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

    // ブレンドステートの取得
    ID3D11BlendState* GetBlendState(BLENDSTATE index) { return blendStates[static_cast<int>(index)].Get(); }

    // ラスタライザの取得
    ID3D11RasterizerState* GetRasterizerState(RASTERIZERSTATE index) { return rasterizerStates[static_cast<int>(index)].Get(); }

    // 深度ステート の取得
    ID3D11DepthStencilState* GetDepthStencilState(DEPTHSTATE index) { return depthStencilStates[static_cast<int>(index)].Get(); }

    //サンプラーステートの設定
    void SetSamplerState();

    //シェーダー取得
    ModelShader* GetModelShader(int number)const { return modelshaders[number].get(); }

    // スクリーン幅取得
    float GetScreenWidth() const { return screenWidth; }

    // スクリーン高さ取得
    float GetScreenHeight() const { return screenHeight; }

    // デバッグレンダラ取得
    DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

    // ラインレンダラ取得
    LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

private:
    static Graphics* instance;
    Microsoft::WRL::ComPtr<ID3D11Device>			device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates[static_cast<int>(BLENDSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates[static_cast<int>(RASTERIZERSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DEPTHSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[static_cast<int>(SAMPLEMODE::MAX)];

    std::unique_ptr<ModelShader>                    modelshaders[2];
    std::unique_ptr<DebugRenderer>					debugRenderer;
    std::unique_ptr<LineRenderer>					lineRenderer;

    float	screenWidth;
    float	screenHeight;
};
