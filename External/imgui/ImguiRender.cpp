//------< include >-----------------------------------------------------------------------

#include "ImguiRender.h"

//========================================================================================
//
//      ImGuiCtrl
//
//========================================================================================
#if USE_IMGUI
namespace ImGuiCtrl
{
    void Initialize(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* immediateContext)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        ImGui::GetIO().Fonts->AddFontFromFileTTF("Data\\Font\\ArialUni.ttf", 18.0f, nullptr, glyphRangesJapanese);
        ImGui_ImplWin32_Init(windowHandle);
        ImGui_ImplDX11_Init(device, immediateContext);
        ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        {
            //文字色
            style.Colors[ImGuiCol_Text] = { 0,0,0,1 };

            //チェックマーク色変化
            style.Colors[ImGuiCol_CheckMark] = { 1,0,0,1 };

            //ボタン色変化
            style.Colors[ImGuiCol_Button] = { 0,1,1,0 };

            //縁
            style.Colors[ImGuiCol_Border] = { 2,1,3,1 };

            //閉じたときの色変化
            style.Colors[ImGuiCol_BorderShadow] = { 1,0,1,1 };

            //スライダーの色変化
            style.Colors[ImGuiCol_SliderGrab] = { 1,0,1,1 };

            //separetの色変化
            style.Colors[ImGuiCol_Separator] = { 0,0,0,1 };

            //下の△の色変化
            style.Colors[ImGuiCol_ResizeGrip] = { 2,0,0,1 };

            //windowの色
            style.Colors[ImGuiCol_WindowBg] = { 1,0.5f,1,0.2f };
        }
    }

    void ClearFrame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void Display()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void Uninitialize()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}

#endif // USE_IMGUI