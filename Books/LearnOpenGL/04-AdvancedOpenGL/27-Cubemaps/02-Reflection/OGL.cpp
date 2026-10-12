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
#define STB_IMAGE_IMPLEMENTATION
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
GLuint skyboxShaderProgram; 
GLuint refractionShaderProgram; 

GLuint cubeVAO, cubeVBO;
GLuint skyboxVAO, skyboxVBO; 

GLuint skyboxTexture, cubeTexture;

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

		ShowCursor(FALSE); // Optional step : If upon Full Screen, should the cursor be displayed
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

		ShowCursor(TRUE);
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

	skyboxShaderProgram = createShaderFromSourceFiles("shaders/skybox.vs", "shaders/skybox.fs"); 
	if(skyboxShaderProgram == 0) 
	{
		fprintf(gpFile, "Failed to create depth shader program\n"); 
		return (-11); 
	} 
	refractionShaderProgram = createShaderFromSourceFiles("shaders/skyboxReflection.vs", "shaders/skyboxReflection.fs"); 
	if(refractionShaderProgram == 0) 
	{
		fprintf(gpFile, "Failed to create depth shader program\n"); 
		return (-12); 
	} 

    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
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

    // cube 
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
    glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_NORMAL);
    glVertexAttribPointer(SJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox 
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(SJ_ATTRIBUTE_POSITION);
    glVertexAttribPointer(SJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	const char* cubemapImages[6] = {
		"environment/right.jpg", 
		"environment/left.jpg", 
		"environment/top.jpg", 
		"environment/bottom.jpg", 
		"environment/front.jpg", 
		"environment/back.jpg" 
	}; 
	loadCubemapUsingSTBI(&skyboxTexture, cubemapImages); 
	if(skyboxTexture == 0) 
	{
		fprintf(gpFile, "failed to create cubemap texture\n"); 
		return (-21); 
	} 

	loadTextureUsingSTBI(&cubeTexture, "metal.png"); 
	if(cubeTexture == 0) 
	{
		fprintf(gpFile, "failed to create metal texture\n"); 
		return (-22); 
	}

	// depth related code 
	glClearDepth(1.0f); 
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

	// object 
	vmath::mat4 modelMatrix = vmath::mat4::identity(); 
	glUseProgram(refractionShaderProgram); 
	glUniformMatrix4fv(glGetUniformLocation(refractionShaderProgram, "u_modelMatrix"), 1, GL_FALSE, modelMatrix);  
	glUniformMatrix4fv(glGetUniformLocation(refractionShaderProgram, "u_viewMatrix"), 1, GL_FALSE, camera.viewMatrix);  
	glUniformMatrix4fv(glGetUniformLocation(refractionShaderProgram, "u_projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix);  
	glUniform3fv(glGetUniformLocation(refractionShaderProgram, "u_viewPosition"), 1, camera.pos);  

	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glUniform1i(glGetUniformLocation(refractionShaderProgram, "u_texture"), 0); 
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// skybox 
	glDepthFunc(GL_LEQUAL); // set to lequal, as scene objects have depth < 1.0 so skybox will be occluded by them 

	glUseProgram(skyboxShaderProgram); 
	
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "u_viewMatrix"), 1, GL_FALSE, camera.viewMatrix); 
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "u_projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix); 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture); 
	glBindTexture(glGetUniformLocation(skyboxShaderProgram, "u_cubemap"), 0); 
	
	glBindVertexArray(skyboxVAO); 
	glDrawArrays(GL_TRIANGLES, 0, 36); 
	glBindVertexArray(0); 
	glUseProgram(0); 

	glDepthFunc(GL_LESS); 	// reset the test to default (less) 

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

	if(skyboxTexture) 
	{
		glDeleteTextures(1, &skyboxTexture); 
		skyboxTexture = 0; 
	} 
	if(cubeTexture) 
	{
		glDeleteTextures(1, &cubeTexture); 
		cubeTexture = 0; 
	} 

	if(skyboxVAO) 
	{
		glDeleteVertexArrays(1, &skyboxVAO); 
		skyboxVAO = 0; 
	}
	if(skyboxVBO) 
	{
		glDeleteBuffers(1, &skyboxVBO); 
		skyboxVBO = 0; 
	}
	if(cubeVAO) 
	{
		glDeleteVertexArrays(1, &cubeVAO); 
		cubeVAO = 0; 
	}
	if(cubeVBO) 
	{
		glDeleteBuffers(1, &cubeVBO); 
		cubeVBO = 0; 
	}

	uninitShader(skyboxShaderProgram); 
	skyboxShaderProgram = 0; 
	uninitShader(refractionShaderProgram); 
	refractionShaderProgram = 0; 
	
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
