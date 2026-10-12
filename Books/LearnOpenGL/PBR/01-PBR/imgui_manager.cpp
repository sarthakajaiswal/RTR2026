#include <iostream> 
#include <stdio.h> 
#include <vector> 

#include "imgui_manager.h" 

#include "vmath.h" 
#include "E_camera.h"   

extern HWND ghwnd; 
extern FILE* gpFile; 
extern E_camera camera; 
extern int specularPower; 

extern vmath::vec3 lightPosition; 
extern vmath::vec3 lightAmbient; 
extern vmath::vec3 lightDiffuse; 
extern vmath::vec3 lightSpecular; 

extern vmath::vec3 materialAmbient; 
extern vmath::vec3 materialDiffuse; 
extern vmath::vec3 materialSpecular; 
extern float materialShininess; 

bool ImGuiManager::initialized = false;

bool ImGuiManager::Initialize(HWND hwnd) {
    if (initialized || !ENABLE_IMGUI)
        return false;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    initialized = true;
    return true;
}

void ImGuiManager::Cleanup() {
    if (!initialized || !ENABLE_IMGUI)
        return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    initialized = false;
}

void ImGuiManager::NewFrame() {
    if (!initialized || !ENABLE_IMGUI)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    if (!initialized || !ENABLE_IMGUI)
        return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::RenderDebugWindow() {
    // function declarations 

    // code 
    if (!initialized || !ENABLE_IMGUI)
        return;

    // Create a debug window
    ImGui::Begin("Debug Window");

    // Add any debug information you want to display
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Spacing(); 
    ImGui::Text("Light"); 


    ImGui::Spacing(); 
    ImGui::Text("Camera"); 
    ImGui::SliderFloat("speed", &camera.speed, 0.01f, 0.5f); 

    ImGui::End(); 
}

