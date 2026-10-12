#define _WIN32_WINNT 0x0600

// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h> // For CPU Hardware Detection (__cpuid)

// Define missing flag if using older SDK headers
#ifndef DISPLAY_DEVICE_MIRROR_DRIVER
#define DISPLAY_DEVICE_MIRROR_DRIVER 0x00000008
#endif

// Custom header files
#include "OGL.h"

// OpenGL header files
#include <gl/glew.h>    // Must be included before gl/GL.h
#include <gl/GL.h> 

#include "vmath.h" 
using namespace vmath; 

// OpenGL libraries
#pragma comment(lib, "glew32.lib") 
#pragma comment(lib, "opengl32.lib") 

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev;

// OpenGL related variables
HDC ghdc = NULL; 
HGLRC ghrc = NULL; 

// Window dimensions tracker for Ortho projection
int g_winWidth = WIN_WIDTH;
int g_winHeight = WIN_HEIGHT;

// 3D Scene Shader Variables
GLuint shaderProgramObject; 

enum 
{
    AMC_ATTRIBUTE_POSITION = 0, 
    AMC_ATTRIBUTE_COLOR, 
}; 

GLuint vao = 0; 
GLuint vbo_position = 0; 
GLuint vbo_color = 0; 

GLuint mvpMatrixUniform = 0; 
mat4 perspectiveProjectionMatrix; 

// 2D Text HUD Shader Variables
GLuint g_textShaderProgram = 0;
GLuint g_fontTexture = 0;
GLuint g_textVAO = 0;
GLuint g_textVBO = 0;
GLuint g_textOrthoUniform = 0;
GLuint g_textColorUniform = 0;

// Hardware Information & HUD Mode
enum HUDMode {
    HUD_SHOW_CPU = 0,
    HUD_SHOW_GPU
};
enum HUDMode g_hudMode = HUD_SHOW_CPU;

char g_cpuName[128] = "Unknown CPU";
char g_gpuName[128] = "Unknown GPU";

struct CharacterInfo {
    float u0, v0, u1, v1;
    int width;
} g_characters[128];

// File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// Active Window & Exit variables
BOOL gbActiveWindow = FALSE;
BOOL gbEscapeKeyIsPressed = FALSE;

// =================== HARDWARE & TIMER HELPERS ===================
void queryCPUName(void)
{
    int cpuInfo[4] = { -1 };
    char brandString[0x40] = { 0 };

    __cpuid(cpuInfo, 0x80000000);
    unsigned int nExIds = cpuInfo[0];

    if (nExIds >= 0x80000004)
    {
        __cpuid((int*)(brandString), 0x80000002);
        __cpuid((int*)(brandString + 16), 0x80000003);
        __cpuid((int*)(brandString + 32), 0x80000004);

        char* p = brandString;
        while (*p == ' ') p++;
        snprintf(g_cpuName, sizeof(g_cpuName), "%s", p);
    }
}

void queryGPUName(void)
{
    DISPLAY_DEVICEA dd = { 0 };
    dd.cb = sizeof(DISPLAY_DEVICEA);
    BOOL foundDiscrete = FALSE;

    // Enumerate all display devices to find discrete graphics
    for (DWORD i = 0; EnumDisplayDevicesA(NULL, i, &dd, 0); i++)
    {
        if (dd.StateFlags & DISPLAY_DEVICE_MIRROR_DRIVER)
            continue;

        // Check for dedicated GPU brands
        if (strstr(dd.DeviceString, "NVIDIA") != NULL || 
            strstr(dd.DeviceString, "GeForce") != NULL ||
            strstr(dd.DeviceString, "AMD") != NULL || 
            strstr(dd.DeviceString, "Radeon") != NULL)
        {
            snprintf(g_gpuName, sizeof(g_gpuName), "%s", dd.DeviceString);
            foundDiscrete = TRUE;
            break;
        }
    }

    // Fallback: Use active OpenGL renderer string
    if (!foundDiscrete)
    {
        const char* gpuStr = (const char*)glGetString(GL_RENDERER);
        if (gpuStr)
        {
            snprintf(g_gpuName, sizeof(g_gpuName), "%s", gpuStr);
        }
    }
}

