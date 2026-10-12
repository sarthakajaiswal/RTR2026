#pragma once

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h> 

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

#define ENABLE_IMGUI    1 

class ImGuiManager {
public:
    static bool Initialize(HWND hwnd);
    static void Cleanup();
    static void NewFrame();
    static void Render();
    static bool IsEnabled() { return ENABLE_IMGUI; }

    // Add your custom ImGui windows here
    static void RenderDebugWindow();
    
public:
    static bool initialized;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

