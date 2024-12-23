#include <memory>
#include <sstream>
#include <tchar.h>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "Scene\SceneGame\SceneGame.h"
#include "Scene\SceneManager.h"
#include "Scene\SceneUI\SceneUI.h"
#include "Scene\SceneResult\SceneResult.h"
#include "Scene\SceneSugimoto\SceneSugimoto.h"

#include "Scene\\SceneTitle\SceneTitle.h"
#include "ImGuiRender.h"
#include <imgui.h>
#include <ImGuizmo.h>

// 垂直同期間隔設定
static const int syncInterval = 1;

Framework* Framework::instance = nullptr;

// コンストラクタ
Framework::Framework(HWND hWnd)
    : hWnd_(hWnd)
    , input_(hWnd)
    , graphics_(hWnd)
{
    //sceneGame.Initialize();
    SceneManager::Instance().ChangeScene(new SceneGame);
    //SceneManager::Instance().ChangeScene(new SceneStageEditor);
    //SceneManager::Instance().ChangeScene(new SceneTitle);
    //SceneManager::Instance().ChangeScene(new SceneResult);
    //SceneManager::Instance().ChangeScene(new SceneIKTest);
    //SceneManager::Instance().ChangeScene(new SceneTitle);
     //SceneManager::Instance().ChangeScene(new SceneUI);

    // オーディオ初期化
    Audio::Initialize();

    //IMGUI初期化
    IMGUI_CTRL_INITIALIZE(hWnd_, graphics_.GetDevice(), graphics_.GetDeviceContext());

    instance = this;
}

// デストラクタ
Framework::~Framework()
{
    //sceneGame.Finalize();
    SceneManager::Instance().Clear();

    //IMGUI終了化
    IMGUI_CTRL_UNINITIALZE();
}

#include "Logger.h"
clock_t startTime1 = 0, endTime1 = 0;

// 更新処理
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/, float fps_)
{
    // 入力更新処理
    input_.Update();

    // シーン更新処理
    SceneManager::Instance().Update(elapsedTime);

    //IMGUI更新
    IMGUI_CTRL_CLEAR_FRAME();
}

// 描画処理
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
    //別スレッド中にデバイスコンテキストが使われていた場合に
    //同時アクセスしないように排他制御する
    std::lock_guard<std::mutex> lock(graphics_.GetMutex());

    ID3D11DeviceContext* dc = graphics_.GetDeviceContext();

    //シーンだけ上に描画
    //PostEffect::Instance().SceneImGui();

    //imguiguizmo
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    RECT r;
    ::GetWindowRect(hWnd_, &r);
    ImGuizmo::SetRect(static_cast<float>(r.left), static_cast<float>(r.top), static_cast<float>(r.right) - static_cast<float>(r.left), static_cast<float>(r.bottom) - static_cast<float>(r.top));

    // シーン描画処理
    SceneManager::Instance().Render(elapsedTime);

    //IMGUI描画
    IMGUI_CTRL_DISPLAY();

    // バックバッファに描画した画を画面に表示する。
    graphics_.GetSwapChain()->Present(syncInterval, 0);

    // リサイズ
    if (resize)
    {
        graphics_.ResizeBackBuffer(width, height);
        //PostEffect::Instance().ResizeBuffer();
        resize = false;
    }
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
    // Code computes the average frames per second, and also the
    // average time it takes to render one frame.  These stats
    // are appended to the window caption bar.
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    // Compute averages over one second period.
    if ((timer_.TimeStamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        SetWindowTextA(hWnd_, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;

        //FPSをセット
        Graphics::Instance().SetFPSFramework(fps);
    }
}

void Framework::Resize(int w, int h)
{
    width = (std::max)(16, w);
    height = (std::max)(16, h);
    resize = true;

#ifdef USE_IMGUI
    IMGUI_CTRL_RESIZE(width, height);
#endif
}

// アプリケーションループ
int Framework::Run()
{
    MSG msg = {};

    BOOL fullscreen = 0;
    graphics_.GetSwapChain()->GetFullscreenState(&fullscreen, 0);
    if (fullscreen)
    {
        graphics_.GetSwapChain()->SetFullscreenState(TRUE, 0);
    }

    //終了コードならwhileぬける
    while (WM_QUIT != msg.message)
    {
        //PeekMessage = メッセージ受信
        //メッセージを取得しなかった場合、0 が返る
        //第3,4引数、両方0の場合すべてのメッセージを返す
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	//PM_REMOVE = PeekMessage 関数がメッセージを処理した後、そのメッセージをキューから削除
        {
            TranslateMessage(&msg);	//メッセージ変換
            DispatchMessage(&msg);	//プロシージャへコールバック
        }
        else
        {
            timer_.Tick();
            CalculateFrameStats();

            float elapsedTime = syncInterval == 0
                ? timer_.TimeInterval()
                : syncInterval / 60.0f
                //: syncInterval / fps_
                ;

            startTime1 = clock();

            Update(elapsedTime, fps_);
            Render(elapsedTime);

            endTime1 = clock();
            //Logger::Print((std::string("Frame Time : ") + std::to_string(endTime1 - startTime1) + "\n\n").c_str());
        }
    }

    //// ComPtr用リーク型名表示　作成
    //typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
    //HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
    //fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

    //DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&debugGI_);

    //debugGI_->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_DETAIL);

    //Graphics::Instance().GetDevice()->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugID_);
    //debugID_->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
    //debugID_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //imguiのハンドル
    IMGUI_CTRL_WND_PRC_HANDLER(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;
    case WM_ENTERSIZEMOVE:
        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
        timer_.Stop();
        break;
    case WM_EXITSIZEMOVE:
        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
        timer_.Start();
        break;
    case WM_MOUSEWHEEL:
        Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    case WM_SIZE:
    {
        // サイズ変更
        Resize(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}