float calculateFPS(void)
{
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER lastTime;
    static BOOL initialized = FALSE;
    static float fps = 0.0f;
    static int frameCount = 0;
    static float timeAccumulator = 0.0f;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);
        initialized = TRUE;
    }

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    float deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart;
    lastTime = currentTime;

    frameCount++;
    timeAccumulator += deltaTime;

    if (timeAccumulator >= 0.25f)
    {
        fps = (float)frameCount / timeAccumulator;
        frameCount = 0;
        timeAccumulator = 0.0f;
    }

    return fps;
}

// =================== PROGRAMMABLE TEXT ENGINE ===================
BOOL initFontEngine(HDC hdc)
{
    // 1. Text Shaders
    const GLchar* vertexShaderSource = 
        "#version 460 core\n"
        "layout(location = 0) in vec2 aPosition;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "out vec2 outTexCoord;\n"
        "uniform mat4 uOrthoMatrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = uOrthoMatrix * vec4(aPosition, 0.0, 1.0);\n"
        "    outTexCoord = aTexCoord;\n"
        "}\n";

    const GLchar* fragmentShaderSource = 
        "#version 460 core\n"
        "in vec2 outTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uFontTexture;\n"
        "uniform vec4 uTextColor;\n"
        "void main(void)\n"
        "{\n"
        "    float alpha = texture(uFontTexture, outTexCoord).r;\n"
        "    FragColor = vec4(uTextColor.rgb, uTextColor.a * alpha);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);

    g_textShaderProgram = glCreateProgram();
    glAttachShader(g_textShaderProgram, vs);
    glAttachShader(g_textShaderProgram, fs);
    glLinkProgram(g_textShaderProgram);

    glDetachShader(g_textShaderProgram, vs);
    glDetachShader(g_textShaderProgram, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    g_textOrthoUniform = glGetUniformLocation(g_textShaderProgram, "uOrthoMatrix");
    g_textColorUniform = glGetUniformLocation(g_textShaderProgram, "uTextColor");

    // 2. Generate Texture Atlas via GDI DIB
    HDC hMemDC = CreateCompatibleDC(hdc);
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 512;
    bmi.bmiHeader.biHeight = -512;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);

    HFONT hFont = CreateFontA(-24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, MONO_FONT | FF_DONTCARE, "Consolas");
    SelectObject(hMemDC, hFont);

    SetTextColor(hMemDC, RGB(255, 255, 255));
    SetBkColor(hMemDC, RGB(0, 0, 0));
    SetBkMode(hMemDC, OPAQUE);

    memset(pBits, 0, 512 * 512 * 4);

    int cellW = 32, cellH = 32;
    for (int i = 32; i < 127; i++)
    {
        int col = (i - 32) % 16;
        int row = (i - 32) / 16;
        int x = col * cellW;
        int y = row * cellH;

        char ch = (char)i;
        SIZE sz;
        GetTextExtentPoint32A(hMemDC, &ch, 1, &sz);
        TextOutA(hMemDC, x, y, &ch, 1);

        g_characters[i].u0 = (float)x / 512.0f;
        g_characters[i].v0 = (float)y / 512.0f;
        g_characters[i].u1 = (float)(x + cellW) / 512.0f;
        g_characters[i].v1 = (float)(y + cellH) / 512.0f;
        g_characters[i].width = sz.cx;
    }

    // Convert RGB bitmap buffer to Alpha channel map
    unsigned char* alphaBuffer = (unsigned char*)malloc(512 * 512);
    unsigned char* pSrc = (unsigned char*)pBits;
    for (int i = 0; i < 512 * 512; i++)
    {
        alphaBuffer[i] = pSrc[i * 4];
    }

    glGenTextures(1, &g_fontTexture);
    glBindTexture(GL_TEXTURE_2D, g_fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, alphaBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(alphaBuffer);
    DeleteObject(hFont);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);

    // 3. Text VAO & Dynamic VBO
    glGenVertexArrays(1, &g_textVAO);
    glGenBuffers(1, &g_textVBO);

    glBindVertexArray(g_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4 * 128, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return TRUE;
}

float getTextWidth(const char* text, float scale)
{
    float w = 0.0f;
    for (const char* p = text; *p; p++)
    {
        w += g_characters[(unsigned char)*p].width * scale;
    }
    return w;
}

void renderText2D(const char* text, float x, float y, float scale, float r, float g, float b, float a)
{
    glUseProgram(g_textShaderProgram);

    mat4 ortho = vmath::ortho(0.0f, (float)g_winWidth, 0.0f, (float)g_winHeight, -1.0f, 1.0f);
    glUniformMatrix4fv(g_textOrthoUniform, 1, GL_FALSE, ortho);
    glUniform4f(g_textColorUniform, r, g, b, a);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_fontTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(g_textVAO);

    GLfloat vertices[512 * 24];
    int vertIndex = 0;
    float curX = x;
    float charW = 32.0f * scale;
    float charH = 32.0f * scale;

    for (const char* p = text; *p; p++)
    {
        unsigned char c = (unsigned char)*p;
        CharacterInfo ch = g_characters[c];

        float x0 = curX;
        float y0 = y;
        float x1 = curX + charW;
        float y1 = y + charH;

        // Quad Triangles (2D Screen Coordinates)
        vertices[vertIndex++] = x0; vertices[vertIndex++] = y0; vertices[vertIndex++] = ch.u0; vertices[vertIndex++] = ch.v1;
        vertices[vertIndex++] = x1; vertices[vertIndex++] = y0; vertices[vertIndex++] = ch.u1; vertices[vertIndex++] = ch.v1;
        vertices[vertIndex++] = x0; vertices[vertIndex++] = y1; vertices[vertIndex++] = ch.u0; vertices[vertIndex++] = ch.v0;

        vertices[vertIndex++] = x1; vertices[vertIndex++] = y0; vertices[vertIndex++] = ch.u1; vertices[vertIndex++] = ch.v1;
        vertices[vertIndex++] = x1; vertices[vertIndex++] = y1; vertices[vertIndex++] = ch.u1; vertices[vertIndex++] = ch.v0;
        vertices[vertIndex++] = x0; vertices[vertIndex++] = y1; vertices[vertIndex++] = ch.u0; vertices[vertIndex++] = ch.v0;

        curX += ch.width * scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertIndex * sizeof(GLfloat), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, vertIndex / 4);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdLine, int iCmdShow)
{
    int initialize(void);
    void resize(int, int);
    void display(void);
    void update(void);
    void uninitialize(void);

    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("RTR6");
    BOOL bDone = FALSE;

    gpFile = fopen(gszLogFileName, "w");
    if (gpFile == NULL)
    {
        MessageBox(NULL, TEXT("Log file creation FAILED!!!"), TEXT("File I/O error"), MB_OK);
        exit(0);
    }
    else
    {
        fprintf(gpFile, "Programme Started Successfully!\n");
    }

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

    RegisterClassEx(&wndclass);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                    szAppName,
                    TEXT("OpenGL Window"),
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                    GetSystemMetrics(SM_CXSCREEN)/2 - WIN_WIDTH/2,
                    GetSystemMetrics(SM_CYSCREEN)/2 - WIN_HEIGHT/2,
                    WIN_WIDTH,
                    WIN_HEIGHT,
                    NULL,
                    NULL,
                    hInstance,
                    NULL
                );

    ghwnd = hwnd;

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    int result = initialize();
    if (result != 0)
    {
        fprintf(gpFile, "Initialize() FAILED!\n");
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    else
    {
        fprintf(gpFile, "Initialize() Completed Successfully.\n");
    }

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    while (bDone == FALSE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            if (msg.message == WM_QUIT)
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
            if (gbActiveWindow == TRUE)
            {
                if (gbEscapeKeyIsPressed == TRUE)
                {
                    bDone = TRUE;
                }

                display();
                update();
            }
        }
    }

    uninitialize();
    return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    void toggleFullScreen(void);
    void resize(int, int); 
    void uninitialize(void); 

    switch (iMsg)
    {
    case WM_CREATE:
        ZeroMemory((void *)&wpPrev, sizeof(WINDOWPLACEMENT)); 
        wpPrev.length = sizeof(WINDOWPLACEMENT);              
        break;

    case WM_SETFOCUS:
        gbActiveWindow = TRUE;
        break;

    case WM_KILLFOCUS:
        gbActiveWindow = FALSE;
        break;

    case WM_ERASEBKGND: 
        return(0);
    
    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE: 
            gbEscapeKeyIsPressed = TRUE;
            break;

        default:
            break;
        }
        break;

    case WM_CHAR:
        switch (wParam)
        {
        case 'F':
        case 'f':
            if (gbFullScreen == FALSE)
            {
                toggleFullScreen();
                gbFullScreen = TRUE;
            }
            else
            {
                toggleFullScreen();
                gbFullScreen = FALSE;
            }
            break;

        case 'C':
        case 'c':
            g_hudMode = HUD_SHOW_CPU;
            break;

        case 'G':
        case 'g':
            g_hudMode = HUD_SHOW_GPU;
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

void toggleFullScreen(void)
{
    MONITORINFO mi;

    if (gbFullScreen == FALSE)
    {
        dwstyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwstyle & WS_OVERLAPPEDWINDOW)
        {
            ZeroMemory((void *)&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }
    else
    {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPED);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int initialize(void)
{
    void resize(int, int);
    void printGLInfo(void); 
    void uninitialize(void); 

    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    GLenum glewResult; 

    ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32; 
    
    ghdc = GetDC(ghwnd);
    if (ghdc == NULL) return(-1);

    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if (iPixelFormatIndex == 0) return(-2);

    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) return(-3);

    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL) return(-4);

    if (wglMakeCurrent(ghdc, ghrc) == FALSE) return(-5);

    glewResult = glewInit(); 
    if(glewResult != GLEW_OK) return (-6); 

    printGLInfo(); 
    queryCPUName();
    queryGPUName();

    // Vertex Shader 
    const GLchar* vertexShaderSourceCode = 
    "#version 460 core\n" \
    "in vec4 aPosition;\n" \
    "in vec4 aColor;\n" \
    "out vec4 out_color;\n" \
    "uniform mat4 uMVPMatrix;\n" \
    "void main(void) \n" \
    "{\n" \
    "gl_Position = uMVPMatrix * aPosition;\n" \
    "out_color = aColor;\n" \
    "}\n"; 

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL); 
    glCompileShader(vertexShaderObject); 

    // Fragment Shader 
    const char* fragmentShaderSourceCode = 
    "#version 460 core\n" \
    "in vec4 out_color;\n" \
    "out vec4 FragColor;\n" \
    "void main(void)\n" \
    "{\n" \
    "FragColor = out_color;\n" \
    "}\n"; 

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL); 
    glCompileShader(fragmentShaderObject); 

    // Shader Program Object 
    shaderProgramObject = glCreateProgram(); 
    glAttachShader(shaderProgramObject, vertexShaderObject); 
    glAttachShader(shaderProgramObject, fragmentShaderObject); 

    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition"); 
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor"); 

    glLinkProgram(shaderProgramObject); 

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix"); 

    const GLfloat triangle_position[] = 
    {
        0.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f, 
        1.0f, -1.0f, 0.0f 
    }; 

    const GLfloat triangle_color[] = 
    {
        1.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 1.0f  
    }; 

    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao); 
    
    // Position
    glGenBuffers(1, &vbo_position); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW); 
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // Color 
    glGenBuffers(1, &vbo_color); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW); 
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 

    glClearDepth(1.0f); 
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL); 

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity(); 

    // Initialize Programmable Font Engine
    initFontEngine(ghdc);

    resize(WIN_WIDTH, WIN_HEIGHT);
    return (0);
}

