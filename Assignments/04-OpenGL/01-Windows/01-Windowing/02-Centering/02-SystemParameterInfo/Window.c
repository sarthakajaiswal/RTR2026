// header files 
#include <Windows.h> 

// macros 
#define WIN_WIDTH   800 
#define WIN_HEIGHT  600 

// global function declarations 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

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
    wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION); 
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW); 
    wndclass.lpszClassName  = szAppName; 
    wndclass.lpszMenuName   = NULL; 
    wndclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION); 
    
    // Register above WNDCLASS 
    RegisterClassEx(&wndclass); 

    // centering 
    RECT screenSize; 
    SystemParametersInfo(SPI_GETWORKAREA, sizeof(screenSize), &screenSize, 0); 
    int screenWidth = screenSize.right - screenSize.left; 
    int screenHeight = screenSize.bottom - screenSize.top; 

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
    // code 
    switch(iMsg) 
    {
    case WM_DESTROY:    
        PostQuitMessage(0); 
        break; 
    default: 
        break; 
    } 

    return (DefWindowProc(hwnd, iMsg, wParam, lParam)); 
} 
