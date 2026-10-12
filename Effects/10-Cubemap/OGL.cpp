// Win32 headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.hpp" 

// Custom header files
#include "OGL.h"
#include "model.hpp" 
#include "global.hpp" 

// OpenGL header files
#include <gl/glew.h> 	// this header file must be included before gl/GL.h 
#include <gl/GL.h> 

#include "vmath.h" 
using namespace vmath; 

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

// shader related variables 
GLuint shaderProgramObject; 
GLuint modelProgram; 

GLuint vao = 0; 
GLuint vbo_position = 0; 

GLuint viewMatrixUniform = 0; 
GLuint projectionMatrixUniform = 0; 
GLuint skyboxSamplerUniform = 0; 

mat4 perspectiveProjectionMatrix; 

// variables related to File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// Active Window related variables
BOOL gbActiveWindow = FALSE;

// Exit keypress related
BOOL gbEscapeKeyIsPressed = FALSE;

// cubemap related variables 
GLuint texture_cubemap; 

Camera camera; 
Model* model = NULL; 

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


	camera.cameraCallback(hwnd, iMsg, wParam, lParam); 

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

		ShowCursor(FALSE); // Optional step : If upon Full Screen, should the cursor be displayed
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
	void initModelProgram(void); 

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

	printGLInfo(); 
	
	// ================= vertex shader ======================= 
	// 1) Write the shader source code
	const GLchar* vertexShaderSourceCode = 
	"#version 460 core\n" \
	"in vec4 aPosition;\n" \
	"uniform mat4 uViewMatrix;\n" \
	"uniform mat4 uProjectionMatrix;\n" \
	"out vec3 out_texCoords;\n" \
	"void main(void) \n" \
	"{\n" \
	"vec4 pos = uProjectionMatrix * mat4(mat3(uViewMatrix)) * aPosition;\n" \
	"gl_Position = pos.xyww;\n" \
	"out_texCoords = vec3(aPosition.xyz);\n" \
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
	"out vec4 FragColor;"
	"in vec3 out_texCoords;\n" \
	"uniform samplerCube uSkyboxSampler;\n" \
	"void main(void)\n" \
	"{\n" \
	"FragColor = texture(uSkyboxSampler, out_texCoords);\n" \
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

	// ========================== create, attach, link shader program object =========================== 
	// 1) Create shader program object 
	shaderProgramObject = glCreateProgram(); 

	// 2) attch shaders to this shader program object 
	glAttachShader(shaderProgramObject, vertexShaderObject); 

	// bind shader attribute at certain index in shader to save index in host program 
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition"); // address fixation 

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
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix"); 
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix"); 
	skyboxSamplerUniform = glGetUniformLocation(shaderProgramObject, "uSkyboxSampler"); 

	const GLfloat cube_position[] = 
	{
		// front
		1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front
		1.0f, -1.0f,  1.0f, // bottom-right of front

		// right
		1.0f,  1.0f, -1.0f, // top-right of right
		1.0f,  1.0f,  1.0f, // top-left of right
		1.0f, -1.0f,  1.0f, // bottom-left of right
		1.0f, -1.0f, -1.0f, // bottom-right of right

		// back
		1.0f,  1.0f, -1.0f, // top-right of back
		-1.0f,  1.0f, -1.0f, // top-left of back
		-1.0f, -1.0f, -1.0f, // bottom-left of back
		1.0f, -1.0f, -1.0f, // bottom-right of back

		// left
		-1.0f,  1.0f,  1.0f, // top-right of left
		-1.0f,  1.0f, -1.0f, // top-left of left
		-1.0f, -1.0f, -1.0f, // bottom-left of left
		-1.0f, -1.0f,  1.0f, // bottom-right of left

		// top
		1.0f,  1.0f, -1.0f, // top-right of top
		-1.0f,  1.0f, -1.0f, // top-left of top
		-1.0f,  1.0f,  1.0f, // bottom-left of top
		1.0f,  1.0f,  1.0f, // bottom-right of top

		// bottom
		1.0f, -1.0f,  1.0f, // top-right of bottom
		-1.0f, -1.0f,  1.0f, // top-left of bottom
		-1.0f, -1.0f, -1.0f, // bottom-left of bottom
		1.0f, -1.0f, -1.0f, // bottom-right of bottom
	}; 

	// CUBE 
	glGenVertexArrays(1, &vao); // vao-vertex array object 
	glBindVertexArray(vao); 
	
	// position  
	glGenBuffers(1, &vbo_position); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_position), cube_position, GL_STATIC_DRAW); 
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 

	// depth related code
	glClearDepth(1.0f); 
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL); 

	// Instruct OpenGl to choose the colour to clear the screen 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity(); 

	const char* cubemapImages[6] = 
	{
		"environment1/px.png", 
		"environment1/nx.png", 
		"environment1/py.png", 
		"environment1/ny.png", 
		"environment1/pz.png", 
		"environment1/nz.png" 
	}; 


	fprintf(gpFile, "before texture loaded\n"); 
	fflush(gpFile); 

	void loadCubeMapTexture(const char* images[]); 
	loadCubeMapTexture(cubemapImages); 

	fprintf(gpFile, "texture loaded\n"); 
	fflush(gpFile); 

	initModelProgram(); 
	model = new Model("medival-house/scene.gltf"); 

	// Warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
		
	return (0);
}

