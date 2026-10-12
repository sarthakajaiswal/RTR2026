// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.hpp" 

// OpenGL header files
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

// Custom heander files
#include "OGL.h"
#include "imgui_manager.hpp" 

#include "vmath.h" 
using namespace vmath; 

//OpenGL libraries
#pragma comment(lib, "glew32.lib") 
#pragma comment(lib, "opengl32.lib") 

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define SHADOW_WIDTH	512 
#define SHADOW_HEIGHT	512 

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev;

int gWinWidth, gWinHeight; 

//OpenGL related variables
HDC ghdc = NULL; 
HGLRC ghrc = NULL; 

// shader related variables 
GLuint shaderProgramObject; 
GLuint lightProgram; 

enum 
{
	AMC_ATTRIBUTE_POSITION = 0, 
	AMC_ATTRIBUTE_COLOR, 
	AMC_ATTRIBUTE_NORMAL, 
	AMC_ATTRIBUTE_TEXCOORD
}; 

GLuint vao = 0; 
GLuint vbo_position = 0; 
GLuint vbo_color = 0; 

GLuint mvpMatrixUniform = 0; 

mat4 perspectiveProjectionMatrix; 

// variables related to File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// Active Window related variables
BOOL gbActiveWindow = FALSE;

// Exit keypress related
BOOL gbEscapeKeyIsPressed = FALSE;

// triangle color related variables 
float topColor[4] = {1.0f, 0.0f, 0.0f, 1.0f}; 
float leftBottomColor[4] = {0.0f, 1.0f, 0.0f, 1.0f}; 
float rightBottomColor[4] = {0.0f, 0.0f, 1.0f, 1.0f}; 

Camera camera; 

GLuint vao_cube, vbo_cube;
GLuint vao_pyramid;
GLuint vbo_pyramid; 

float pyramidX, pyramidY = 3.0f, pyramidZ = -3.0f, pyramidAngle = 0.0f; 
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // white  
GLfloat materialDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f}; 
float lightPosition[4] = {5.0f, 5.0f, 5.0f, 1.0f}; 

// Shadow mapping variables
GLuint depthMapFBO = 0;
GLuint depthMapTexture = 0;
GLuint simpleDepthProgram = 0;

// Entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function declarations
	int initialize(void);
	void resize(int, int);

	void display(void);
	void update(void);
	void uninitialize(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");
	BOOL bDone = FALSE;

	// code
	// Create Log file
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
					TEXT("Sarthak Ayodhyaprasad Jaiswal"),
					WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
					GetSystemMetrics(SM_CXSCREEN)/2 - WIN_WIDTH/2,  // X co-ordintate of window
					GetSystemMetrics(SM_CYSCREEN)/2 - WIN_HEIGHT/2, // Y co-ordintate of window
					WIN_WIDTH,	 // Width of window
					WIN_HEIGHT,	 // Height of window
					NULL,
					NULL,
					hInstance,
					NULL
				);

	ghwnd = hwnd;

	// show window
	ShowWindow(hwnd, iCmdShow);

	// Paint background of window
	UpdateWindow(hwnd);

	// Initialize
	int result = initialize();
	if (result != 0)
	{
		fprintf(gpFile, "Initialize() FAILED! \n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fprintf(gpFile, "Initialize() Completed Successfully.\n");
	}

	// Set this window as Foreground & Active window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop
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

				// render
				display();

				// update
				update();
			}
		}
	}

	// Uninitialize
	uninitialize();

	return ((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration 
	void toggleFullScreen(void);
	void resize(int, int); 
	void uninitialize(void); 

	// code
	// 1. Send input to Dear ImGui handler
    if (ImGuiManager::IsEnabled() && ImGui_ImplWin32_WndProcHandler(hwnd, iMsg, wParam, lParam))
        return true;

    // 2. Prevent application interaction if ImGui captures mouse or keyboard
    if (ImGui::GetCurrentContext())
    {
        ImGuiIO& io = ImGui::GetIO();

        // Block mouse events if clicking/hovering over ImGui
        if (io.WantCaptureMouse && (iMsg == WM_LBUTTONDOWN || iMsg == WM_LBUTTONUP ||
                                    iMsg == WM_RBUTTONDOWN || iMsg == WM_RBUTTONUP ||
                                    iMsg == WM_MBUTTONDOWN || iMsg == WM_MBUTTONUP ||
                                    iMsg == WM_MOUSEWHEEL  || iMsg == WM_MOUSEHWHEEL ||
                                    iMsg == WM_MOUSEMOVE))
        {
            return 0;
        }

        // Block keyboard events if typing inside an ImGui input widget
        if (io.WantCaptureKeyboard && (iMsg == WM_KEYDOWN || iMsg == WM_KEYUP || iMsg == WM_CHAR))
        {
            return 0;
        }
    }

	camera.cameraCallback(hwnd, iMsg, wParam, lParam); 

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
	// variable declarations
	MONITORINFO mi;

	// code
	if (gbFullScreen == FALSE)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			ZeroMemory((void *)&mi, sizeof(MONITORINFO));
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW); // '~' removes or negates value
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		// ShowCursor(FALSE); // Optional step : If upon Full Screen, should the cursor be displayed
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPED); // '|' adds style , '~' removes style
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		
		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// function declarations 
	void resize(int, int);
	void printGLInfo(void); 
	void uninitialize(void);
	
	BOOL createAndPrepareDepthMapFBO(GLuint textureWidth, GLuint textureHeight, GLuint* pfbo, GLuint* pfboDepthTexture); 
	void initColoredObjectProgram(void); 
	void initLightProgram(void) ; 
	void initTriangle(void); 
	void initCube(void); 
	void initPyramid(void); 
	void initLightProgram(void); 
	void initShadowProgram(void); 

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	GLenum glewResult; 

	// code
	// Pixel Format Descriptor initialisation
	ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1; // FFP in Windows, expects this to be 1 by default
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32; 
	
	// getdc
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "GetDC() function FAILED! \n");
		return(-1);
	}

	// Get matching Pixel Format Index using hdc & pfd
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() FAILED! \n");
		return(-2);
	}

	// Select the Pixel Format of found index
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() FAILED! \n");
		return(-3);
	}

	// Create Rendering Context using hdc, pfd & iPixelFormatIndex
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() FAILED! \n");
		return(-4);
	}

	// Make this rendering context as current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() FAILED! \n");
		return(-5);
	}

	// initialize glew 
	glewResult = glewInit(); 
	if(glewResult != GLEW_OK) 
	{
		fprintf(gpFile, "glewInit() failed\n"); 
		return (-6); 
	} 
	
	initColoredObjectProgram(); 
	initLightProgram(); 
	initShadowProgram(); 
	initTriangle(); 
	initCube(); 
	initPyramid(); 
	
	// depth related code 
	glClearDepth(1.0f); 
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); 

	// Instruct OpenGl to choose the colour to clear the screen 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// initialize ImGUI 
	if (ImGuiManager::IsEnabled() && !ImGuiManager::Initialize(ghwnd))
	{
		fprintf(gpFile, "ImGui initialization failed\n");
		return (-1);
	}

	perspectiveProjectionMatrix = mat4::identity();  

	camera.setPosition(vmath::vec3(5.63, 2.50, 12.96)); 
	camera.setYaw(-107.50); 
	camera.setPitch(-14.20); 

	// Warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
		
	return (0);
}

BOOL createAndPrepareDepthMapFBO(GLuint textureWidth, GLuint textureHeight, GLuint* pfbo, GLuint* pfboDepthTexture) 
{
    GLint maxRenderBufferSize; 

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize); 
    if(maxRenderBufferSize < (GLint)textureWidth || maxRenderBufferSize < (GLint)textureHeight) 
    {
        fprintf(gpFile, "FBO width or height are exceeding max. render buffer size\n"); 
        return (FALSE); 
    } 

    glGenFramebuffers(1, pfbo); 
    glBindFramebuffer(GL_FRAMEBUFFER, *pfbo); 

    glGenTextures(1, pfboDepthTexture); 
    glBindTexture(GL_TEXTURE_2D, *pfboDepthTexture); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *pfboDepthTexture, 0); 
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
    {
        fprintf(gpFile, "FBO creation incomplete\n"); 
        return (FALSE);     
    } 
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    return (TRUE); 
}

