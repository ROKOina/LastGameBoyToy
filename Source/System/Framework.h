#pragma once

#include <windows.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include "HighResolutionTimer.h"
#include "Graphics/Graphics.h"
#include "Input/input.h"

class Framework
{
public:
    Framework(HWND hWnd);
    ~Framework();

    //インスタンス取得
    static Framework& Instance() { return *instance; }

private:
    void Update(float elapsedTime/*Elapsed seconds from last frame*/);
    void Render(float elapsedTime/*Elapsed seconds from last frame*/);

    void CalculateFrameStats();

public:
    int Run();
    LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    const HWND& GetHwnd() const { return hWnd; }

private:
    const HWND				hWnd;
    HighResolutionTimer		timer;
    Graphics				graphics;
    Input					input;
    static Framework* instance;
};