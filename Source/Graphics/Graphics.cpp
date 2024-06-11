#include "Misc.h"
#include "Graphics/Graphics.h"

#include "Shaders/3D/ModelShader.h"

#include "Components/CameraCom.h"

Graphics* Graphics::instance_ = nullptr;

extern "C"
{
    __declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 1;
}

// コンストラクタ
Graphics::Graphics(HWND hWnd)
    :hWnd_(hWnd)
{
    // インスタンス設定
    _ASSERT_EXPR(instance_ == nullptr, "already instantiated");
    instance_ = this;

    // 画面のサイズを取得する。
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT screenWidth = rc.right - rc.left;
    UINT screenHeight = rc.bottom - rc.top;

    this->screenWidth_ = static_cast<float>(screenWidth);
    this->screenHeight_ = static_cast<float>(screenHeight);

    HRESULT hr = S_OK;

    // デバイス＆スワップチェーンの生成
    {
        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        // スワップチェーンを作成するための設定オプション
        DXGI_SWAP_CHAIN_DESC swapchainDesc;
        {
            swapchainDesc.BufferDesc.Width = screenWidth;
            swapchainDesc.BufferDesc.Height = screenHeight;
            swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
            swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 1ピクセルあたりの各色(RGBA)を8bit(0～255)のテクスチャ(バックバッファ)を作成する。
            swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

            swapchainDesc.SampleDesc.Count = 1;
            swapchainDesc.SampleDesc.Quality = 0;
            swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapchainDesc.BufferCount = 1;		// バックバッファの数
            swapchainDesc.OutputWindow = hWnd;	// DirectXで描いた画を表示するウインドウ
            swapchainDesc.Windowed = TRUE;		// ウインドウモードか、フルスクリーンにするか。
            swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swapchainDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        }

        D3D_FEATURE_LEVEL featureLevel;

        // デバイス＆スワップチェーンの生成
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,						// どのビデオアダプタを使用するか？既定ならばnullptrで、IDXGIAdapterのアドレスを渡す。
            D3D_DRIVER_TYPE_HARDWARE,		// ドライバのタイプを渡す。D3D_DRIVER_TYPE_HARDWARE 以外は基本的にソフトウェア実装で、特別なことをする場合に用いる。
            nullptr,						// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドルを渡す。それ以外を指定している際には必ずnullptrを渡す。
            createDeviceFlags,				// 何らかのフラグを指定する。詳しくはD3D11_CREATE_DEVICE列挙型で検索。
            featureLevels,					// D3D_FEATURE_LEVEL列挙型の配列を与える。nullptrにすることでも上記featureと同等の内容の配列が使用される。
            ARRAYSIZE(featureLevels),		// featureLevels配列の要素数を渡す。
            D3D11_SDK_VERSION,				// SDKのバージョン。必ずこの値。
            &swapchainDesc,					// ここで設定した構造体に設定されているパラメータでSwapChainが作成される。
            swapchain_.GetAddressOf(),		// 作成が成功した場合に、SwapChainのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でSwapChainを操作する。
            device_.GetAddressOf(),			// 作成が成功した場合に、Deviceのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でDeviceを操作する。
            &featureLevel,					// 作成に成功したD3D_FEATURE_LEVELを格納するためのD3D_FEATURE_LEVEL列挙型変数のアドレスを設定する。
            immediateContext_.GetAddressOf()	// 作成が成功した場合に、Contextのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でContextを操作する。
        );
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // レンダーターゲットビューの生成
    {
        // スワップチェーンからバックバッファテクスチャを取得する。
        // ※スワップチェーンに内包されているバックバッファテクスチャは'色'を書き込むテクスチャ。
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hr = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        // バックバッファテクスチャへの書き込みの窓口となるレンダーターゲットビューを生成する。
        hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // 深度ステンシルビューの生成
    {
        // 深度ステンシル情報を書き込むためのテクスチャを作成する。
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        depthStencilBufferDesc.Width = screenWidth;
        depthStencilBufferDesc.Height = screenHeight;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 1ピクセルあたり、深度情報を24Bit / ステンシル情報を8bitのテクスチャを作成する。
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// 深度ステンシル用のテクスチャを作成する。
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.MiscFlags = 0;
        hr = device_->CreateTexture2D(&depthStencilBufferDesc, nullptr, depthStencilBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        // 深度ステンシルテクスチャへの書き込みに窓口になる深度ステンシルビューを作成する。
        hr = device_->CreateDepthStencilView(depthStencilBuffer_.Get(), nullptr, depthStencilView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // ビューポートの設定
    {
        // 画面のどの領域にDirectXで描いた画を表示するかの設定。
        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(screenWidth);
        viewport.Height = static_cast<float>(screenHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        immediateContext_->RSSetViewports(1, &viewport);
    }

    // ブレンドステート
    {
        D3D11_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        // BS_NONE
        {
            desc = {};
            desc.AlphaToCoverageEnable = false;
            desc.IndependentBlendEnable = false;
            desc.RenderTarget[0].BlendEnable = false;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::NONE)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // BS_ALPHA
        {
            desc = {};
            desc.AlphaToCoverageEnable = false;
            desc.IndependentBlendEnable = false;
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::ALPHA)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // BS_ADD
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::ADD)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // BS_SUBTRACT
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_SRC_ALPHA
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3D11_BLEND_ONE
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; //D3D11_BLEND_OP_SUBTRACT
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::SUBTRACT)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        //	BS_REPLACE
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::REPLACE)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        //	BS_MULTIPLY
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::MULTIPLY)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        //	BS_LIGHTEN
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::LIGHTEN)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        //	BS_DARKEN
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::DARKEN)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        //	BS_SCREEN
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTCOLOR
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::SCREEN)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // MULTIPLE_RENDER_TARGETS
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            desc.RenderTarget[1].BlendEnable = TRUE;
            desc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            desc.RenderTarget[2].BlendEnable = TRUE;
            desc.RenderTarget[2].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[2].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[2].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[2].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[2].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[2].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            desc.RenderTarget[3].BlendEnable = TRUE;
            desc.RenderTarget[3].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[3].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[3].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[3].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[3].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[3].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            desc.RenderTarget[4].BlendEnable = TRUE;
            desc.RenderTarget[4].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE
            desc.RenderTarget[4].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[4].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[4].SrcBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[4].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[4].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[4].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            desc.RenderTarget[5].BlendEnable = FALSE;
            desc.RenderTarget[5].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[5].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[5].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[5].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[5].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[5].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[5].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            hr = device_->CreateBlendState(&desc, blendStates[static_cast<int>(BLENDSTATE::MULTIPLERENDERTARGETS)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }
    }

    // 深度ステンシルステート
    {
        D3D11_DEPTH_STENCIL_DESC desc;

        // None
        {
            ZeroMemory(&desc, sizeof(desc));

            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[(int)(DEPTHSTATE::NONE)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // TEST_ON & WRITE_ON
        {
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DEPTHSTATE::ZT_ON_ZW_ON)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // TEST_ON & WRITE_ON
        {
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DEPTHSTATE::ZT_ON_ZW_OFF)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // TEST_ON & WRITE_ON
        {
            desc.DepthEnable = FALSE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DEPTHSTATE::ZT_OFF_ZW_ON)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // TEST_ON & WRITE_ON
        {
            desc.DepthEnable = FALSE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DEPTHSTATE::ZT_OFF_ZW_OFF)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // MASK
        {
            ZeroMemory(&desc, sizeof(desc));

            desc.DepthEnable = TRUE;// 深度テストを行う
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;// 深度値を書き込まない
            desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;// 深度テストに必ず失敗する（描画はしないがステンシル値は書き込む）
            desc.StencilEnable = TRUE;// ステンシルテストを行う
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;// ステンシルテストには必ず合格
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;// 深度テストに失敗してステンシルテストに成功した場合
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[(int)(DEPTHSTATE::MASK)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // APPLY_MASK
        {
            ZeroMemory(&desc, sizeof(desc));

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO; // ステンシルバッファへの書き込みをオフにする
            desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS; // 深度テストに必ず失敗する
            desc.StencilEnable = TRUE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[(int)(DEPTHSTATE::APPLY_MASK)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // EXCLUSIVE
        {
            ZeroMemory(&desc, sizeof(desc));

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO; // ステンシルバッファへの書き込みをオフにする
            desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS; // 深度テストに必ず失敗する
            desc.StencilEnable = TRUE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
            hr = device_->CreateDepthStencilState(&desc, depthStencilStates[(int)(DEPTHSTATE::EXCLUSIVE)].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }
    }

    // ラスタライザーステート
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true;

        // NONE
        {
            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = D3D11_CULL_NONE;
            desc.AntialiasedLineEnable = false;

            hr = device_->CreateRasterizerState(&desc, rasterizerStates[(int)RASTERIZERSTATE::SOLID_CULL_NONE].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // CULL_BACK
        {
            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = D3D11_CULL_BACK;
            desc.AntialiasedLineEnable = false;

            hr = device_->CreateRasterizerState(&desc, rasterizerStates[(int)RASTERIZERSTATE::SOLID_CULL_BACK].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // CULL_FRONT
        {
            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = D3D11_CULL_FRONT;
            desc.AntialiasedLineEnable = false;

            hr = device_->CreateRasterizerState(&desc, rasterizerStates[(int)RASTERIZERSTATE::SOLID_CULL_FRONT].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // WIREFRAME
        {
            desc.FillMode = D3D11_FILL_WIREFRAME;
            desc.CullMode = D3D11_CULL_NONE;
            desc.AntialiasedLineEnable = TRUE;

            hr = device_->CreateRasterizerState(&desc, rasterizerStates[(int)RASTERIZERSTATE::WIREFRAME].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }
    }

    // サンプラーステート
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.BorderColor[0] = 0;
        desc.BorderColor[1] = 0;
        desc.BorderColor[2] = 0;
        desc.BorderColor[3] = 0;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        // MIP_POINT
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WRAP_POINT].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // MIP_LINEAR
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WRAP_LINEAR].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // ANISOTROPIC
        {
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WRAP_ANISOTROPIC].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

        // BLACK_BORDER_POINT
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::BLACK_BORDER_POINT].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // BLACK_BORDER_LINEAR
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::BLACK_BORDER_LINEAR].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // BLACK_BORDER_ANISOTROPIC
        {
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::BLACK_BORDER_ANISOTROPIC].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        desc.BorderColor[0] = 1;
        desc.BorderColor[1] = 1;
        desc.BorderColor[2] = 1;
        desc.BorderColor[3] = 1;

        // WHITE_BORDER_POINT
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WHITE_BORDER_POINT].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // WHITE_BORDER_LINEAR
        {
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WHITE_BORDER_LINEAR].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // WHITE_BORDER_ANISOTROPIC
        {
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::WHITE_BORDER_ANISOTROPIC].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        // SHADOW_ONLY
        {
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.ComparisonFunc = D3D11_COMPARISON_GREATER;
            desc.MinLOD = 0;
            desc.MaxLOD = D3D11_FLOAT32_MAX;
            hr = device_->CreateSamplerState(&desc, samplerStates[SAMPLEMODE::SHADOW].GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }
    }

    // シェーダー
    {
        m_modelshaders[static_cast<int>(SHADERMODE::DEFALT)] = std::make_unique<ModelShader>(ModelShader::MODELSHADER::DEFALT);
        m_modelshaders[static_cast<int>(SHADERMODE::DEFERRED)] = std::make_unique<ModelShader>(ModelShader::MODELSHADER::DEFERRED);
        m_modelshaders[static_cast<int>(SHADERMODE::BLACK)] = std::make_unique<ModelShader>(ModelShader::MODELSHADER::BLACK);
        m_modelshaders[static_cast<int>(SHADERMODE::AREA_EFFECT_CIRCLE)] = std::make_unique<ModelShader>(ModelShader::MODELSHADER::AREA_EFFECT_CIRCLE);
    }

    // レンダラ
    {
        debugRenderer_ = std::make_unique<DebugRenderer>(device_.Get());
        lineRenderer_ = std::make_unique<LineRenderer>(device_.Get(), 1024);
    }

    //スレッドプール
    {
        threadPool_ = std::make_unique<ThreadPool>();
    }
}

// デストラクタ
Graphics::~Graphics()
{
}

DirectX::XMFLOAT3 Graphics::WorldToScreenPos(DirectX::XMFLOAT3 worldPos, std::shared_ptr<CameraCom> camera)
{
    //ビューポート
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    immediateContext_->RSGetViewports(&numViewports, &viewport);

    //変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&camera->GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera->GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&worldPos);

    //ワールド座標からスクリーン座標に変換
    Pos = DirectX::XMVector3Project(
        Pos,
        viewport.TopLeftX, viewport.TopLeftY,
        viewport.Width, viewport.Height,
        viewport.MinDepth, viewport.MaxDepth,
        Projection, View, World
    );

    DirectX::XMFLOAT3 pos;
    DirectX::XMStoreFloat3(&pos, Pos);

    return pos;
}

//サンプラーステートの設定
void Graphics::SetSamplerState()
{
    immediateContext_->PSSetSamplers(0, 1, samplerStates[SAMPLEMODE::WRAP_POINT].GetAddressOf());
    immediateContext_->PSSetSamplers(1, 1, samplerStates[SAMPLEMODE::WRAP_LINEAR].GetAddressOf());
    immediateContext_->PSSetSamplers(2, 1, samplerStates[SAMPLEMODE::WRAP_ANISOTROPIC].GetAddressOf());
    immediateContext_->PSSetSamplers(3, 1, samplerStates[SAMPLEMODE::BLACK_BORDER_POINT].GetAddressOf());
    immediateContext_->PSSetSamplers(4, 1, samplerStates[SAMPLEMODE::BLACK_BORDER_LINEAR].GetAddressOf());
    immediateContext_->PSSetSamplers(5, 1, samplerStates[SAMPLEMODE::BLACK_BORDER_ANISOTROPIC].GetAddressOf());
    immediateContext_->PSSetSamplers(6, 1, samplerStates[SAMPLEMODE::WHITE_BORDER_POINT].GetAddressOf());
    immediateContext_->PSSetSamplers(7, 1, samplerStates[SAMPLEMODE::WHITE_BORDER_LINEAR].GetAddressOf());
    immediateContext_->PSSetSamplers(8, 1, samplerStates[SAMPLEMODE::WHITE_BORDER_ANISOTROPIC].GetAddressOf());
}