// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h> // For CPU Hardware Detection (__cpuid)
#include <iostream> 
#include <map> 

#include <ft2build.h> 
#include FT_FREETYPE_H 

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
GLuint fontShaderProgram; 

enum 
{
    AMC_ATTRIBUTE_POSITION = 0, 
    AMC_ATTRIBUTE_COLOR, 
}; 

GLuint vao = 0; 
GLuint vbo_position = 0; 
GLuint vbo_color = 0; 

GLuint textVAO, textVBO; 

GLuint mvpMatrixUniform = 0; 
mat4 perspectiveProjectionMatrix; 

// File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// Active Window & Exit variables
BOOL gbActiveWindow = FALSE;
BOOL gbEscapeKeyIsPressed = FALSE;

// font rendering related 
FT_Library  ft; 
FT_Face     face; 

struct Character 
{
    GLuint  textureID; 
    vmath::vec2 size; 
    vmath::vec2 bearing; 
    GLuint      advance; 
}; 

std::map<GLchar, Character> characters; 

void GetCPUName(char* buffer) 
{
    int cpuInfo[4] = { 0 };
    
    // Check if the CPU supports fetching the brand string
    __cpuid(cpuInfo, 0x80000000);
    unsigned int maxExtendedId = cpuInfo[0];

    if (maxExtendedId >= 0x80000004) {
        // Query extended CPUID leaves 0x80000002 to 0x80000004
        __cpuid((int*)(buffer),      0x80000002);
        __cpuid((int*)(buffer + 16), 0x80000003);
        __cpuid((int*)(buffer + 32), 0x80000004);
        buffer[48] = '\0';
    } else {
        snprintf(buffer, 49, "Unknown CPU");
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

    // ===================================================================================================================================
        // Vertex Shader 
    const GLchar* fontVSCode = 
    "#version 460 core\n" \
    "in vec4 aPosition;\n" \
    "out vec2 out_texCoords;\n" \

    "uniform mat4 uProjectionMatrix;\n" \
    
    "void main(void) \n" \
    "{\n" \
    "gl_Position = uProjectionMatrix * vec4(aPosition.xy, 0.0f, 1.0f);\n" \
    "out_texCoords = aPosition.zw;\n" \
    "}\n"; 

    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&fontVSCode, NULL); 
    glCompileShader(vertexShaderObject); 

    // Fragment Shader 
    const char* fontFSCode = 
    "#version 460 core\n" \
    
    "in vec2 out_texCoords;\n" \
    "out vec4 FragColor;\n" \

    "uniform sampler2D uText;\n" \
    "uniform vec3 uTextColor;\n" \

    "void main(void)\n"
    "{\n"
    "   vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(uText, out_texCoords).r);\n"
    "   FragColor = vec4(uTextColor, 1.0) * sampled;\n"
    "}\n";

    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fontFSCode, NULL); 
    glCompileShader(fragmentShaderObject); 

    // Shader Program Object 
    fontShaderProgram = glCreateProgram(); 
    glAttachShader(fontShaderProgram, vertexShaderObject); 
    glAttachShader(fontShaderProgram, fragmentShaderObject); 

    glBindAttribLocation(fontShaderProgram, AMC_ATTRIBUTE_POSITION, "aPosition"); 

    glLinkProgram(fontShaderProgram); 
    GLint status = 0;
    glGetProgramiv(fontShaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(fontShaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            char* szInfoLog = (char*)malloc(logLength);
            if (szInfoLog)
            {
                glGetProgramInfoLog(fontShaderProgram, logLength, NULL, szInfoLog);
                fprintf(gpFile, "Font Shader Link Log: %s\n", szInfoLog);
                free(szInfoLog);
            }
        }
        return (-8);
    }

    // ===================================================================================================================================

    glGenVertexArrays(1, &textVAO); 
    glGenBuffers(1, &textVBO); 

    glBindVertexArray(textVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, textVBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW); 
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // ===================================================================================================================================

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // free type related 
    if(FT_Init_FreeType(&ft)) 
    {
        fprintf(gpFile, "initialize() > FREETYPE: Could not init freetype library\n"); 
        fflush(gpFile); 
    }     

    if(FT_New_Face(ft, "arial.ttf", 0, &face)) 
    {
        fprintf(gpFile, "FREETYPE: failed to load font\n"); 
        fflush(gpFile);  
    } 

    FT_Set_Pixel_Sizes(face, 0, 100); 

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    for(GLubyte c = 0; c < 128; c++) 
    {
        // load characters glymph 
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) 
        {
            fprintf(gpFile, "initialize() > FREETYPE: failed to load glymph\n"); 
            fflush(gpFile); 
            if(c < 128)
                continue; 
        } 

        // generate texture 
        GLuint texture; 
        glGenTextures(1, &texture); 
        glBindTexture(GL_TEXTURE_2D, texture); 
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED, 
            face->glyph->bitmap.width, 
            face->glyph->bitmap.rows, 
            0, 
            GL_RED, 
            GL_UNSIGNED_BYTE, 
            face->glyph->bitmap.buffer 
        ); 

        // set texture options 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

        // now store charaters for later use 
        Character character = {
            texture, 
            vmath::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), 
            vmath::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top), 
            face->glyph->advance.x 
        }; 
        characters.insert(std::pair<GLchar, Character>(c, character)); 
    } 

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
    // function declarations 
    void RenderText(GLuint shaderProgram, const char* text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color); 

    // code 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject); 
    mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f); 
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);  
    glBindVertexArray(vao); 
    glDrawArrays(GL_TRIANGLES, 0, 3); 
    glBindVertexArray(0); 
    glUseProgram(0); 

    float fps = calculateFPS();

    char str[128]; 
    sprintf(str, "%.2f", fps); 
    RenderText(fontShaderProgram, str, 0, 0, 1.0f, vmath::vec3(1.0f, 1.0f, 1.0f));  

    SwapBuffers(ghdc);
}

