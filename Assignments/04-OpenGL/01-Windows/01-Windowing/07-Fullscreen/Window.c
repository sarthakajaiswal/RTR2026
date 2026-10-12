/*
    FULLSCREEN ALGORITHM 
1) Make our window using extended style to hide taskbar and also set window styles accordingly 
2) Check wether we are already in fullscreen or not 
3) If not 
    A) Get current window style 
    B) Check wether this style contains WS_OVERLAPPEDWINDOW 
    C) Ge Current Window placement 
    D) Get current monitor information 
    E) If both above are obtained then remove WS_OVERLAPPEDWINDOW style from current style 
    F) Set Window position to obtained current monitor info dimentions 
    G) Usually in fullscreen cursor is hidden, so hide the cursor 
4) But if window is already fullscreen then 
    A) again add WS_OVERLAPPEDWINDOW to style 
    B) Set window placement to original which was before fullscreen 
    C) Set the window position accordingly as before 
    D) Start showing cursor which was hidden 
*/

// header files 
#include <Windows.h> 

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
    // 2) Check wether we are already in fullscreen or not 
    if(bFullscreen == FALSE) 
    {
        // A) Get current window style 
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE); 

        // B) Check wether this style contains WS_OVERLAPPEDWINDOW 
        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            memset(&mi, 0, sizeof(MONITORINFO)); 
            mi.cbSize = sizeof(MONITORINFO); 

            // C) Get Current Window placement and current monitor info 
            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                // E) If both above are obtained then remove WS_OVERLAPPEDWINDOW style from current style 
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW); 

                // F) Set Window position to obtained current monitor info dimentions 
                SetWindowPos(
                    ghwnd, HWND_TOP, 
                    mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right-mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top, 
                    SWP_NOZORDER | SWP_FRAMECHANGED 
                ); 
                // WSP_FRAMECHANGED -> using this flag internally posts message WM_NCCALCSIZE to calculate size of frame (non-client area/bars of window)

                // G) Usually in fullscreen cursor is hidden, so hide the cursor 
                ShowCursor(FALSE); 
            } 
        } 
    }  
    // 4) But if window is already fullscreen then 
    else 
    {
        // A) again add WS_OVERLAPPEDWINDOW to style 
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW); 

        // B) Set window placement to original which was before fullscreen 
        SetWindowPlacement(ghwnd, &wpPrev); 
        
        // C) Set the window position accordingly as before 
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED); 
        // SWP_NOMOVE -> window position set kratanna setWindowPlacement() ne set keleli placement move kru nko  
        // SWP_NOSIZE -> window position set kratanna setWindowPlacement() ne set keleli size bdlu nko  
        // SWP_NOOWNERZORDER -> if owned window changed order, then do not change my order (owned window- like window created on hovering)  
        
        // D) Start showing cursor which was hidden 
        ShowCursor(TRUE); 
    } 
} 
