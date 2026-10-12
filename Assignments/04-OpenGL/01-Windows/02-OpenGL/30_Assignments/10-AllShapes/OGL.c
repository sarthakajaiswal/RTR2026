// header files 
#include <Windows.h> 
#include <stdio.h> 
#include <stdlib.h> 
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

#define DEG2RAD     (3.14/180.0f) 

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

// toggle related variables 
BOOL bGraph = TRUE; 
BOOL bTriangle = TRUE; 
BOOL bSquare = TRUE; 
BOOL bCircle = TRUE; 

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

            case 'g': 
            case 'G': 
                if(bGraph == TRUE) 
                    bGraph = FALSE; 
                else 
                    bGraph = TRUE; 
                break; 
                
            case 't': 
            case 'T': 
                if(bTriangle == TRUE) 
                    bTriangle = FALSE; 
                else 
                    bTriangle = TRUE; 
                break; 

            case 'r': 
            case 'R': 
                if(bSquare == TRUE) 
                    bSquare = FALSE; 
                else 
                    bSquare = TRUE; 
                break; 

            case 'c': 
            case 'C': 
                if(bCircle == TRUE) 
                    bCircle = FALSE; 
                else 
                    bCircle = TRUE; 
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
    // choose screen clearing color as blue 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    glEnable(GL_POINT_SMOOTH); 

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
    // function declarations 
    void drawGraph(void); 
    void drawCircle(float, float, float); 

    // code 
    glClear(GL_COLOR_BUFFER_BIT); 
    
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();  

    glTranslatef(0.0f, 0.0f, -2.5f); 

    // graph 
    if(bGraph) 
    {
        drawGraph(); 
    } 

    // change state for below shapes 
    glLineWidth(1.0f); 
    glColor3f(1.0f, 1.0f, 0.0f);

    // square vertices  
    float s_vertices[4][2]; 
    float s_v1x, s_v1y;     // vertex1  : top right 
    float s_v2x, s_v2y;     // vertex2  : top left  
    float s_v3x, s_v3y;     // vertex2  : bottom left 
    float s_v4x, s_v4y;     // vertex3  : bottom right 
    
    float vertexToCenterDistance = 0.5f; 

    s_v1x = vertexToCenterDistance*cos(45.0f*3.14/180.0f); 
    s_v1y = vertexToCenterDistance*sin(45.0f*3.14/180.0f); 
    s_v2x = vertexToCenterDistance*cos(135.0f*3.14/180.0f); 
    s_v2y = vertexToCenterDistance*sin(135.0f*3.14/180.0f); 
    s_v3x = vertexToCenterDistance*cos(225.0f*3.14/180.0f); 
    s_v3y = vertexToCenterDistance*sin(225.0f*3.14/180.0f);  
    s_v4x = vertexToCenterDistance*cos(315.0f*3.14/180.0f); 
    s_v4y = vertexToCenterDistance*sin(315.0f*3.14/180.0f);  

    // draw triangle 
    float t_v1x = (s_v1x+s_v2x)/2.0,    t_v1y = (s_v1y+s_v2y)/2.0; 
    float t_v2x = s_v3x,                t_v2y = s_v3y; 
    float t_v3x = s_v4x,                t_v3y = s_v4y; 
    if(bTriangle) 
    {
        glBegin(GL_LINE_LOOP); 
        {
            glVertex2f(t_v1x, t_v1y); 
            glVertex2f(t_v2x, t_v2y); 
            glVertex2f(t_v3x, t_v3y); 
        } 
        glEnd(); 
    } 

    // draw rectangle 
    if(bSquare) 
    {
        glBegin(GL_LINE_LOOP); 
        {
            // rectangle top vertices are taken using triangles vertices. 
            glVertex2f(s_v1x, s_v1y); 
            glVertex2f(s_v2x, s_v2y); 
            glVertex2f(s_v3x, s_v3y); 
            glVertex2f(s_v4x, s_v4y); 
        } 
        glEnd(); 
    } 

    // circle 
    if(bCircle) 
    {
        // calculate radius and center of incenter using triangle vertices 
        // center of circle will be at center because of way we drawn triangle 
        float lenA = sqrtf((t_v3x-t_v2x)*(t_v3x-t_v2x) + (t_v3y-t_v2y)*(t_v3y-t_v2y)); 
        float lenB = sqrtf((t_v3x-t_v1x)*(t_v3x-t_v1x) + (t_v3y-t_v1y)*(t_v3y-t_v1y)); 
        float lenC = sqrtf((t_v2x-t_v1x)*(t_v2x-t_v1x) + (t_v2y-t_v1y)*(t_v2y-t_v1y)); 

        float semiperimeter = (lenA + lenB + lenC) / 2.0f; 
        float area = sqrtf(semiperimeter * (semiperimeter-lenA) * (semiperimeter-lenB) * (semiperimeter-lenC));      // herons formula  

        float centerX = (lenA*t_v1x + lenB*t_v2x + lenC*t_v3x) / (lenA + lenB + lenC); 
        float centerY = (lenA*t_v1y + lenB*t_v2y + lenC*t_v3y) / (lenA + lenB + lenC); 
        float radius = area / semiperimeter; 

        drawCircle(centerX, centerY, radius); 
    } 

    // do double buffering 
    SwapBuffers(ghdc); 
} 

void drawGraph(void) 
{   
    // lines above and below central line 
    int numberOfLines = 20; 
    float distanceBetweenLines = 1.0f / (numberOfLines+1); 

    glColor3f(0.0f, 0.0f, 1.0f); 
    for(int i = 1; i <= 20; ++i) 
    {
        if(i % 5 == 0)  
            glLineWidth(1.5f); 
        else 
            glLineWidth(1.0f); 

        glBegin(GL_LINES); 
        {
            // vertical lines 
            glVertex3f(-i*distanceBetweenLines, 1.0, 0.0f);
            glVertex3f(-i*distanceBetweenLines, -1.0, 0.0f); 
            glVertex3f(i*distanceBetweenLines, 1.0, 0.0f);
            glVertex3f(i*distanceBetweenLines, -1.0, 0.0f); 
            
            // horizontal lines 
            glVertex3f(-1.0f, i*distanceBetweenLines, 0.0f);
            glVertex3f(1.0f, i*distanceBetweenLines, 0.0f); 
            glVertex3f(-1.0f, -i*distanceBetweenLines, 0.0f);
            glVertex3f(1.0f, -i*distanceBetweenLines, 0.0f); 
        } 
        glEnd(); 
    } 

    // central lines 

    glLineWidth(2.0f); 
    glBegin(GL_LINES); 
    {
        // central red line 
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f); 

        // central green line 
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -1.0f, 0.0f); 
    } 
    glEnd(); 
} 

void drawCircle(float cx, float cy, float r)  
{
    glBegin(GL_LINE_LOOP); 
    {        
        float angle = 0.0f; 
        while(angle <= 360.0f) 
        {
            glVertex3f(
                cx + r*cos(angle*DEG2RAD), 
                cy + r*sin(angle*DEG2RAD), 
                0.0f); 
            angle += 1.0f; 
        } 
    }
    glEnd(); 
} 

void update(void) 
{
    // code 
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
