#pragma once

#include <windows.h>
#include "HighResolutionTimer.h"
#include "Audio/Audio.h"
#include "Graphics/Graphics.h"
#include "Input/input.h"

#include "dxgidebug.h"

class Framework
{
public:
    Framework(HWND hWnd);
    ~Framework();

private:
    void Update(float elapsedTime/*Elapsed seconds from last frame*/, float fps_);
    void Render(float elapsedTime/*Elapsed seconds from last frame*/);

    void CalculateFrameStats();

public:
    int Run();
    LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
    const HWND& GetHWND() {
      return hWnd_;
    }

    static Framework* GetInstance() {
      return instance;
    }

#endif // _DEBUG

private:
    const HWND				hWnd_;
    HighResolutionTimer		timer_;
    Audio					audio_;
    Graphics				graphics_;
    Input					input_;

    float fps_;

    Microsoft::WRL::ComPtr<IDXGIDebug>				debugGI_;
    Microsoft::WRL::ComPtr<ID3D11Debug>				debugID_;

#ifdef _DEBUG
    static Framework* instance;

#endif // _DEBUG
};