void printGLInfo(void) 
{
    fprintf(gpFile, "OpenGL Vendor = %s\n", glGetString(GL_VENDOR)); 
    fprintf(gpFile, "OpenGL Renderer = %s\n", glGetString(GL_RENDERER));  
    fprintf(gpFile, "OpenGL Version = %s\n", glGetString(GL_VERSION)); 
    fprintf(gpFile, "GLSL Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); 
} 

void resize(int width, int height)
{
    if(height <= 0) height = 1;

    g_winWidth = width;
    g_winHeight = height;

    glViewport(0, 0, (GLsizei)(width), (GLsizei)(height)); 

    perspectiveProjectionMatrix = vmath::perspective(
        45.0f, 
        (GLfloat)width/(GLfloat)height, 
        0.1f, 
        100.f  
    ); 
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Render 3D Scene
    glUseProgram(shaderProgramObject); 

    mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f); 
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  
    
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);  

    glBindVertexArray(vao); 
    glDrawArrays(GL_TRIANGLES, 0, 3); 
    glBindVertexArray(0); 

    glUseProgram(0); 

    // 2. Render Programmable HUD Overlay (Top Right Corner)
    float fps = calculateFPS();
    float fontScale = 1.0f; // Scale font size dynamically

    char fpsStr[64];
    char infoStr[256];

    snprintf(fpsStr, sizeof(fpsStr), "FPS: %.1f", fps);

    if (g_hudMode == HUD_SHOW_CPU)
    {
        snprintf(infoStr, sizeof(infoStr), "CPU: %s", g_cpuName);
    }
    else if (g_hudMode == HUD_SHOW_GPU)
    {
        snprintf(infoStr, sizeof(infoStr), "GPU: %s", g_gpuName);
    }

    float fpsWidth = getTextWidth(fpsStr, fontScale);
    float infoWidth = getTextWidth(infoStr, fontScale);

    float padding = 20.0f;
    float fpsX = (float)g_winWidth - fpsWidth - padding;
    float fpsY = (float)g_winHeight - (35.0f * fontScale);

    float infoX = (float)g_winWidth - infoWidth - padding;
    float infoY = (float)g_winHeight - (65.0f * fontScale);

    // Render HUD: Green for FPS, Cyan for Hardware
    renderText2D(fpsStr, fpsX, fpsY, fontScale, 0.2f, 1.0f, 0.2f, 1.0f);
    renderText2D(infoStr, infoX, infoY, fontScale, 0.0f, 0.9f, 1.0f, 1.0f);

    SwapBuffers(ghdc);
}

