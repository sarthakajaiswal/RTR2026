// Win32 headers
#include <windows.h>
#include <Windowsx.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <assert.h> 
#include <math.h> 
#include <iostream> 
#include <string> 

// OpenGL header files
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

// Custom heander files
#include "OGL.h"
#include "vmath.h" 
#include "globals.h" 
#include "E_shader.h" 
#include "E_camera.h" 
#include "E_utils.h" 
#include "imgui_manager.h" 
#define	STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

//OpenGL libraries
#pragma comment(lib, "glew32.lib") 
#pragma comment(lib, "opengl32.lib") 

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev;

//OpenGL related variables
HDC ghdc = NULL; 
HGLRC ghrc = NULL;  

// variables related to File I/O
char gszLogFileName[] = "output/Log.txt";
FILE *gpFile = NULL;

// Active Window related variables
BOOL gbActiveWindow = FALSE;

// Exit keypress related
BOOL gbEscapeKeyIsPressed = FALSE;

int gWinWidth=WIN_WIDTH, gWinHeight=WIN_HEIGHT; 

// shader related variables 
GLuint pbrShaderProgram = 0; 
GLuint lightShaderProgram = 0; 
GLuint skyboxShaderProgram = 0; 

GLuint lightVAO = 0; 
GLuint lightVBO = 0; 
GLuint objectVAO = 0; 
GLuint skyboxVAO, skyboxVBO = 0; 

E_camera camera; 

// lights
// ------
vmath::vec3 lightPositions[] = {
	vmath::vec3(-10.0f,  10.0f, 10.0f),
	vmath::vec3( 10.0f,  10.0f, 10.0f),
	vmath::vec3(-10.0f, -10.0f, 10.0f),
	vmath::vec3( 10.0f, -10.0f, 10.0f),
};
vmath::vec3 lightColors[] = {
	vmath::vec3(300.0f, 300.0f, 300.0f),
	vmath::vec3(300.0f, 300.0f, 300.0f),
	vmath::vec3(300.0f, 300.0f, 300.0f),
	vmath::vec3(300.0f, 300.0f, 300.0f)
};
int nrRows    = 7;
int nrColumns = 7;
float spacing = 2.5;

GLuint texture_albedoMap; 
GLuint texture_normalMap; 
GLuint texture_roughnessMap; 
GLuint texture_metallicMap; 
GLuint texture_aoMap; 

