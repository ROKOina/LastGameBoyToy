#pragma warning(disable: 4996)

#ifndef INCLUDED_IMGUI_CTRL_H
#define INCLUDED_IMGUI_CTRL_H

//------< pragma >------------------------------------------------------------------------
#pragma once

//------< macro >-------------------------------------------------------------------------

#define USE_IMGUI 1

//------< include >-----------------------------------------------------------------------
#if USE_IMGUI
#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImSequencer.h"
#include <vector>
#endif

//------< extern >------------------------------------------------------------------------
#if USE_IMGUI
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[]; //ref: imgui_ja_gryph_ranges.cpp
#endif

//========================================================================================
//
//      ImGuiCtrl
//
//========================================================================================
#if USE_IMGUI
namespace ImGuiCtrl
{
    void Initialize(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* immediateContext);

    // ImGui::Begin()前に呼び出す。
    void ClearFrame();

    // 描画結果を出力
    void Display();

    //imguiのResize
    void Resize(float width, float height);

    void Uninitialize();
}
#endif // USE_IMGUI

//------< macro >-------------------------------------------------------------------------
#if USE_IMGUI
#define IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wParam, lParam) \
if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true
#define IMGUI_CTRL_INITIALIZE(hwnd, device, context) ImGuiCtrl::Initialize(hwnd, device, context)
#define IMGUI_CTRL_CLEAR_FRAME() ImGuiCtrl::ClearFrame()
#define IMGUI_CTRL_DISPLAY() ImGuiCtrl::Display()
#define IMGUI_CTRL_UNINITIALZE() ImGuiCtrl::Uninitialize()
#define IMGUI_CTRL_RESIZE(width,height)ImGuiCtrl::Resize(width,height)
#else
#define IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wParam, lParam)
#define IMGUI_CTRL_INITIALIZE(hwnd, device, context)
#define IMGUI_CTRL_CLEAR_FRAME()
#define IMGUI_CTRL_DISPLAY()
#define IMGUI_CTRL_UNINITIALZE()

#endif // USE_IMGUI

#endif // !INCLUDED_IMGUI_CTRL_H