void initShadowProgram(void)
{
    void uninitialize(void);
    BOOL createAndPrepareDepthMapFBO(GLuint, GLuint, GLuint*, GLuint*);

    // --- Vertex Shader ---
    const GLchar* depthVertexShaderSource =
        "#version 460 core\n"
        "in vec4 aPosition;\n"
        "uniform mat4 uLightSpaceMatrix;\n"
        "uniform mat4 uModel;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = uLightSpaceMatrix * uModel * aPosition;\n"
        "}\n";

    GLuint depthVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(depthVS, 1, &depthVertexShaderSource, NULL);
    glCompileShader(depthVS);

    // --- Fragment Shader ---
    const GLchar* depthFragmentShaderSource =
        "#version 460 core\n"
        "void main()\n"
        "{\n"
        "}\n";

    GLuint depthFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(depthFS, 1, &depthFragmentShaderSource, NULL);
    glCompileShader(depthFS);

    // --- Shader Program ---
    simpleDepthProgram = glCreateProgram();
    glAttachShader(simpleDepthProgram, depthVS);
    glAttachShader(simpleDepthProgram, depthFS);
    glBindAttribLocation(simpleDepthProgram, AMC_ATTRIBUTE_POSITION, "aPosition");
    glLinkProgram(simpleDepthProgram);

    // Clean up individual shader objects post-link
    glDeleteShader(depthVS);
    glDeleteShader(depthFS);

    // --- Create Depth FBO ---
    if (createAndPrepareDepthMapFBO(1024, 1024, &depthMapFBO, &depthMapTexture) == FALSE)
    {
        fprintf(gpFile, "createAndPrepareDepthMapFBO() FAILED!\n");
        uninitialize();
    }
}

void initColoredObjectProgram(void) 
{
	// function declarations 
	void uninitialize(void); 

	// code 
	// ================= vertex shader ======================= 
	// 1) Write the shader source code 
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

	GLint status = 0; 
	GLint infoLogLength = 0; 
	GLchar* szInfoLog = NULL; 
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "vertex shader compilation log = %s\n", szInfoLog);  
			
				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		uninitialize(); 
	} 
	
	// ======================= FRAGMENT SHADER =========================== 
	const char* fragmentShaderSourceCode = 
	"#version 460 core\n" \
	"in vec4 out_color;\n" \
	"out vec4 FragColor;"
	"void main(void)\n" \
	"{\n" \
	"FragColor = out_color;\n" \
	"}\n"; 

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL); 
	glCompileShader(fragmentShaderObject); 

	status = 0; 
	infoLogLength = 0; 
	szInfoLog = NULL; 

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "Fragment shader compilation log = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		uninitialize(); 
	}

	// ================================= create, attach, link shader program object =========================== 
	// 1) Create shader program object 
	shaderProgramObject = glCreateProgram(); 

	// 2) attch shaders to this shader program object 
	glAttachShader(shaderProgramObject, vertexShaderObject); 

	// bind shader attribute at certain index in shader to save index in host program 
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition"); // address fixation 
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "out_color"); // address fixation 

	glAttachShader(shaderProgramObject, fragmentShaderObject); 

	// 3) tell to link shader objects to shader program objects 
	glLinkProgram(shaderProgramObject); 

	// 4) check for link error log 
	status = 0; 
	infoLogLength = 0; 
	szInfoLog = 0; 
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "shader program object = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		uninitialize(); 
	} 

	// ----- SENDING DATA INTO UNIFORM ---------- 
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix"); 
} 

