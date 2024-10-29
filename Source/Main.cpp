#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "Framework.h"
#include "ImGuiRender.h"

const LONG SCREEN_WIDTH = static_cast<LONG>(1920);
const LONG SCREEN_HEIGHT = static_cast<LONG>(900/*1080*/);
const LPCWSTR TITLE = L"Game";

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
    srand(static_cast<unsigned int>(time(nullptr)));

#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //メモリリーク検索
    //_CrtSetBreakAlloc(212925);
    //_CrtSetBreakAlloc(258);
    //_CrtSetBreakAlloc(264);
    //_CrtSetBreakAlloc(260);
#endif

    // サイズ調整
    DWORD dw_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    DWORD dw_ex_style = WS_EX_APPWINDOW;
    RECT rect;
    bool resize = true;
    if (resize)
    {
        dw_style |= WS_THICKFRAME;
    }
    ::SetRect(&rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ::AdjustWindowRectEx(&rect, dw_style, FALSE, 0);
    LONG width = rect.right - rect.left;
    LONG height = rect.bottom - rect.top;

    // ウインドウクラス設定
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = fnWndProc;		//ウィンドウのメッセージを処理するためのコールバック関数
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);	//ウィンドウ背景色
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _T("Game");
    wcex.hIconSm = 0;
    RegisterClassEx(&wcex);

    // ウインドウ作成
    HWND hwnd = ::CreateWindowEx(dw_ex_style, TITLE, TITLE, dw_style, 0, 0, width, height, NULL, nullptr, instance, NULL);

    ShowWindow(hwnd, cmd_show);
    Framework framework(hwnd);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
    return framework.Run();
}