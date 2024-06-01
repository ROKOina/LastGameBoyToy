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

static const char* SequencerItemTypeNames[] = { "Animation" };

struct MySequence : public ImSequencer::SequenceInterface
{
    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }

    virtual int GetItemCount() const { return (int)myItems.size(); }

    virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
    virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
    virtual const char* GetItemLabel(int index) const
    {
        static char tmps[512];
        snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
        return tmps;
    }

    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }
    virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
    virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
    virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

    virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(100) {}
    int mFrameMin, mFrameMax;
    struct MySequenceItem
    {
        int mType;
        int mFrameStart, mFrameEnd;
        bool mExpanded;
    };
    std::vector<MySequenceItem> myItems;

    virtual void DoubleClick(int index) {
        if (myItems[index].mExpanded)
        {
            myItems[index].mExpanded = false;
            return;
        }
        for (auto& item : myItems)
            item.mExpanded = false;
        myItems[index].mExpanded = !myItems[index].mExpanded;
    }
};

#else
#define IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wParam, lParam)
#define IMGUI_CTRL_INITIALIZE(hwnd, device, context)
#define IMGUI_CTRL_CLEAR_FRAME()
#define IMGUI_CTRL_DISPLAY()
#define IMGUI_CTRL_UNINITIALZE()

#endif // USE_IMGUI

#endif // !INCLUDED_IMGUI_CTRL_H