void RenderText(GLuint shaderProgram, const char* text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color) 
{
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram); 
    glUniform3fv(glGetUniformLocation(shaderProgram, "uTextColor"), 1, color); 
    vmath::mat4 textProjectionMatrix = vmath::ortho(0.0f, g_winWidth, 0.0f, g_winHeight, -1.0f, 1.0f); 
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjectionMatrix"), 1, GL_FALSE, textProjectionMatrix); 

    glActiveTexture(GL_TEXTURE0); 
    glUniform1i(glGetUniformLocation(fontShaderProgram, "uText"), 0);
    glBindVertexArray(textVAO); 

    // iterate through all characters 
    int i = 0; 
    while(text[i] != '\0')
    { 
        Character ch = characters[text[i]]; 

        GLfloat xPos    = x + ch.bearing[0] * scale; 
        GLfloat yPos    = y - (ch.size[1] - ch.bearing[1]) * scale; 
        GLfloat w       = ch.size[0] * scale; 
        GLfloat h       = ch.size[1] * scale; 

        // update VBO for each character 
        GLfloat vertices[6][4] = 
        {
            {xPos,      yPos+h, 0.0f, 0.0f},  
            {xPos,      yPos,   0.0f, 1.0f},  
            {xPos+w,    yPos,   1.0f, 1.0f}, 

            {xPos,      yPos+h, 0.0f, 0.0f},  
            {xPos+w,    yPos,   1.0f, 1.0f},  
            {xPos+w,    yPos+h, 1.0f, 0.0f},  
        }; 

        // render glyph texture over quad 
        glBindTexture(GL_TEXTURE_2D, ch.textureID); 
        // update content of vbo memory 
        glBindBuffer(GL_ARRAY_BUFFER, textVBO); 
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        // render quad 
        glDrawArrays(GL_TRIANGLES, 0, 6); 

        // now advance cursor for next glyph 
        x += (ch.advance >> 6) * scale; 

        i += 1; 
    } 

    glBindVertexArray(0);  
    glBindTexture(GL_TEXTURE_2D, 0); 

    glEnable(GL_DEPTH_TEST);
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

    FT_Done_Face(face); 
    FT_Done_FreeType(ft); 

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