void initLightProgram(void) 
{
    void uninitialize(void); 

    // ================= VERTEX SHADER =================
    const GLchar* vertexShaderSourceCode = 
    "#version 460 core\n"
    "in vec4 aPosition;\n"
    "in vec3 aNormal;\n"
    "\n"
    "out vec3 out_fragPos;\n"
    "out vec3 out_normal;\n"
    "out vec4 out_fragPosLightSpace;\n"
    "\n"
    "uniform mat4 uModel;\n"
    "uniform mat4 uModelViewMatrix;\n"
    "uniform mat4 uProjectionMatrix;\n"
    "uniform mat4 uLightSpaceMatrix;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    out_fragPos = vec3(uModel * aPosition);\n"
    "    out_normal = mat3(transpose(inverse(uModel))) * aNormal;\n"
    "    out_fragPosLightSpace = uLightSpaceMatrix * vec4(out_fragPos, 1.0);\n"
    "\n"
    "    gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;\n"
    "}\n";

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL); 
    glCompileShader(vertexShaderObject); 

    GLint status = 0; 
    GLint infoLogLength = 0; 
    GLchar* szInfoLog = NULL; 
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status); 
    if (status == GL_FALSE) 
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
        if (infoLogLength > 0) 
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
            if (szInfoLog != NULL) 
            {
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog); 
                fprintf(gpFile, "Light Vertex shader compilation log = %s\n", szInfoLog);  
                free(szInfoLog); 
                szInfoLog = NULL; 
            }
        }
        uninitialize(); 
    } 
    
    // ================= FRAGMENT SHADER =================
    const char* fragmentShaderSourceCode = 
    "#version 460 core\n"
    "in vec3 out_fragPos;\n"
    "in vec3 out_normal;\n"
    "in vec4 out_fragPosLightSpace;\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "uniform sampler2D shadowMap;\n"
    "uniform vec4 uLightPosition;\n"
    "uniform vec3 uLd;\n"
    "uniform vec3 uKd;\n"
    "uniform int uLKeyIsPressed;\n"
    "\n"
    "float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)\n"
    "{\n"
    "    // Perform perspective divide\n"
    "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
    "    // Transform to [0,1] range\n"
    "    projCoords = projCoords * 0.5 + 0.5;\n"
    "\n"
    "    if (projCoords.z > 1.0)\n"
    "        return 0.0;\n"
    "\n"
    "    // Sample depth from shadow map\n"
    "    float closestDepth = texture(shadowMap, projCoords.xy).r;\n"
    "    float currentDepth = projCoords.z;\n"
    "\n"
    "    // Calculate bias based on surface slope to eliminate shadow acne\n"
    "    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n"
    "\n"
    "    // Depth comparison\n"
    "    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;\n"
    "    return shadow;\n"
    "}\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    if (uLKeyIsPressed == 1)\n"
    "    {\n"
    "        vec3 norm = normalize(out_normal);\n"
    "        vec3 lightDir = normalize(vec3(uLightPosition) - out_fragPos);\n"
    "\n"
    "        // Ambient component\n"
    "        vec3 ambient = 0.2f * uKd;\n"
    "\n"
    "        // Diffuse component\n"
    "        float diff = max(dot(norm, lightDir), 0.0);\n"
    "        vec3 diffuse = uLd * uKd * diff;\n"
    "\n"
    "        // Shadow calculation\n"
    "        float shadow = CalculateShadow(out_fragPosLightSpace, norm, lightDir);\n"
    "\n"
    "        vec3 result = ambient + (1.0 - shadow) * diffuse;\n"
    "        FragColor = vec4(result, 1.0f);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        FragColor = vec4(uKd, 1.0f);\n"
    "    }\n"
    "}\n"; 

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL); 
    glCompileShader(fragmentShaderObject); 

    status = 0; 
    infoLogLength = 0; 
    szInfoLog = NULL; 

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status); 
    if (status == GL_FALSE) 
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength); 
        if (infoLogLength > 0) 
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
            if (szInfoLog != NULL) 
            {
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog); 
                fprintf(gpFile, "Light Fragment shader compilation log = %s\n", szInfoLog); 
                free(szInfoLog); 
                szInfoLog = NULL; 
            }
        }
        uninitialize(); 
    }

    // ================= CREATE & LINK PROGRAM =================
    lightProgram = glCreateProgram(); 

    glAttachShader(lightProgram, vertexShaderObject); 
    glAttachShader(lightProgram, fragmentShaderObject); 

    glBindAttribLocation(lightProgram, AMC_ATTRIBUTE_POSITION, "aPosition"); 
    glBindAttribLocation(lightProgram, AMC_ATTRIBUTE_NORMAL, "aNormal"); 

    glLinkProgram(lightProgram); 

    status = 0; 
    infoLogLength = 0; 
    szInfoLog = 0; 
    glGetProgramiv(lightProgram, GL_LINK_STATUS, &status); 
    if (status == GL_FALSE) 
    {
        glGetProgramiv(lightProgram, GL_INFO_LOG_LENGTH, &infoLogLength); 
        if (infoLogLength > 0) 
        {
            szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
            if (szInfoLog != NULL) 
            {
                glGetProgramInfoLog(lightProgram, infoLogLength, NULL, szInfoLog); 
                fprintf(gpFile, "Light shader program link log = %s\n", szInfoLog); 
                free(szInfoLog); 
                szInfoLog = NULL; 
            }
        }
        uninitialize(); 
    }

    // Free individual shader objects post-linking
    glDetachShader(lightProgram, vertexShaderObject);
    glDetachShader(lightProgram, fragmentShaderObject);
    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);
}

