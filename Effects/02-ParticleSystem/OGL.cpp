// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// OpenGL header files
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

// Custom heander files
#include "OGL.h"
#include "imgui_manager.hpp"  
#include "camera.hpp" 
#include "vmath.h" 
#include "global.hpp" 
#include "particles.hpp"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h" 
using namespace vmath; 

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
BOOL gbActiveWindow = FALSE;
BOOL gbEscapeKeyIsPressed = FALSE;

// variables related to File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

//OpenGL related variables
HDC ghdc = NULL; 
HGLRC ghrc = NULL; 

GLuint smokeProgram; 
mat4 perspectiveProjectionMatrix; 

GLuint smokeTexture = 0; 

Camera camera; 

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

	extern std::vector<Particle> particles;

	// code
	if (ImGuiManager::IsEnabled() && ImGui_ImplWin32_WndProcHandler(hwnd, iMsg, wParam, lParam))
		return true;

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

		case 'Q': 
		case 'q': 
			for(int i = 0; i < particles.size(); ++i) 
			{
				fprintf(gpFile, "p=(%.2f, %.2f, %.2f) l=%.2f\n", particles[i].position[0], particles[i].position[1], particles[i].position[2], particles[i].lifespan); 
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

GLuint loadTexture(const char* pathToImage, BOOL bFlipImage) 
{
	// function prototypes 
	void uninitialize(void); 

	// variable declarations 
	GLuint texture; 
	int width, height, nrComponents; 
	unsigned char* pData = NULL; 

	// code 
	if(bFlipImage == TRUE) 
	{
		stbi_set_flip_vertically_on_load(1); 
	} 

	// get the image data 
	pData = stbi_load(pathToImage, &width, &height, &nrComponents, 0); 
	if(pData == NULL) 
	{
		fprintf(gpFile, "Texture loading failed for %s\n", pathToImage); 
		uninitialize(); 
		exit(EXIT_FAILURE); 
	}  	
	
	// generate OpenGL texture object 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); 	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format; 
	switch(nrComponents) 
	{
		case 1: 
			format = GL_R; 
			break; 

		case 2: 
			format = GL_RG; 
			break; 

		case 3: 
			format = GL_RGB; 
			break; 

		case 4: 
			format = GL_RGBA; 
			break; 

		default: 
			format = GL_RGBA; 
			break; 
	} 

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		format, 
		width, 
		height, 
		0, 
		format, 
		GL_UNSIGNED_BYTE, 
		pData
	);
	glGenerateMipmap(GL_TEXTURE_2D);  

	glBindTexture(GL_TEXTURE_2D, 0); 
	stbi_image_free(pData); 
	pData = NULL; 

	fprintf(gpFile, "Texture loaded : %s\n", pathToImage); 

	stbi_set_flip_vertically_on_load(0); 

	return (texture); 
}  

int initialize(void)
{
	// function declarations 
	void resize(int, int);
	void printGLInfo(void); 
	void uninitialize(void);
	
	void initParticleProgram(void); 
	void initTriangle(void); 

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
	
	initParticleProgram(); 

	initParticles(); 

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

	smokeTexture = loadTexture("star.png", FALSE); 

	// Warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
		
	return (0);
}

void initParticleProgram(void) 
{
	// function declarations 
	void uninitialize(void); 

	// code 
	// ================= vertex shader ======================= 
	// 1) Write the shader source code 
	const GLchar* vertexShaderSourceCode = 
	"#version 460 core\n" \

	"layout (location=0) in vec4 vertex;\n" \

	"uniform mat4 projection;\n" \
	"uniform mat4 view;\n" \
	"uniform vec2 offset;\n" \
	"uniform vec4 color;\n" \

	"out vec4 out_color;\n" \
	"out vec2 out_texCoords;\n" \

	"void main(void) \n" \
	"{\n" \
	"	float scale = 1.0f;" \
	"	out_texCoords = vertex.zw;" \
	"	out_color = color;\n" \
	"	gl_Position = projection * vec4((vertex.xy * scale) + offset, -10.0f, 1.0f);" \
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

	"in vec2 out_texCoords;\n" \
	"out vec4 FragColor;\n" \
	
	"uniform sampler2D sprite;\n" \
	"uniform float life;\n" \

	"void main(void)\n" \
	"{\n" \
	"	FragColor = vec4(texture(sprite, out_texCoords).xyz, life);\n" \
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

	smokeProgram = glCreateProgram(); 

	glAttachShader(smokeProgram, vertexShaderObject); 
	glBindAttribLocation(smokeProgram, 0, "vertex");
	glAttachShader(smokeProgram, fragmentShaderObject); 

	glLinkProgram(smokeProgram); 
	status = 0; 
	infoLogLength = 0; 
	szInfoLog = 0; 
	glGetProgramiv(smokeProgram, GL_LINK_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetProgramiv(smokeProgram, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetProgramInfoLog(smokeProgram, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "shader program object = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}
		uninitialize(); 
	} 
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

	perspectiveProjectionMatrix = vmath::perspective(
		45.0f, // fov 
		(GLfloat)width/(GLfloat)height, // aspect ratio 
		0.1f, // near 
		1000.f // far  
	); 
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

	glUseProgram(smokeProgram); 
	glUniformMatrix4fv(glGetUniformLocation(smokeProgram, "projection"), 1, GL_FALSE, perspectiveProjectionMatrix); 
	glUniformMatrix4fv(glGetUniformLocation(smokeProgram, "view"), 1, GL_FALSE, camera.getViewMatrix()); 
	renderParticles(smokeProgram, smokeTexture); 
	glUseProgram(0); 

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
	updateParticles(); 
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

	uninitializeProgram(&smokeProgram); 

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
