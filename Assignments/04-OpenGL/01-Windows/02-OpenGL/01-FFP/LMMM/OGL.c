// header files 
#include <Windows.h> 
#include <stdio.h> 
#include <stdlib.h> 
#define _USE_MATH_DEFINES 1
#include <math.h> 

#include <gl/GL.h>  
#include <gl/glu.h> 

#include "OGL.h" 

// link with OpenGL import library 
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib") 

// macros 
#define WIN_WIDTH   800 
#define WIN_HEIGHT  600 

// global function declarations 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

// global variable declarations 
HWND ghwnd  = NULL; 
HDC ghdc    = NULL; 
HGLRC ghrc  = NULL;  // graphics library baddal genuine mahiti asnaara handle 
BOOL bFullscreen = FALSE; 
DWORD dwStyle; 
WINDOWPLACEMENT wpPrev; 

// log file related 
FILE* gpFile = NULL; 

BOOL bActiveWindow = FALSE; 
BOOL bEscapeKeyIsPressed = FALSE; 

BOOL bDone = FALSE; 

float angleCube = 0.0f; 

// Transformation Matrices (All 16-element column-major 4x4 matrices)
GLfloat identityMatrix[16]; 
GLfloat translationMatrix[16]; 
GLfloat scaleMatrix[16]; 
GLfloat rotationMatrix_X[16]; 
GLfloat rotationMatrix_Y[16]; 
GLfloat rotationMatrix_Z[16];

// entry-point function 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) 
{
    // function declarations 
    int initialize(void); 
    void render(void);  
    void update(void); 
    void uninitialize(void); 

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
    wndclass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // owndc flag asnaarya class ne fixed aani non-discardable memory tyaar hote 
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

    int iResult = initialize(); 
    if(iResult != 0) 
    {
        if(iResult == -1) 
            fprintf(gpFile, "GetDC() failed.\n"); 

        else if(iResult == -2) 
            fprintf(gpFile, "ChoosePixrlFormat() failed\n"); 

        else if(iResult == -3) 
            fprintf(gpFile, "SetPixelFormat() failed\n"); 

        else if(iResult == -4) 
            fprintf(gpFile, "wglCreateContext() failed\n"); 

        else if(iResult == -5) 
            fprintf(gpFile, "wglMakeCurrent() failed\n"); 

        fprintf(gpFile, "WinMain() : initialize() failed\n"); 
        DestroyWindow(hwnd); 
        hwnd = NULL; 
    } 
    else 
    {
        fprintf(gpFile, "WinMain() : initialize() succeded\n"); 
    } 

    // Show window 
    ShowWindow(hwnd, iCmdShow); 

    // update the window to paint background 
    UpdateWindow(hwnd); 

    SetForegroundWindow(hwnd); 
    SetFocus(hwnd); 

    // Game Loop 
    while(bDone == FALSE) 
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            if(msg.message == WM_QUIT) 
            {
                bDone = TRUE; 
            } 
            else 
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } 
        } 
        else 
        {
            if(bActiveWindow == TRUE) 
            {
                if(bEscapeKeyIsPressed == TRUE) 
                    bDone = TRUE; 

                // render 
                render(); 

                // update 
                update(); 
            } 
        } 
    } 

    uninitialize(); 

    return ((int)msg.wParam); 
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    // function declarations 
    void toggleFullscreen(void); 
    void resize(int, int); 
    void uninitialize(void); 

    // code 
    switch(iMsg) 
    {
    case WM_CREATE:
        memset(&wpPrev, 0, sizeof(WINDOWPLACEMENT)); 
        wpPrev.length = sizeof(WINDOWPLACEMENT); 
        break; 

    case WM_SETFOCUS: 
        bActiveWindow = TRUE; 
        break; 

    case WM_KILLFOCUS: 
        bActiveWindow = FALSE; 
        break; 

    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam)); 
        break; 

    case WM_KEYDOWN: 
        switch(wParam) 
        {
        case VK_ESCAPE: 
            bEscapeKeyIsPressed = TRUE; 
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
        uninitialize(); 
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

int initialize(void) 
{
    // function declarations 
    void resize(int, int); 
    void printGLInfo(void); 

    // variable declarations 
    PIXELFORMATDESCRIPTOR pfd; 
    int iPixelFormatIndex;
    
    // code 
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); 
    pfd.nSize       = sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion    = 1; 
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.iPixelType  = PFD_TYPE_RGBA; 
    pfd.cColorBits  = 32; 
    pfd.cRedBits    = 8; 
    pfd.cGreenBits  = 8; 
    pfd.cBlueBits   = 8; 
    pfd.cAlphaBits  = 8; 
    pfd.cDepthBits  = 32; 

    // 1) Ask OS to give its pixel format to match rendering requirements this will require device context too 
    ghdc = GetDC(ghwnd); 
    if(ghdc == NULL) 
    {
        return (-1); 
    } 

    // 2) Choose and select that OS given pixel format 
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd); // index is 1 based not 0 based 
    if(iPixelFormatIndex == 0) 
    {
        return (-2); 
    }
    
    if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) 
    {
        return (-3); 
    } 

    // 3) Use bridging API to create rendering context according to chosen pixel format and device context 
    ghrc = wglCreateContext(ghdc); 
    if(ghrc == NULL) 
    {
        return (-4); 
    }
    
    // 4) Make that rendering context as current rendering context for further rendering 
    if(wglMakeCurrent(ghdc, ghrc) == FALSE) 
    {
        return (-5); 
    }

    // print opengl info 
    printGLInfo(); 

    // 5) start rendering API 
    glShadeModel(GL_SMOOTH); 
    glClearDepth(1.0f); 
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL); 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 

    // choose screen clearing color  
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    // warm-up resize 
    resize(WIN_WIDTH, WIN_HEIGHT); 

    return (0); 
} 

