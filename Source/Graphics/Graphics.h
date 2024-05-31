#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Shaders/Shader.h"
#include "Graphics/DebugRenderer/DebugRenderer.h"
#include "Graphics/DebugRenderer/LineRenderer.h"
#include "Graphics\Dx11StateLib.h"


#include "ThreadPool/ThreadPool.h"

#include <mutex>

#define DEBUG_GUI_ true

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


class CameraCom;
class ModelShader;

// グラフィックス
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// インスタンス取得
	static Graphics& Instance() { return *instance_; }

	// デバイス取得
	ID3D11Device* GetDevice() const { return device_.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext_.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swapchain_.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView_.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView_.Get(); }

	//シェーダー取得
	ModelShader* GetModelShader(int number)const { return modelshaders[number].get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth_; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight_; }

	// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debugRenderer_.get(); }

	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer_.get(); }

	//描画周り設定呼び出し
	const std::unique_ptr<Dx11StateLib>& GetDx11State() { return dx11State_; }

	// 描画ターゲットの退避
	void CacheRenderTargets();

	// 描画ターゲットを戻す
	void RestoreRenderTargets();


	// ブレンドステートの取得
	ID3D11BlendState* GetBlendState(BLENDSTATE index) { return blendStates[static_cast<int>(index)].Get(); }

	// ラスタライザの取得
	ID3D11RasterizerState* GetRasterizerState(RASTERIZERSTATE index) { return rasterizerStates[static_cast<int>(index)].Get(); }

	// 深度ステート の取得
	ID3D11DepthStencilState* GetDepthStencilState(DEPTHSTATE index) { return depthStencilStates[static_cast<int>(index)].Get(); }

	//サンプラーステートの設定
	void SetSamplerState();


	//ワールド座標からスクリーン座標にする
	DirectX::XMFLOAT3 WorldToScreenPos(DirectX::XMFLOAT3 worldPos, std::shared_ptr<CameraCom> camera);

	//FPS(セットはフレームワークでしか使わない予定)
	void SetFPSFramework(float fps) { this->fps_ = fps; }
	float GetFPS() { return fps_; }

	//ミューテックス取得
	std::mutex& GetMutex() { return mutex_; }

	//デバッグ表示するか
	bool IsDebugGUI() { return isDebugGui_; }

	//スレッドプール取得
	ThreadPool* GetThreadPool() { return threadPool_.get(); }

	//ハンドルゲット
	HWND GetHwnd() { return hWnd_; }


private:
	static Graphics* instance_;

	Microsoft::WRL::ComPtr<ID3D11Device>			device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext_;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

	std::unique_ptr<DebugRenderer>					debugRenderer_;
	std::unique_ptr<LineRenderer>					lineRenderer_;

	//描画周り一括初期化
	std::unique_ptr<Dx11StateLib> dx11State_;

	//描画ターゲット避難用
	UINT			cachedViewportCount_{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView_;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates[static_cast<int>(BLENDSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates[static_cast<int>(RASTERIZERSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DEPTHSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[static_cast<int>(SAMPLEMODE::MAX)];

	std::unique_ptr<ModelShader>                    modelshaders[2];


private:
	float	screenWidth_;
	float	screenHeight_;

	float fps_;
	std::mutex	mutex_;

	//デバッグ表示
	bool isDebugGui_ = DEBUG_GUI_;

	//スレッドプール
	std::unique_ptr<ThreadPool> threadPool_;

	const HWND				hWnd_;

	//世界の速度
public:
	float GetWorldSpeed() { return worldSpeed_; }
	void SetWorldSpeed(float speed) { worldSpeed_ = speed; }

private:
	float worldSpeed_ = 1.0f;
};