void initTriangle(void) 
{
	// provide vertex position, color, normal, texCoord, etc... 
	const GLfloat triangle_position[] = 
	{
		0.0f, 1.0f, 0.0f, 
		-1.0f, -1.0f, 0.0f, 
		1.0f, -1.0f, 0.0f 
	}; 

	// vertex array object for arrays of vertex attributes 
	glGenVertexArrays(1, &vao); // vao-vertex array object 
	glBindVertexArray(vao); 
	
	// position  
	glGenBuffers(1, &vbo_position); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW); 
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// color 
	glGenBuffers(1, &vbo_color); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*3, NULL, GL_DYNAMIC_DRAW); 
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 
} 

void initCube(void) 
{
	const GLfloat cube_PCNT[] =
	{
		// front
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
							
		// right			 
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
							
		// back				 
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
							
		// left				 
		// position				// color			 // normals				// texcoords
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
							
		// top				 
		// position				// color			 // normals				// texcoords
		1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
							
		// bottom			 
		// position				// color			 // normals				// texcoords
		1.0f, -1.0f,  1.0f,		1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
	};

	// CUBE 
	glGenVertexArrays(1, &vao_cube); // vao-vertex array object 
	glBindVertexArray(vao_cube); 
	
	// create a common vbo for p, c, n, t 
	glGenBuffers(1, &vbo_cube); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_PCNT), cube_PCNT, GL_STATIC_DRAW); 

	// for position 
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(0 * sizeof(float))); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
	
	// for color 
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(3 * sizeof(float))); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); 

	// for normals 
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(6 * sizeof(float))); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL); 

	// for texCoords 
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(9*sizeof(float))); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD); 

	glBindVertexArray(0); 
} 

void initPyramid(void) 
{
    const GLfloat pyramid_PCNT[] =
    {
        // front
        // position          // color            // normals                          // texcoords
        0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.000000f, 0.447214f,  0.894427f,   0.5f, 1.0f,
       -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,   0.000000f, 0.447214f,  0.894427f,   0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,   0.000000f, 0.447214f,  0.894427f,   1.0f, 0.0f,
                                                                                 
        // right                                                                 
        // position          // color            // normals                          // texcoords
        0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.894427f, 0.447214f,  0.000000f,   0.5f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,   0.894427f, 0.447214f,  0.000000f,   1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.894427f, 0.447214f,  0.000000f,   0.0f, 0.0f,

        // back              
        // position          // color            // normals                          // texcoords
        0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,   0.000000f, 0.447214f, -0.894427f,   0.5f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.000000f, 0.447214f, -0.894427f,   0.0f, 0.0f,
       -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,   0.000000f, 0.447214f, -0.894427f,   1.0f, 0.0f,

        // left
        // position          // color            // normals                          // texcoords
        0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,  -0.894427f, 0.447214f,  0.000000f,   0.5f, 1.0f,
       -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,  -0.894427f, 0.447214f,  0.000000f,   1.0f, 0.0f,
       -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  -0.894427f, 0.447214f,  0.000000f,   0.0f, 0.0f,
    };

    // PYRAMID VAO Setup
    glGenVertexArrays(1, &vao_pyramid); 
    glBindVertexArray(vao_pyramid); 
    
    // Create common VBO for P, C, N, T
    glGenBuffers(1, &vbo_pyramid); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_PCNT), pyramid_PCNT, GL_STATIC_DRAW); 

    // Position Attribute Pointer (3 floats)
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float))); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
    
    // Color Attribute Pointer (3 floats)
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); 

    // Normal Attribute Pointer (3 floats)
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL); 

    // Texture Coordinate Attribute Pointer (2 floats)
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float))); 
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD); 

    // Unbind VAO
    glBindVertexArray(0); 
}