void printGLInfo(void) 
{
    // code 
    fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR)); 
    fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER)); 
    fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION)); 
} 

void resize(int width, int height) 
{
    // code 
    if(height <= 0) 
        height = 1; 
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 

    // perspective projection 
    gluPerspective(
        45.0f, 
        (GLfloat)width / (GLfloat)height, 
        0.1f, 
        100.0f 
    ); 
} 

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    // Identity Matrix
    identityMatrix[0]  = 1.0f; identityMatrix[4]  = 0.0f; identityMatrix[8]  = 0.0f; identityMatrix[12] = 0.0f;
    identityMatrix[1]  = 0.0f; identityMatrix[5]  = 1.0f; identityMatrix[9]  = 0.0f; identityMatrix[13] = 0.0f;
    identityMatrix[2]  = 0.0f; identityMatrix[6]  = 0.0f; identityMatrix[10] = 1.0f; identityMatrix[14] = 0.0f;
    identityMatrix[3]  = 0.0f; identityMatrix[7]  = 0.0f; identityMatrix[11] = 0.0f; identityMatrix[15] = 1.0f;

    glLoadMatrixf(identityMatrix);

    // Translation Matrix (Translating Z by -6.0f)
    translationMatrix[0]  = 1.0f; translationMatrix[4]  = 0.0f; translationMatrix[8]  = 0.0f; translationMatrix[12] = 0.0f;
    translationMatrix[1]  = 0.0f; translationMatrix[5]  = 1.0f; translationMatrix[9]  = 0.0f; translationMatrix[13] = 0.0f;
    translationMatrix[2]  = 0.0f; translationMatrix[6]  = 0.0f; translationMatrix[10] = 1.0f; translationMatrix[14] = -6.0f;
    translationMatrix[3]  = 0.0f; translationMatrix[7]  = 0.0f; translationMatrix[11] = 0.0f; translationMatrix[15] = 1.0f;

    glMultMatrixf(translationMatrix);

    // Scale Matrix
    scaleMatrix[0]  = 0.75f; scaleMatrix[4]  = 0.0f;  scaleMatrix[8]  = 0.0f;  scaleMatrix[12] = 0.0f;
    scaleMatrix[1]  = 0.0f;  scaleMatrix[5]  = 0.75f; scaleMatrix[9]  = 0.0f;  scaleMatrix[13] = 0.0f;
    scaleMatrix[2]  = 0.0f;  scaleMatrix[6]  = 0.0f;  scaleMatrix[10] = 0.75f; scaleMatrix[14] = 0.0f;
    scaleMatrix[3]  = 0.0f;  scaleMatrix[7]  = 0.0f;  scaleMatrix[11] = 0.0f;  scaleMatrix[15] = 1.0f;

    glMultMatrixf(scaleMatrix);

    float angle = angleCube * (float)(M_PI / 180.0f);

    // Rotation Matrix X
    rotationMatrix_X[0] = 1.0f; rotationMatrix_X[4] = 0.0f;        rotationMatrix_X[8]  = 0.0f;        rotationMatrix_X[12] = 0.0f;
    rotationMatrix_X[1] = 0.0f; rotationMatrix_X[5] = cosf(angle);  rotationMatrix_X[9]  = sinf(angle);  rotationMatrix_X[13] = 0.0f;
    rotationMatrix_X[2] = 0.0f; rotationMatrix_X[6] = -sinf(angle); rotationMatrix_X[10] = cosf(angle);  rotationMatrix_X[14] = 0.0f;
    rotationMatrix_X[3] = 0.0f; rotationMatrix_X[7] = 0.0f;        rotationMatrix_X[11] = 0.0f;        rotationMatrix_X[15] = 1.0f;

    // Rotation Matrix Y
    rotationMatrix_Y[0] = cosf(angle);  rotationMatrix_Y[4] = 0.0f; rotationMatrix_Y[8]  = -sinf(angle); rotationMatrix_Y[12] = 0.0f;
    rotationMatrix_Y[1] = 0.0f;         rotationMatrix_Y[5] = 1.0f; rotationMatrix_Y[9]  = 0.0f;        rotationMatrix_Y[13] = 0.0f;
    rotationMatrix_Y[2] = sinf(angle);  rotationMatrix_Y[6] = 0.0f; rotationMatrix_Y[10] = cosf(angle);  rotationMatrix_Y[14] = 0.0f;
    rotationMatrix_Y[3] = 0.0f;         rotationMatrix_Y[7] = 0.0f; rotationMatrix_Y[11] = 0.0f;        rotationMatrix_Y[15] = 1.0f;

    // Rotation Matrix Z
    rotationMatrix_Z[0] = cosf(angle);  rotationMatrix_Z[4] = -sinf(angle); rotationMatrix_Z[8]  = 0.0f; rotationMatrix_Z[12] = 0.0f;
    rotationMatrix_Z[1] = sinf(angle);  rotationMatrix_Z[5] = cosf(angle);  rotationMatrix_Z[9]  = 0.0f; rotationMatrix_Z[13] = 0.0f;
    rotationMatrix_Z[2] = 0.0f;         rotationMatrix_Z[6] = 0.0f;        rotationMatrix_Z[10] = 1.0f; rotationMatrix_Z[14] = 0.0f;
    rotationMatrix_Z[3] = 0.0f;         rotationMatrix_Z[7] = 0.0f;        rotationMatrix_Z[11] = 0.0f; rotationMatrix_Z[15] = 1.0f;

    glMultMatrixf(rotationMatrix_X);
    glMultMatrixf(rotationMatrix_Y);
    glMultMatrixf(rotationMatrix_Z);

    // Draw 3D Cube
    glBegin(GL_QUADS);

    // Front Face (Red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Right Face (Green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Back Face (Blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    // Left Face (Cyan)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Top Face (Magenta)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    // Bottom Face (Yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    glEnd();

    SwapBuffers(ghdc);
}

void update(void)
{
    angleCube = angleCube + 0.2f; 
    if(angleCube > 360.0f)
        angleCube = angleCube - 360.0f; 
} 
void uninitialize(void) 
{
    // code 
    // if exitting in fullscreen, first restore and then proceed 
    if(bFullscreen == TRUE) 
    {
        toggleFullscreen(); 
        bFullscreen = FALSE; 
    } 

    // first check current context and if it is, then unmake it. 
    if(wglGetCurrentContext() == ghrc) 
    {
        wglMakeCurrent(NULL, NULL); 
    } 

    // now destroy rendering context 
    if(ghrc)
    {
        wglDeleteContext(ghrc); 
        ghrc = NULL; 
    } 

    // release device context 
    if(ghdc) 
    {
        ReleaseDC(ghwnd, ghdc); 
        ghdc = NULL; 
    } 

    // destroy window 
    if(ghwnd) 
    {
        DestroyWindow(ghwnd); 
        ghwnd = NULL; 
    } 

    // close log file 
    if(gpFile) 
    {
        fprintf(gpFile, "SAJ: Program terminated successfully\n"); 
        fclose(gpFile); 
        gpFile = NULL; 
    } 
} 
