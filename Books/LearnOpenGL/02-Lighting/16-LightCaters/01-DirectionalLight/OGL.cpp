// Win32 headers
#include <windows.h>
#include <Windowsx.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <assert.h> 

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

// shader related variables 
GLuint cubeShaderProgram = 0; 
GLuint lightShaderProgram = 0; 

GLuint lightVAO = 0; 
GLuint lightVBO = 0; 
GLuint objectVAO = 0; 

vmath::vec3 lightPosition = vmath::vec3(2.0f, 2.0f, 4.0f); 
vmath::vec3 lightAmbient = vmath::vec3(0.2f, 0.2f, 0.2f); 
vmath::vec3 lightDiffuse = vmath::vec3(0.5f, 0.5f, 0.5f); 
vmath::vec3 lightSpecular = vmath::vec3(1.0f, 1.0f, 1.0f); 

float materialShininess 		= 32.0f;
GLuint diffuseTexture; 
GLuint specularTexture; 

// cube positions 
std::vector<vmath::vec3> cubePositions = {
	vmath::vec3( 0.0f,  0.0f,  0.0f),
	vmath::vec3( 2.0f,  5.0f, -15.0f),
	vmath::vec3(-1.5f, -2.2f, -2.5f),
	vmath::vec3(-3.8f, -2.0f, -12.3f),
	vmath::vec3( 2.4f, -0.4f, -3.5f),
	vmath::vec3(-1.7f,  3.0f, -7.5f),
	vmath::vec3( 1.3f, -2.0f, -2.5f),
	vmath::vec3( 1.5f,  2.0f, -2.5f),
	vmath::vec3( 1.5f,  0.2f, -1.5f),
	vmath::vec3(-1.3f,  1.0f, -1.5f)
};

E_camera camera; 

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

	cubeShaderProgram = createShaderFromSourceFiles("shaders/object.vs", "shaders/object.fs"); 
	if(cubeShaderProgram & 0x80000000) 
	{
		fprintf(gpFile, "initialize() > failed to create object shader prgoram\n"); 
		return (-11); 
	} 
	lightShaderProgram = createShaderFromSourceFiles("shaders/object.vs", "shaders/light.fs"); 
	if(lightShaderProgram & 0x80000000) 
	{
		fprintf(gpFile, "initialize() > failed to create light shader prgoram\n"); 
		return (-12); 
	} 

    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

	camera.pos = vmath::vec3(0.0f, 2.0f, 10.0f);

	// depth related code 
	glClearDepth(1.0f); 
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (ImGuiManager::IsEnabled() && !ImGuiManager::Initialize(ghwnd))
	{
		fprintf(gpFile, "ImGui initialization failed\n");
		return (-21); 
	}

	loadTextureUsingSTBI(&diffuseTexture, "container_diffuse.png"); 
	if(diffuseTexture & 0x80000000) 
	{
		fprintf(gpFile, "Failed to load diffuse texture\n"); 
		return (-31); 
	} 
	loadTextureUsingSTBI(&specularTexture, "container_specular.png"); 
	if(specularTexture & 0x80000000) 
	{
		fprintf(gpFile, "Failed to load specular texture\n"); 
		return (-32); 
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

	// Set the View port
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height)); 

	camera.resize(width, height); 
}

void display(void)
{
	// code 	
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ImGuiManager::IsEnabled())
	{
		// Start new ImGui frame
		ImGuiManager::NewFrame();
	}

	// static float cameraRotationAngle = 0.0f; 
	// float r = 10.0f; 
	// camera.pos[0] = r * sin(cameraRotationAngle * 3.14f/180.0); 
	// camera.pos[2] = r * cos(cameraRotationAngle * 3.14f/180.0); 
	// cameraRotationAngle += 0.75f; 
	// camera.lookatPoint = objectPosition; 

	// -------------- object -------------- 
	glUseProgram(cubeShaderProgram); 

	vmath::mat4 modelMatrix = vmath::mat4::identity(); 
	
	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "u_viewPosition"), 1, camera.pos); 

	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "u_light.direction"), 1, vmath::vec3(-0.2f, -1.0f, -0.3f)); 
	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "u_light.ambient"), 1, lightAmbient); 
	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "u_light.diffuse"), 1, lightDiffuse); 
	glUniform3fv(glGetUniformLocation(cubeShaderProgram, "u_light.specular"), 1, lightSpecular); 

	glActiveTexture(GL_TEXTURE0); 
	glUniform1i(glGetUniformLocation(cubeShaderProgram, "u_material.diffuseTex"), 0); 
	glBindTexture(GL_TEXTURE_2D, diffuseTexture); 
	glActiveTexture(GL_TEXTURE1); 
	glUniform1i(glGetUniformLocation(cubeShaderProgram, "u_material.specularTex"), 1); 
	glBindTexture(GL_TEXTURE_2D, specularTexture); 
	glUniform1f(glGetUniformLocation(cubeShaderProgram, "u_material.shininess"), materialShininess); 

	glBindVertexArray(objectVAO);
	for (unsigned int i = 0; i < cubePositions.size(); i++)
	{
		vmath::mat4 modelMatrix = vmath::mat4::identity();
		modelMatrix = vmath::translate(cubePositions[i]);
		float angle = 20.0f * i;
		modelMatrix *= vmath::rotate(vmath::radians(angle), vmath::vec3(1.0f, 0.3f, 0.5f));
		
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "u_modelMatrix"), 1, GL_FALSE, modelMatrix);  
		glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "u_mvpMatrix"), 1, GL_FALSE, 
							camera.projectionMatrix * camera.viewMatrix * modelMatrix);  

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
	glUseProgram(0); 

	// -------------- light --------------
	// no light object is rendered for directional light 
	// glUseProgram(lightShaderProgram); 

	// modelMatrix = vmath::translate(lightPosition) * vmath::scale(0.2f); 

	// glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "u_mvpMatrix"), 1, GL_FALSE, 
	// 					camera.projectionMatrix * camera.viewMatrix * modelMatrix);  
	// glBindVertexArray(lightVAO);  
	// glDrawArrays(GL_TRIANGLES, 0, 36); 
	// glBindVertexArray(0); 

	// glUseProgram(0); 

	// Render ImGui debug window and other UI elements
	if (ImGuiManager::IsEnabled())
	{
		ImGuiManager::RenderDebugWindow();
		ImGuiManager::Render();
	}

	// Swap the buffers
	SwapBuffers(ghdc);
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

	if(specularTexture) 
	{
		glDeleteTextures(1, &specularTexture); 
		specularTexture = 0; 
	} 
	if(diffuseTexture) 
	{
		glDeleteTextures(1, &diffuseTexture); 
		diffuseTexture = 0; 
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

	uninitShader(cubeShaderProgram); 
	cubeShaderProgram = 0; 
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
