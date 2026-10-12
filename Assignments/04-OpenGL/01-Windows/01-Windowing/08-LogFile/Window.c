// header files 
#include <Windows.h> 
#include <stdio.h> 
#include <stdlib.h> 

#include "Window.h" 

// macros 
#define WIN_WIDTH   800 
#define WIN_HEIGHT  600 

// global function declarations 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

// global variable declarations 
HWND ghwnd = NULL; 
BOOL bFullscreen = FALSE; 
DWORD dwStyle; 
WINDOWPLACEMENT wpPrev; 

// log file related 
FILE* gpFile = NULL; 

// entry-point function 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) 
{
    // variable declarations 
    WNDCLASSEX wndclass; 
    HWND hwnd = NULL;  
    MSG msg; 
    TCHAR szAppName[] = TEXT("The Standard Window"); 

    // code 
    // create log file 
    gpFile = fopen("log.txt", "w"); 
    if(gpFile == NULL) 
    {
        MessageBox(NULL, TEXT("Log file creation failed"), TEXT("Error"), MB_OK); 
        exit(0); 
    } 
    else 
    {
        fprintf(gpFile, "SAJ: Program started successfully\n"); 
    } 

    // WINDOWCLASSEX initialization 
    wndclass.cbSize         = sizeof(WNDCLASSEX); 
    wndclass.style          = CS_HREDRAW | CS_VREDRAW; 
    wndclass.cbClsExtra     = 0; 
    wndclass.cbWndExtra     = 0; 
    wndclass.lpfnWndProc    = WndProc; 
    wndclass.hInstance      = hInstance; 
    wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wndclass.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON)); 
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW); 
    wndclass.lpszClassName  = szAppName; 
    wndclass.lpszMenuName   = NULL; 
    wndclass.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON)); 
    
    // Register above WNDCLASS 
    RegisterClassEx(&wndclass); 

    // centering 
    int screenWidth = GetSystemMetrics(SM_CXSCREEN); 
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); 

    // Create the window 
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, // app window is special kind of window which is topmost in z order 
                        szAppName, 
                        TEXT("SAJ"), 
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 
                        screenWidth/2 - WIN_WIDTH/2, 
                        screenHeight/2 - WIN_HEIGHT/2, 
                        WIN_WIDTH, 
                        WIN_HEIGHT, 
                        NULL, 
                        NULL, 
                        hInstance, 
                        NULL); 

    // set global window handle 
    ghwnd = hwnd; 

    // Show window 
    ShowWindow(hwnd, iCmdShow); 

    // update the window to paint background 
    UpdateWindow(hwnd); 

    // Message Loop 
    while(GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 

    // close log file 
    if(gpFile) 
    {
        fprintf(gpFile, "SAJ: Program terminated successfully\n"); 
        fclose(gpFile); 
        gpFile = NULL; 
    } 

    return ((int)msg.wParam); 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    // function declarations 
    void toggleFullscreen(void); 

    // code 
    switch(iMsg) 
    {
    case WM_CREATE:
        memset(&wpPrev, 0, sizeof(WINDOWPLACEMENT)); 
        wpPrev.length = sizeof(WINDOWPLACEMENT); 
        break; 

    case WM_SETFOCUS: 
        break; 

    case WM_KILLFOCUS: 
        break; 

    case WM_SIZE: 
        break; 

    case WM_KEYDOWN: 
        switch(wParam) 
        {
        case VK_ESCAPE: 
            break; 
        default: 
            break; 
        } 
        break; 

    case WM_CHAR: 
        switch(wParam) 
        {
            case 'f': 
            case 'F': 
                if(bFullscreen == FALSE) 
                {
                    toggleFullscreen(); 
                    bFullscreen = TRUE; 
                } 
                else 
                {
                    toggleFullscreen(); 
                    bFullscreen = FALSE; 
                } 
                break; 

            default: 
                break; 
        } 
        break; 

    case WM_CLOSE: 
        break; 

    case WM_DESTROY:    
        PostQuitMessage(0); 
        break; 

    default: 
        break; 
    } 

    return (DefWindowProc(hwnd, iMsg, wParam, lParam)); 
} 
    
void toggleFullscreen(void) 
{
    // variable declarations 
    MONITORINFO mi; 

    // code 
    if(bFullscreen == FALSE) 
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE); 

        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            memset(&mi, 0, sizeof(MONITORINFO)); 
            mi.cbSize = sizeof(MONITORINFO); 

            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {    
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW); 
                SetWindowPos(
                    ghwnd, HWND_TOP, 
                    mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right-mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top, 
                    SWP_NOZORDER | SWP_FRAMECHANGED 
                );     
                ShowCursor(FALSE); 
            } 
        } 
    } 
    else 
    {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW); 
        SetWindowPlacement(ghwnd, &wpPrev); 
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED); 
        ShowCursor(TRUE); 
    } 
} 