GLuint texture_skybox; 
GLuint texture_irrandianceMap; 

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
	fprintf(gpFile, "result = %d\n", result); 
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
	// imgui 
	if (ImGuiManager::initialized)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, iMsg, wParam, lParam))
			return true; // Event consumed directly by ImGui internal handler
	}
	bool imguiWantsMouse    = false;
	bool imguiWantsKeyboard = false;
	if (ImGuiManager::initialized)
	{
		ImGuiIO& io = ImGui::GetIO();
		imguiWantsMouse    = io.WantCaptureMouse;
		imguiWantsKeyboard = io.WantCaptureKeyboard;
	}
	bool isMouseMessage = (iMsg >= WM_MOUSEFIRST && iMsg <= WM_MOUSELAST);
	if (imguiWantsMouse && isMouseMessage)
	{
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
	}
	bool isKeyboardMessage = (iMsg >= WM_KEYFIRST && iMsg <= WM_KEYLAST);
	if (imguiWantsKeyboard && isKeyboardMessage)
	{
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
	}

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

    case WM_MOUSEMOVE:
        xpos = GET_X_LPARAM(lParam);
        ypos = GET_Y_LPARAM(lParam);
        if (wParam & MK_LBUTTON)
        {
            camera.mouse_callback(xpos, ypos);
        }
        else
        {
            lastX = xpos;
            lastY = ypos;
        }
	    break;

	case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE: gbEscapeKeyIsPressed = TRUE; break;
        case 'W': keyW = true; break;
        case 'A': keyA = true; break;
        case 'S': keyS = true; break;
        case 'D': keyD = true; break;
        case 'Z': keyZ = true; break;
        case 'X': keyX = true; break;
        default: break;
        }
        break;
    case WM_KEYUP:
        switch (wParam)
        {
        case 'W': keyW = false; break;
        case 'A': keyA = false; break;
        case 'S': keyS = false; break;
        case 'D': keyD = false; break;
        case 'Z': keyZ = false; break;
        case 'X': keyX = false; break;
        default: break;
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
		/*
		SWP_NOMOVE - Do not move as SetWindowPlacement as already set the position
		SWP_NOSIZE - Do not resize
		SWP_NOOWNERZORDER - Do not let Owner make changes
		*/

		// ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// function declarations 
	void resize(int, int);
	void printGLInfo(void); 
	void uninitialize(void); 

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

	pbrShaderProgram = createShaderFromSourceFiles("shaders/pbr.vs", "shaders/PBR.fs"); 
	if(pbrShaderProgram & 80000000) 
	{
		fprintf(gpFile, "initialize() > failed to create object shader prgoram\n"); 
		return (-11); 
	} 
	lightShaderProgram = createShaderFromSourceFiles("shaders/light.vs", "shaders/light.fs"); 
	if(lightShaderProgram & 80000000) 
	{
		fprintf(gpFile, "initialize() > failed to create light shader prgoram\n"); 
		return (-12); 
	} 
	skyboxShaderProgram = createShaderFromSourceFiles("shaders/skybox.vs", "shaders/skybox.fs"); 
	if(skyboxShaderProgram & 80000000) 
	{
		fprintf(gpFile, "initialize() > failed to create skybox shader prgoram\n"); 
		return (-12); 
	} 

    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  	0.0f,  0.0f,
		
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  	0.0f,  0.0f,
		
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  	1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
		
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  	1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  	0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  	1.0f,  0.0f,
		
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  	0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  	1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  	1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  	0.0f,  1.0f,
		
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  	0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  	1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  	1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  	1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  	0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  	0.0f,  1.0f
    };

	// light 
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO); 
	
	glGenBuffers(1, &lightVBO); 
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); 
    glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
	glBindVertexArray(0); 

	// object 
    glGenVertexArrays(1, &objectVAO);
    glBindVertexArray(objectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
    glVertexAttribPointer(SJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(SJ_ATTRIBUTE_NORMAL);
    glVertexAttribPointer(SJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(SJ_ATTRIBUTE_TEXTURE0);
	glBindVertexArray(0); 

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox 
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
    glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	

	camera.pos = vmath::vec3(0.0f, 2.0f, 10.0f);

	loadTextureUsingSTBI(&texture_albedoMap, "textures/albedo.png"); 
	loadTextureUsingSTBI(&texture_normalMap, "textures/normal.png"); 
	loadTextureUsingSTBI(&texture_roughnessMap, "textures/roughness.png"); 
	loadTextureUsingSTBI(&texture_metallicMap, "textures/metallic.png"); 
	loadTextureUsingSTBI(&texture_aoMap, "textures/ao.png"); 
	
	texture_skybox = convertEquirectangularMapIntoCubemap("textures/rainforest.hdr"); 
	texture_irrandianceMap = createIrradianceMapFromCubemap(texture_skybox); 

	// depth related code 
	glClearDepth(1.0f); 
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); 
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (ImGuiManager::IsEnabled() && !ImGuiManager::Initialize(ghwnd))
	{
		fprintf(gpFile, "ImGui initialization failed\n");
		return (-21); 
	}

	// Warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
		
	return (0);
}

void resize(int width, int height)
{
	// code
	// If height accidentally becomes 0 or less, then make it 1
	if(height <= 0)
	{
		height = 1;
	}

	gWinWidth = width; 
	gWinHeight = height; 

	// Set the View port
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height)); 

	camera.resize(width, height); 
}

