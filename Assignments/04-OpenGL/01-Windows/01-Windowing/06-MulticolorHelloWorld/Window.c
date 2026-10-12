// header files 
#include <Windows.h> 

#include "Window.h" 

// macros 
#define WIN_WIDTH   800 
#define WIN_HEIGHT  600 

// global function declarations 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

// global variable declarations 
int iPaintFlag = -1; 

// entry-point function 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) 
{
    // variable declarations 
    WNDCLASSEX wndclass; 
    HWND hwnd = NULL;  
    MSG msg; 
    TCHAR szAppName[] = TEXT("The Standard Window"); 

    // code 
    // WINDOWCLASSEX initialization 
    wndclass.cbSize         = sizeof(WNDCLASSEX); 
    wndclass.style          = CS_HREDRAW | CS_VREDRAW; 
    wndclass.cbClsExtra     = 0; 
    wndclass.cbWndExtra     = 0; 
    wndclass.lpfnWndProc    = WndProc; 
    wndclass.hInstance      = hInstance; 
    wndclass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH); 
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
    hwnd = CreateWindow(szAppName, 
                        TEXT("SAJ"), 
                        WS_OVERLAPPEDWINDOW, 
                        screenWidth/2 - WIN_WIDTH/2, 
                        screenHeight/2 - WIN_HEIGHT/2, 
                        WIN_WIDTH, 
                        WIN_HEIGHT, 
                        NULL, 
                        NULL, 
                        hInstance, 
                        NULL); 

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

    return ((int)msg.wParam); 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    // variable declarations 
    HDC hdc; 
    PAINTSTRUCT ps; 
    RECT rc; 
    TCHAR str[] = TEXT("Hello World !!!"); 

    // code 
    switch(iMsg) 
    {
    case WM_CREATE:
        break;          // return here itself, if message dont need to be passed to DefWindowProc() 
                        // used in API hooking, dll hooking 

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
        case 'F': 
        case 'f': 
            break; 

        case 'R': 
        case 'r': 
            iPaintFlag = 1; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case 'G': 
        case 'g': 
            iPaintFlag = 2; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case 'B': 
        case 'b': 
            iPaintFlag = 3; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case 'C': 
        case 'c': 
            iPaintFlag = 4; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case 'M': 
        case 'm': 
            iPaintFlag = 5; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        case 'Y': 
        case 'y': 
            iPaintFlag = 6; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 

        default: 
            iPaintFlag = -1; 
            InvalidateRect(hwnd, NULL, TRUE); 
            break; 
        } 
        break; 

    case WM_PAINT: 
        GetClientRect(hwnd, &rc); 
        hdc = BeginPaint(hwnd, &ps);    // while handeling WM_PAINT it is recommended to get hdc using BeginPaint() not by GetDC() 
        
        SetBkColor(hdc, RGB(0, 0, 0)); 
        if (iPaintFlag == 1) 
            SetTextColor(hdc, RGB(255, 0, 0)); 
        else if (iPaintFlag == 2) 
            SetTextColor(hdc, RGB(0, 255, 0)); 
        else if (iPaintFlag == 3) 
            SetTextColor(hdc, RGB(0, 0, 255)); 
        else if (iPaintFlag == 4) 
            SetTextColor(hdc, RGB(0, 255, 255)); 
        else if (iPaintFlag == 5) 
            SetTextColor(hdc, RGB(255, 0, 255)); 
        else if (iPaintFlag == 6) 
            SetTextColor(hdc, RGB(255, 255, 0)); 
        else 
            SetTextColor(hdc, RGB(255, 255, 255)); 

        DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER); 
        EndPaint(hwnd, &ps); 
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