void initModelProgram(void) 
{
	// function declarations 
	void uninitialize(void); 

	// code 
	// ================= vertex shader ======================= 
	// 1) Write the shader source code 
	const GLchar* vertexShaderSourceCode = 
	"#version 460 core\n" \
	"in vec4 aPosition;\n" \
	"in vec2 aTexCoords0;\n" \
	"in vec2 aTexCoords1;\n" \
	"in vec2 aTexCoords2;\n" \

	"uniform int channel;\n" \

	"out vec2 out_texCoords;\n" \
	"uniform mat4 uMVPMatrix;\n" \
	
	"void main(void) \n" \
	"{\n" \
	"gl_Position = uMVPMatrix * aPosition;\n" \
	"if(channel == 0)\n" \
	"	out_texCoords = aTexCoords0;\n" \
	"else if(channel == 1)\n" \
	"	out_texCoords = aTexCoords1;\n" \
	"else if(channel == 2)\n" \
	"	out_texCoords = aTexCoords2;\n" \
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
	"in vec2 out_texCoords;\n" \
	"out vec4 FragColor;\n" \
	"uniform sampler2D texture_diffuse1;\n" \
	"void main(void)\n" \
	"{\n" \
	"FragColor = texture(texture_diffuse1, out_texCoords);\n" \
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
	modelProgram = glCreateProgram(); 

	// 2) attch shaders to this shader program object 
	glAttachShader(modelProgram, vertexShaderObject); 

	// bind shader attribute at certain index in shader to save index in host program 
	glBindAttribLocation(modelProgram, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(modelProgram, AMC_ATTRIBUTE_TEXTURE0, "aTexCoords0");
	glBindAttribLocation(modelProgram, AMC_ATTRIBUTE_TEXTURE1, "aTexCoords1");
	glBindAttribLocation(modelProgram, AMC_ATTRIBUTE_TEXTURE2, "aTexCoords2");

	glAttachShader(modelProgram, fragmentShaderObject); 

	// 3) tell to link shader objects to shader program objects 
	glLinkProgram(modelProgram); 

	// 4) check for link error log 
	status = 0; 
	infoLogLength = 0; 
	szInfoLog = 0; 
	glGetProgramiv(modelProgram, GL_LINK_STATUS, &status); 
	if(status == GL_FALSE) 
	{
		glGetProgramiv(modelProgram, GL_INFO_LOG_LENGTH, &infoLogLength); 
		if(infoLogLength > 0) 
		{
			szInfoLog = (GLchar*)malloc(infoLogLength * sizeof(GLchar)); 
			if(szInfoLog != NULL) 
			{
				glGetProgramInfoLog(modelProgram, infoLogLength, NULL, szInfoLog); 
				fprintf(gpFile, "shader program object = %s\n", szInfoLog); 

				free(szInfoLog); 
				szInfoLog = NULL; 
			}
		}

		uninitialize(); 
	} 
} 

void loadCubeMapTexture(const char* images[])
{
    glGenTextures(1, &texture_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < 6; ++i)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(images[i], &width, &height, &nrChannels, 4);

        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGBA,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
            fprintf(gpFile, "%dth image loaded\n", i);
        }
        else
        {
            fprintf(gpFile, "Failed to load image: %s\n", images[i]);
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void printGLInfo(void) 
{
	// variable declarations 
	GLint numExtensions, i; 

	// code 
	fprintf(gpFile, "OpenGL Vendor = %s\n", glGetString(GL_VENDOR)); 
	fprintf(gpFile, "OpenGL Renderer = %s\n", glGetString(GL_RENDERER));  
	fprintf(gpFile, "OpenGL Version = %s\n", glGetString(GL_VERSION)); 

	fprintf(gpFile, "GLSL Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); 

	// get number of extensions 
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);  
	fprintf(gpFile, "Number of extensions = %d\n", numExtensions);  
	
	// print OpenGL extensions 
	// for(i = 0; i < numExtensions; ++i) 
	// {
	// 	fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i)); 
	// } 
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
		100.f // far  
	); 
}

void display(void)
{
	// code
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use shader program object 
	glUseProgram(shaderProgramObject); 

	mat4 viewMatrix = camera.getViewMatrix(); 
	
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);  
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);  
	
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap); 
	glUniform1i(skyboxSamplerUniform, 0);  	

	glBindVertexArray(vao); 
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4); 
	glBindVertexArray(0); 

	// unuse shader program object  
	glUseProgram(0); 

	model->draw(modelProgram, mat4::identity(), viewMatrix, perspectiveProjectionMatrix); 

	// Swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// code 
}

void uninitialize(void)
{
	
	// function declarations
	void toggleFullScreen(void);

	// code

	// If iser is exiting FullScreen, then restore screen/window
	if(gbFullScreen == TRUE)
	{
		toggleFullScreen();
	}
	gbFullScreen = FALSE;

	// free vbo 
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

	// Detach, delete shader objects and delete shaderProgramObject 
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

