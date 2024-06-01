#include "Framework.h"
#include "ImGuiRender.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneTool.h"
#include <sstream>

//垂直同期間隔設定
static const int syncInterval = 1;

Framework* Framework::instance = nullptr;

//コンストラクタ
Framework::Framework(HWND hWnd)
    : hWnd(hWnd)
    , graphics(hWnd)
    , input(hWnd)
{
    // インスタンス設定
    _ASSERT_EXPR(instance == nullptr, "already instantiated");
    instance = this;

    //シーンの初期化
    SceneManager::Instance().ChangeScene(new SceneTool);

    //IMGUI初期化
    IMGUI_CTRL_INITIALIZE(hWnd, graphics.GetDevice(), graphics.GetDeviceContext());
}

// デストラクタ
Framework::~Framework()
{
    //IMGUI終了化
    IMGUI_CTRL_UNINITIALZE();

    //シーン終了化
    SceneManager::Instance().Clear();
}

// 更新処理
void Framework::Update(float elapsedTime)
{
    // 入力更新処理
    input.Update();

    //シーン更新処理
    SceneManager::Instance().Update(elapsedTime);

    //IMGUI更新
    IMGUI_CTRL_CLEAR_FRAME();
}

// 描画処理
void Framework::Render(float elapsedTime)
{
    // シーン描画処理
    SceneManager::Instance().Render();

    //IMGUI描画
    IMGUI_CTRL_DISPLAY();

    // バックバッファに描画した画を画面に表示する。
    graphics.GetSwapChain()->Present(syncInterval, 0);
}

void Framework::CalculateFrameStats()
{
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        outs << "　MyFrame";
        SetWindowTextA(hWnd, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;
    }
}

int Framework::Run()
{
    MSG msg = {};

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            timer.Tick();
            CalculateFrameStats();

            float elapsedTime = syncInterval == 0
                ? timer.TimeInterval()
                : syncInterval / 60.0f
                ;
            Update(elapsedTime);
            Render(elapsedTime);
        }
    }
    return static_cast<int>(msg.wParam);
}

LRESULT Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        timer.Stop();
        break;
    case WM_EXITSIZEMOVE:
        timer.Start();
        break;
    case WM_MOUSEWHEEL:
        Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}