void renderCube(void) 
{
	glBindVertexArray(vao_cube); 
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4); 
	glBindVertexArray(0); 
} 

void renderPyramid(void) 
{
    glBindVertexArray(vao_pyramid); 
    glDrawArrays(GL_TRIANGLES, 0, 12); 
    glBindVertexArray(0); 
}

void resize(int width, int height)
{
	// code
	// If height accidentally becomes 0 or less, then make it 1
	if(height <= 0)
	{
		height = 1;
	}

	// Set the View port
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height)); 

	gWinWidth = width, gWinHeight = height; 

	perspectiveProjectionMatrix = vmath::perspective(
		45.0f, // fov 
		(GLfloat)width/(GLfloat)height, // aspect ratio 
		0.1f, // near 
		100.f // far  
	); 
}

void renderScene(bool isDepthPass, mat4 lightSpaceMatrix)
{
    mat4 modelMatrix;

    if (isDepthPass)
    {
        glUseProgram(simpleDepthProgram);

        // --- Floor ---
        modelMatrix = vmath::scale(10.0f, 0.2f, 10.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uLightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        renderCube();

        // --- Back Wall ---
        modelMatrix = vmath::translate(0.0f, 0.0f, -5.0f) * vmath::scale(10.0f, 10.0f, -1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uLightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        renderCube();

        // --- Pyramid ---
        modelMatrix = vmath::translate(pyramidX, pyramidY, pyramidZ) * vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uLightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        renderPyramid();

        glUseProgram(0);
    }
    else
    {
        // --- Floor ---
        glUseProgram(lightProgram);
        modelMatrix = vmath::scale(10.0f, 0.2f, 10.0f);
        mat4 mvMatrix = camera.getViewMatrix() * modelMatrix;
        
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModelViewMatrix"), 1, GL_FALSE, mvMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uProjectionMatrix"), 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uLightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        
        glUniform3fv(glGetUniformLocation(lightProgram, "uLd"), 1, lightDiffuse);
        glUniform3fv(glGetUniformLocation(lightProgram, "uKd"), 1, materialDiffuse);
        glUniform4fv(glGetUniformLocation(lightProgram, "uLightPosition"), 1, lightPosition);
        glUniform1i(glGetUniformLocation(lightProgram, "uLKeyIsPressed"), 1);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(lightProgram, "shadowMap"), 0);

        renderCube();

        // --- Back Wall ---
        modelMatrix = vmath::translate(0.0f, 0.0f, -5.0f) * vmath::scale(10.0f, 10.0f, -1.0f);
        mvMatrix = camera.getViewMatrix() * modelMatrix;
        
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModelViewMatrix"), 1, GL_FALSE, mvMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        
        renderCube();

        // --- Pyramid ---
        modelMatrix = vmath::translate(pyramidX, pyramidY, pyramidZ) * vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
        mvMatrix = camera.getViewMatrix() * modelMatrix;
        
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModelViewMatrix"), 1, GL_FALSE, mvMatrix);
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "uModel"), 1, GL_FALSE, modelMatrix);
        
        renderPyramid();
        glUseProgram(0);

        // --- Light Bulb Indicator ---
        glUseProgram(shaderProgramObject);
        modelMatrix = vmath::translate(lightPosition[0], lightPosition[1], lightPosition[2]) * vmath::scale(0.1f, 0.1f, 0.1f);
        mat4 mvpMatrix = perspectiveProjectionMatrix * camera.getViewMatrix() * modelMatrix;
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix);
        renderCube();
        glUseProgram(0);
    }
}