void display(void)
{
	// function declarations 
	void renderSphere(); 
	float clamp(float, float, float); 

	// code 	
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ImGuiManager::IsEnabled())
	{
		// Start new ImGui frame
		ImGuiManager::NewFrame();
	}

	// -------------- objects: spheres --------------
	vmath::mat4 modelMatrix = vmath::mat4::identity(); 

	glUseProgram(pbrShaderProgram); 

	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightPositions[0]"), 1, lightPositions[0]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightPositions[1]"), 1, lightPositions[1]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightPositions[2]"), 1, lightPositions[2]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightPositions[3]"), 1, lightPositions[3]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightColors[0]"), 1, lightColors[0]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightColors[1]"), 1, lightColors[1]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightColors[2]"), 1, lightColors[2]); 
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_lightColors[3]"), 1, lightColors[3]); 

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, texture_albedoMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_albedoMap"), 0); 
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, texture_normalMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_normalMap"), 1); 
	glActiveTexture(GL_TEXTURE2); 
	glBindTexture(GL_TEXTURE_2D, texture_roughnessMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_roughnessMap"), 2); 
	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_2D, texture_aoMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_aoMap"), 3); 
	glActiveTexture(GL_TEXTURE4); 
	glBindTexture(GL_TEXTURE_2D, texture_metallicMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_metallicMap"), 4); 
	glActiveTexture(GL_TEXTURE5); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_irrandianceMap); 
	glUniform1i(glGetUniformLocation(pbrShaderProgram, "u_irradianceMap"), 5); 

	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_camPos"), 1, camera.pos);
	glUniform3fv(glGetUniformLocation(pbrShaderProgram, "u_albedo"), 1, vmath::vec3(0.5f, 0.0f, 0.0f));
    glUniform1f(glGetUniformLocation(pbrShaderProgram, "u_ao"), 1.0f);
	for (int row = 0; row < nrRows; ++row) 
	{
		glUniform1f(glGetUniformLocation(pbrShaderProgram, "u_metallic"), (float)row/(float)nrRows); 
		for (int col = 0; col < nrColumns; ++col) 
		{
			glUniform1f(glGetUniformLocation(pbrShaderProgram, "u_roughness"), clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
			
			modelMatrix = vmath::mat4(1.0f);
			modelMatrix = vmath::translate(vmath::vec3(
												(col - (nrColumns / 2)) * spacing, 
												(row - (nrRows / 2)) * spacing, 
												0.0f));
			glUniformMatrix4fv(glGetUniformLocation(pbrShaderProgram, "u_modelMatrix"), 1, GL_FALSE, modelMatrix); 
			glUniformMatrix4fv(glGetUniformLocation(pbrShaderProgram, "u_viewMatrix"), 1, GL_FALSE, camera.viewMatrix); 
			glUniformMatrix4fv(glGetUniformLocation(pbrShaderProgram, "u_projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix); 

			renderSphere();
		}
	}

	// -------------- lights --------------
	glUseProgram(lightShaderProgram); 
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		vmath::vec3 newPos = lightPositions[i] + vmath::vec3(sin(GetElapsedTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		glUniform3fv(glGetUniformLocation(lightShaderProgram, "u_color"), 1, lightColors[i]);

		modelMatrix = vmath::mat4::identity();
		modelMatrix = vmath::translate(newPos);
		modelMatrix *= vmath::scale(vmath::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "u_mvpMatrix"), 1, GL_FALSE, 
						camera.projectionMatrix * camera.viewMatrix * modelMatrix);  

		renderSphere();
	}

	glUseProgram(0); 

	// skybox 
	glDepthFunc(GL_LEQUAL); // set to lequal, as scene objects have depth < 1.0 so skybox will be occluded by them 

	glUseProgram(skyboxShaderProgram); 
	
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "u_viewMatrix"), 1, GL_FALSE, camera.viewMatrix); 
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "u_projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix); 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox); 
	glUniform1i(glGetUniformLocation(skyboxShaderProgram, "u_cubemap"), 0); 
	
	glBindVertexArray(skyboxVAO); 
	glDrawArrays(GL_TRIANGLES, 0, 36); 
	glBindVertexArray(0); 
	glUseProgram(0); 

	glDepthFunc(GL_LESS); 

	// Render ImGui debug window and other UI elements
	if (ImGuiManager::IsEnabled())
	{
		ImGuiManager::RenderDebugWindow();
		ImGuiManager::Render();
	}

	// Swap the buffers
	SwapBuffers(ghdc);
}

float clamp(float val, float min, float max) 
{
	if(val < min) 
		return (min); 
	else if(val > max) 
		return (max); 
	
	return (val); 
} 

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<vmath::vec3> positions;
        std::vector<vmath::vec2> uv;
        std::vector<vmath::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(vmath::vec3(xPos, yPos, zPos));
                uv.push_back(vmath::vec2(xSegment, ySegment));
                normals.push_back(vmath::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i][0]);
            data.push_back(positions[i][1]);
            data.push_back(positions[i][2]);           
            if (normals.size() > 0)
            {
                data.push_back(normals[i][0]);
                data.push_back(normals[i][1]);
                data.push_back(normals[i][2]);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i][0]);
                data.push_back(uv[i][1]);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
        glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(SJ_ATTRIBUTE_NORMAL);        
		glVertexAttribPointer(SJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(SJ_ATTRIBUTE_TEXTURE0);
		glVertexAttribPointer(SJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));        
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
        glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(SJ_ATTRIBUTE_NORMAL);
        glVertexAttribPointer(SJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(SJ_ATTRIBUTE_TEXTURE0);
        glVertexAttribPointer(SJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void update(void)
{
	// code 
	camera.update(); 
}

void uninitialize(void)
{
	
	// function declarations
	void toggleFullScreen(void);

	// code

	// If iser is exiting FullScreen, then restore screen/window
	if ( gbFullScreen == TRUE)
	{
		toggleFullScreen();
	}
	gbFullScreen = FALSE;

	if(texture_albedoMap) 
	{
		glDeleteTextures(1, &texture_albedoMap); 
		texture_albedoMap = 0; 
	}  
	if(texture_normalMap) 
	{
		glDeleteTextures(1, &texture_normalMap); 
		texture_normalMap = 0; 
	}  
	if(texture_roughnessMap) 
	{
		glDeleteTextures(1, &texture_roughnessMap); 
		texture_roughnessMap = 0; 
	}  
	if(texture_metallicMap) 
	{
		glDeleteTextures(1, &texture_metallicMap); 
		texture_metallicMap = 0; 
	}  
	if(texture_aoMap) 
	{
		glDeleteTextures(1, &texture_aoMap); 
		texture_aoMap = 0; 
	}  

	if(objectVAO) 
	{
		glDeleteVertexArrays(1, &objectVAO); 
		objectVAO = 0; 
	}
	if(lightVBO) 
	{
		glDeleteBuffers(1, &lightVBO); 
		lightVBO = 0; 
	}
	if(lightVAO) 
	{
		glDeleteVertexArrays(1, &lightVAO); 
		lightVAO = 0; 
	}

	uninitShader(pbrShaderProgram); 
	pbrShaderProgram = 0; 
	uninitShader(lightShaderProgram); 
	lightShaderProgram = 0; 
	
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
