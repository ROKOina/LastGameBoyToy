#pragma once
#define _WINSOCKAPI_  // windows.hを定義した際に、winsock.hを自動的にインクルードしない
#include <Windows.h>

// ダイアログリザルト
enum class DialogResult
{
    Yes,
    No,
    OK,
    Cancel
};

// ダイアログ
class Dialog
{
public:
    // [ファイルを開く]ダイアログボックスを表示
    static DialogResult OpenFileName(char* filepath, int size, const char* filter = nullptr, const char* title = nullptr, HWND hWnd = NULL, bool multiSelect = false);

    // [ファイルを保存]ダイアログボックスを表示
    static DialogResult SaveFileName(char* filepath, int size, const char* filter = nullptr, const char* title = nullptr, const char* ext = nullptr, HWND hWnd = NULL);
};