void display(void)
{
	// function declarations 
	void renderTriangle(void); 

	// code
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ImGuiManager::IsEnabled())
	{
		// Start new ImGui frame
		ImGuiManager::NewFrame();
	} 

	// 1. Calculate Light Projection & View Matrices
    mat4 lightProjection = vmath::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    mat4 lightView = vmath::lookat(vec3(lightPosition[0], lightPosition[1], lightPosition[2]),
                                   vec3(0.0f, 0.0f, 0.0f),
                                   vec3(0.0f, 1.0f, 0.0f));
    mat4 lightSpaceMatrix = lightProjection * lightView;

    // 2. PASS 1: Render Shadow Map (Depth Pass)
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderScene(true, lightSpaceMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. PASS 2: Render Normal Scene (Color Pass)
    resize(gWinWidth, gWinHeight); // Restore viewport dimensions
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene(false, lightSpaceMatrix);

	// Render ImGui debug window and other UI elements
	if (ImGuiManager::IsEnabled())
	{
		ImGuiManager::RenderDebugWindow();
		ImGuiManager::Render();
	}

	// Swap the buffers
	SwapBuffers(ghdc);
}

void renderTriangle(void) 
{
	glBindVertexArray(vao); 
	glDrawArrays(GL_TRIANGLES, 0, 3); 
	glBindVertexArray(0); 
} 

void update(void)
{
	// code
}

void uninitialize(void)
{
	
	// function declarations
	void toggleFullScreen(void);
	void uninitializeTriangle(void); 
	void uninitializeProgram(GLuint* pShaderProgram); 

	// code
	// Cleanup ImGui
	if (ImGuiManager::IsEnabled())
	{
		ImGuiManager::Cleanup();
	}

	// If iser is exiting FullScreen, then restore screen/window
	if ( gbFullScreen == TRUE)
	{
		toggleFullScreen();
	}
	gbFullScreen = FALSE;

	uninitializeTriangle(); 
	uninitializeProgram(&shaderProgramObject); 

	// Make hdc as current context by releasing rendering context as current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	// Delete the rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	// Relese the DC
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// Destroy window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// close the file
	if (gpFile)
	{
		fprintf(gpFile, "Programme Terminated Successfully.");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void uninitializeTriangle(void) 
{
	// free vbo 
	if(vbo_color) 
	{
		glDeleteBuffers(1, &vbo_color); 
		vbo_color = 0; 
	}
	if(vbo_position) 
	{
		glDeleteBuffers(1, &vbo_position); 
		vbo_position = 0; 
	}

	// free vao 
	if(vao) 
	{
		glDeleteVertexArrays(1, &vao); 
		vao = 0; 
	}
} 

void uninitializeProgram(GLuint* pShaderProgram) 
{
	if(pShaderProgram) 
	{
		GLuint programObject = *pShaderProgram; 

		if(programObject) 
		{
			glUseProgram(programObject); 
			
			GLint numShaders; 
			glGetProgramiv(programObject, GL_ATTACHED_SHADERS, &numShaders); 
			if(numShaders > 0) 
			{
				GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint)); 
				if(pShaders != NULL) 
				{
					glGetAttachedShaders(programObject, numShaders, NULL, pShaders);
					
					for(GLint i = 0; i < numShaders; ++i) 
					{
						glDetachShader(programObject, pShaders[i]); 
						glDeleteShader(pShaders[i]); 
						pShaders[i] = 0; 
					}

					free(pShaders); 
					pShaders = 0; 
				}
			}

			glUseProgram(0); 
			glDeleteProgram(programObject);   
		}

		*pShaderProgram = 0; 
	} 
} 