void update(void)
{
}

void uninitialize(void)
{
    if (gbFullScreen == TRUE)
    {
        toggleFullScreen();
    }
    gbFullScreen = FALSE;

    // Clean up Font Resources
    if (g_fontTexture) { glDeleteTextures(1, &g_fontTexture); g_fontTexture = 0; }
    if (g_textVBO) { glDeleteBuffers(1, &g_textVBO); g_textVBO = 0; }
    if (g_textVAO) { glDeleteVertexArrays(1, &g_textVAO); g_textVAO = 0; }
    if (g_textShaderProgram) { glDeleteProgram(g_textShaderProgram); g_textShaderProgram = 0; }

    if(vbo_color) { glDeleteBuffers(1, &vbo_color); vbo_color = 0; }
    if(vbo_position) { glDeleteBuffers(1, &vbo_position); vbo_position = 0; }
    if(vao) { glDeleteVertexArrays(1, &vao); vao = 0; }

    if(shaderProgramObject) 
    {
        glUseProgram(shaderProgramObject); 
        GLint numShaders; 
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders); 
        if(numShaders > 0) 
        {
            GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint)); 
            if(pShaders != NULL) 
            {
                glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
                for(GLint i = 0; i < numShaders; ++i) 
                {
                    glDetachShader(shaderProgramObject, pShaders[i]); 
                    glDeleteShader(pShaders[i]); 
                    pShaders[i] = 0; 
                }
                free(pShaders); 
                pShaders = 0; 
            }
        }
        glUseProgram(0); 
        glDeleteProgram(shaderProgramObject);   
    }

    if (wglGetCurrentContext() == ghrc) wglMakeCurrent(NULL, NULL);
    if (ghrc) { wglDeleteContext(ghrc); ghrc = NULL; }
    if (ghdc) { ReleaseDC(ghwnd, ghdc); ghdc = NULL; }
    if (ghwnd) { DestroyWindow(ghwnd); ghwnd = NULL; }

    if (gpFile)
    {
        fprintf(gpFile, "Programme Terminated Successfully.");
        fclose(gpFile);
        gpFile = NULL;
    }
}

