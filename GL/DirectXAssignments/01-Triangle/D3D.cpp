#include <Windows.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

#include <dxgi.h>  
#include <d3d11.h> 
#include <d3dCompiler.h> 

// for d3d11 maths 
#pragma warning(disable:4838)
#include "XNAMath/xnamath.h" 

// custum headers 
#include "D3d.h" 

// libraries 
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "d3dcompiler.lib") 

// macros 
#define WIN_WIDTH   800 
#define WIN_HEIGHT  600 

// global function declarations 
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 

// global variable declarations 
BOOL gbFullscreen = FALSE; 
HWND ghwnd = NULL; 
DWORD dwStyle; 
WINDOWPLACEMENT wpPrev; 

char gszLogFileName[] = "Log.txt"; 
FILE* gpFile = NULL; 

BOOL gbActiveWindow = FALSE; 
BOOL gbEscapeKeyIsPressed = FALSE; 

IDXGISwapChain* gpIDXGISwapChain = NULL; 
ID3D11Device* gpID3D11Device = NULL; 
ID3D11DeviceContext* gpID3D11DeviceContext = NULL; 
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL; 
ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL; 

ID3D11VertexShader* gpID3D11VertexShader = NULL; 
ID3D11PixelShader* gpID3D11PixelShader = NULL; 
ID3D11Buffer* gpID3D11Buffer_PositionBuffer = NULL; 
ID3D11Buffer* gpID3D11Buffer_ColorBuffer = NULL; 
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL; 
ID3D11InputLayout* gpID3D11InputLayout = NULL; 

ID3D11RasterizerState* gpID3D11RasterizerState = NULL; 

struct CBUFFER 
{
    XMMATRIX worldViewProjectionMatrix; 
}; 

float clearColor[4]; 

XMMATRIX perspectiveProjectionMatrix; 

// animation related variables 
float anglePyramid = 0.0f; 

// entry-point function 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) 
{
    // local function declarations 
    HRESULT initialize(void); 
    void display(void); 
    void update(void); 
    void uninitialize(void); 

    // variable declarations 
    WNDCLASSEX wndclass; 
    HWND hwnd;
    MSG msg; 
    TCHAR szAppName[] = TEXT("RTR7"); 
    BOOL bDone = FALSE; 
    HRESULT result = S_OK; 

    // code 
    // create log file 
    gpFile = fopen(gszLogFileName, "w"); 
    if(gpFile == NULL) 
    {
        MessageBox(NULL, TEXT("Failed to create log file"), TEXT("Error"), MB_OK | MB_ICONERROR); 
        exit (EXIT_FAILURE); 
    } 
    else 
    {
        fprintf(gpFile, "Program started succesfully\n"); 
        fclose(gpFile); 
    } 

    // window class initialization 
    wndclass.cbSize = sizeof(WNDCLASSEX); 
    wndclass.cbClsExtra = 0; 
    wndclass.cbWndExtra = 0; 
    wndclass.style = CS_HREDRAW | CS_VREDRAW; 
    wndclass.hInstance = hInstance; 
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON)); 
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON)); 
    wndclass.hCursor = LoadCursor(hInstance, IDI_APPLICATION); 
    wndclass.lpfnWndProc = WndProc; 
    wndclass.lpszClassName = szAppName; 
    wndclass.lpszMenuName = NULL; 

    // register window class 
    RegisterClassEx(&wndclass); 

    // create  
    hwnd = CreateWindowEx(
                WS_EX_APPWINDOW, 
                szAppName, 
                TEXT("DirectX"), 
                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 
                GetSystemMetrics(SM_CXSCREEN) / 2 - WIN_WIDTH/2, 
                GetSystemMetrics(SM_CYSCREEN)/2 - WIN_HEIGHT/2, 
                WIN_WIDTH, 
                WIN_HEIGHT, 
                NULL, 
                NULL, 
                hInstance, 
                NULL  
            ); 
    if(hwnd == NULL) 
    {
        fprintf(gpFile, "Failed to create window\n"); 
        uninitialize(); 
        exit(EXIT_FAILURE); 
    } 

    ghwnd = hwnd; 

    // show and update 
    ShowWindow(hwnd, iCmdShow); 

    UpdateWindow(hwnd); 

    // initialize 
    result = initialize(); 
    if(FAILED(result)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "WinMain(): initialize() failed\n"); 
        fclose(gpFile); 

        uninitialize(); 
        DestroyWindow(hwnd); 
        hwnd = NULL; 
        ghwnd = NULL; 
    }  
    else 
    {
        fprintf(gpFile, "WinMain() : initialize() completed Successfully\n"); 
        fclose(gpFile); 
    } 

    // set window as foreground and active window 
    SetForegroundWindow(hwnd); 
    SetFocus(hwnd); 

    // game loop 
    while(bDone == FALSE) 
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            if(msg.message == WM_QUIT) 
                bDone = TRUE; 
            else 
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } 
        } 
        else 
        {
            if(gbActiveWindow == TRUE) 
            {
                if(gbEscapeKeyIsPressed == TRUE) 
                    bDone = TRUE; 

                display(); 
                update(); 
            } 
        } 
    } 

    // uninitialize 
    uninitialize();
    
    return ((int)msg.wParam); 
}; 

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    // local function declarations 
    void toggleFullscreen(void); 
    void uninitialize(void); 
    HRESULT resize(int, int); 

    // local variable declarations 
    HRESULT hr = S_OK; 

    // code 
    switch(uMsg) 
    {
        case WM_CREATE: 
            ZeroMemory((void*)&wpPrev, sizeof(WINDOWPLACEMENT)); 
            wpPrev.length = sizeof(WINDOWPLACEMENT); 
            break; 

        case WM_SIZE: 
            if(gpID3D11DeviceContext) 
            {
                hr = resize(LOWORD(lParam), HIWORD(lParam)); 
                if(FAILED(hr)) 
                {
                    gpFile = fopen(gszLogFileName, "a+"); 
                    fprintf(gpFile, "WndProc() > resize() failed\n"); 
                    fclose(gpFile); 
                } 
            } 
            break; 

        case WM_SETFOCUS: 
            gbActiveWindow = TRUE; 
            break; 

        case WM_KILLFOCUS: 
            gbActiveWindow = FALSE; 
            break; 

        case WM_KEYDOWN: 
            switch(wParam) 
            {
                case VK_ESCAPE:    
                    gbEscapeKeyIsPressed = TRUE; 
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
                    if(gbFullscreen == FALSE) 
                    {
                        toggleFullscreen(); 
                        gbFullscreen = TRUE; 
                    } 
                    else 
                    {
                        toggleFullscreen(); 
                        gbFullscreen = FALSE; 
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

    return (DefWindowProc(hwnd, uMsg, wParam, lParam)); 
} 

void toggleFullscreen(void) 
{
    // variable declarations 
    MONITORINFO mi; 

    // code 
    if(gbFullscreen == FALSE) 
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE); 
        if(dwStyle & WS_OVERLAPPEDWINDOW) 
        {
            ZeroMemory((void*)&mi, sizeof(MONITORINFO)); 
            mi.cbSize = sizeof(MONITORINFO); 

            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) 
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW); 
                SetWindowPos(
                    ghwnd, HWND_TOP, 
                    mi.rcMonitor.left, 
                    mi.rcMonitor.top, 
                    mi.rcMonitor.right-mi.rcMonitor.left, 
                    mi.rcMonitor.bottom-mi.rcMonitor.top, 
                    SWP_NOZORDER | SWP_FRAMECHANGED 
                ); 
            } 
        } 

        ShowCursor(FALSE); 
    } 
    else 
    {
        SetWindowPlacement(ghwnd, &wpPrev); 
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW); 
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED); 

        ShowCursor(TRUE); 
    } 
} 

HRESULT initialize(void) 
{
    // function declarations 
    HRESULT resize(int width, int height); 
    
    // variable declarations 
    HRESULT hr = S_OK; 
    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc; 

    // GET DXGI SWAP CHAIN, DEVICE AND DEVICE CONTEXT 
    // specify swap chain description 
    ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC)); 
    dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH; 
    dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT; 
    dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60; 
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; 
    dxgiSwapChainDesc.BufferCount = 1; 
    dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
    dxgiSwapChainDesc.SampleDesc.Count = 1;  
    dxgiSwapChainDesc.SampleDesc.Quality = 0; 
    dxgiSwapChainDesc.OutputWindow = ghwnd; 
    dxgiSwapChainDesc.Windowed = TRUE; 

    // specify required driver type and feature levels 
    D3D_DRIVER_TYPE d3dDriverType; 
    D3D_DRIVER_TYPE d3dDriverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE, 
        D3D_DRIVER_TYPE_WARP, 
        D3D_DRIVER_TYPE_SOFTWARE, 
        D3D_DRIVER_TYPE_REFERENCE 
    }; 
    UINT numDriverTypes = sizeof(d3dDriverTypes)/sizeof(d3dDriverTypes[0]); 

    D3D_FEATURE_LEVEL d3dFeatureLevel_Required = D3D_FEATURE_LEVEL_11_0; 
    D3D_FEATURE_LEVEL d3dFeatureLevel_Acquired = D3D_FEATURE_LEVEL_10_0; 

    // get swap chain, device and device context 
    for(UINT i = 0; i < numDriverTypes; ++i) 
    {
        d3dDriverType = d3dDriverTypes[i]; 
        hr = D3D11CreateDeviceAndSwapChain(
                NULL, 
                d3dDriverType, 
                NULL, 
                0, 
                &d3dFeatureLevel_Required, 
                1, 
                D3D11_SDK_VERSION, 
                &dxgiSwapChainDesc, 
                &gpIDXGISwapChain, 
                &gpID3D11Device, 
                &d3dFeatureLevel_Acquired, 
                &gpID3D11DeviceContext  
            );  
        if(SUCCEEDED(hr)) 
            break; 
    } 

    if(FAILED(hr)) 
    {
        fclose(gpFile); 
        fprintf(gpFile, "initialize() > D3D11CreateDeviceAndSwapChain() failed\n"); 

        return (hr); 
    } 

    if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE) 
        fprintf(gpFile, "Chosen driver is hardware driver\n"); 
    else if(d3dDriverType == D3D_DRIVER_TYPE_WARP) 
        fprintf(gpFile, "Chosen driver is warp driver\n"); 
    else if(d3dDriverType == D3D_DRIVER_TYPE_SOFTWARE) 
        fprintf(gpFile, "Chosen driver is software driver\n"); 
    else if(d3dDriverType == D3D_DRIVER_TYPE_REFERENCE) 
        fprintf(gpFile, "Chosen driver is reference driver\n"); 
    else 
        fprintf(gpFile, "Chosen driver is unknown\n"); 

    // check which frature level is acquired 
    if(d3dFeatureLevel_Acquired == D3D_FEATURE_LEVEL_11_0) 
        fprintf(gpFile, "Feature level 11.0 is acquired\n"); 
    if(d3dFeatureLevel_Acquired == D3D_FEATURE_LEVEL_10_1) 
        fprintf(gpFile, "Feature level 10.1 is acquired\n"); 
    if(d3dFeatureLevel_Acquired == D3D_FEATURE_LEVEL_10_0) 
        fprintf(gpFile, "Feature level 10.0 is acquired\n"); 
    else 
        fprintf(gpFile, "Very old feature level acquired\n"); 
    
    fclose(gpFile); 

    // CREATE AND SET SHADERS 
    const char* vertexShaderSourceCode =
        "cbuffer ConstantBuffer\n" \
        "{\n" \
        "   float4x4 worldViewProjectionMatrix;\n" \
        "}\n" \

        "struct Vertex_Output\n" \
        "{\n" \
        "   float4 position: SV_POSITION;\n" \
        "   float4 color: COLOR;\n" \
        "};\n" \

        "Vertex_Output main(float4 pos : POSITION, float4 col : COLOR)\n" 
        "{\n" \
        "   Vertex_Output output;\n" \
        "   output.position = mul(worldViewProjectionMatrix, pos);\n" \
        "   output.color = col;\n" \
        "   return (output);\n" \
        "}\n"; 

    // compile shader 
    ID3DBlob* pID3DBlob_VertexShaderCode = NULL; 
    ID3DBlob* pID3DBlob_Error = NULL; 
    hr = D3DCompile(
            vertexShaderSourceCode, 
            lstrlenA(vertexShaderSourceCode), 
            "VS", 
            NULL, 
            D3D_COMPILE_STANDARD_FILE_INCLUDE, 
            "main", 
            "vs_5_0", 
            0, 
            0, 
            &pID3DBlob_VertexShaderCode, 
            &pID3DBlob_Error 
        ); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        if(pID3DBlob_Error != NULL) 
        {
            fprintf(gpFile, "initialize() > D3DCompile() for vertex shader failed with error %s\n", 
                    (char*)pID3DBlob_Error->GetBufferPointer()); 
        } 
        fclose(gpFile); 

        pID3DBlob_Error->Release(); 
        pID3DBlob_Error = NULL; 
        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > D3DCompile for vertex shader succesfull\n"); 
        fclose(gpFile); 
    } 

    // create shader 
    hr = gpID3D11Device->CreateVertexShader(
            pID3DBlob_VertexShaderCode->GetBufferPointer(),
            pID3DBlob_VertexShaderCode->GetBufferSize(), 
            NULL, 
            &gpID3D11VertexShader 
        ); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateVertexShader() failed for vertex shader\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateVertexShader() for vertex shader successfull\n"); 
        fclose(gpFile); 
    } 

    // now set this shader into pipeline 
    gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0); 

    const char* pixelShaderSourceCode = 
        "struct Vertex_Output\n" \
        "{\n" \
        "   float4 position: SV_POSITION;\n" \
        "   float4 color : COLOR;\n" \
        "};\n" \

        "float4 main(Vertex_Output input) : SV_TARGET\n" \
        "{\n" \
        "   float4 color = input.color;\n" \
        "   return (color);\n" \
        "}"; 

    // compile shader 
    ID3DBlob* pID3DBlob_PixelShaderCode = NULL; 
    pID3DBlob_Error = NULL; 
    hr = D3DCompile(
            pixelShaderSourceCode, 
            lstrlenA(pixelShaderSourceCode), 
            "PS", 
            NULL, 
            D3D_COMPILE_STANDARD_FILE_INCLUDE, 
            "main", 
            "ps_5_0", 
            0, 
            0, 
            &pID3DBlob_PixelShaderCode, 
            &pID3DBlob_Error 
        ); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        if(pID3DBlob_Error != NULL) 
        {
            fprintf(gpFile, "initialize() > D3DCompile() for vertex pixel failed with error %s\n", 
                    (char*)pID3DBlob_Error->GetBufferPointer()); 
        } 
        fclose(gpFile); 

        pID3DBlob_Error->Release(); 
        pID3DBlob_Error = NULL; 
        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > D3DCompile for pixel shader succesfull\n"); 
        fclose(gpFile); 
    } 

    // create shader 
    hr = gpID3D11Device->CreatePixelShader(
            pID3DBlob_PixelShaderCode->GetBufferPointer(),
            pID3DBlob_PixelShaderCode->GetBufferSize(), 
            NULL, 
            &gpID3D11PixelShader 
        ); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreatePixelShader() failed for pixel shader\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreatePixelShader() for pixel shader successfull\n"); 
        fclose(gpFile); 
    } 

    // now set this shader into pipeline 
    gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0); 

    // SET INPUT LAYOUT INTO PIPELINE  
    // initialize input layout 
    D3D11_INPUT_ELEMENT_DESC d3d11InputElementDesc[2]; 
    ZeroMemory((void*)d3d11InputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC)*_ARRAYSIZE(d3d11InputElementDesc)); 
    // position 
    d3d11InputElementDesc[0].SemanticName = "POSITION"; 
    d3d11InputElementDesc[0].SemanticIndex = 0; 
    d3d11InputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
    d3d11InputElementDesc[0].InputSlot = 0; 
    d3d11InputElementDesc[0].AlignedByteOffset = 0; 
    d3d11InputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
    d3d11InputElementDesc[0].InstanceDataStepRate = 0; 
    // color 
    d3d11InputElementDesc[1].SemanticName = "COLOR"; 
    d3d11InputElementDesc[1].SemanticIndex = 0; 
    d3d11InputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
    d3d11InputElementDesc[1].InputSlot = 1; 
    d3d11InputElementDesc[1].AlignedByteOffset = 0; 
    d3d11InputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
    d3d11InputElementDesc[1].InstanceDataStepRate = 0; 
    
    // create input layout based on above structure  
    hr = gpID3D11Device->CreateInputLayout(
                                d3d11InputElementDesc, 
                                _ARRAYSIZE(d3d11InputElementDesc), 
                                pID3DBlob_VertexShaderCode->GetBufferPointer(), 
                                pID3DBlob_VertexShaderCode->GetBufferSize(), 
                                &gpID3D11InputLayout 
                            ); 
    if(FAILED(hr)) 
    {        
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateInputLayout() failed\n"); 
        fclose(gpFile); 

        if(pID3DBlob_PixelShaderCode) 
        {
            pID3DBlob_PixelShaderCode->Release(); 
            pID3DBlob_PixelShaderCode = NULL; 
        } 
        if(pID3DBlob_VertexShaderCode) 
        {
            pID3DBlob_VertexShaderCode->Release(); 
            pID3DBlob_VertexShaderCode = NULL; 
        } 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateInputLayout() succesfull\n"); 
        fclose(gpFile); 
    } 

    // now set this layout into pipeline 
    gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout); 

    if(pID3DBlob_PixelShaderCode) 
    {
        pID3DBlob_PixelShaderCode->Release(); 
        pID3DBlob_PixelShaderCode = NULL; 
    } 
    if(pID3DBlob_VertexShaderCode) 
    {
        pID3DBlob_VertexShaderCode->Release(); 
        pID3DBlob_VertexShaderCode = NULL; 
    } 

    // CREATE BUFFERS FOR VERTICE DATA  
    const float pyramidPosition[] = 
    {
        // front
        +0.0f, +1.0f, +0.0f, // front-top
        +1.0f, -1.0f, -1.0f, // front-right
        -1.0f, -1.0f, -1.0f, // front-left
        
        // right
        +0.0f, +1.0f, +0.0f, // right-top
        +1.0f, -1.0f, +1.0f, // right-right
        +1.0f, -1.0f, -1.0f, // right-left

        // back
        +0.0f, +1.0f, +0.0f, // back-top
        -1.0f, -1.0f, +1.0f, // back-right
        +1.0f, -1.0f, +1.0f, // back-left

        // left
        +0.0f, +1.0f, +0.0f, // left-top
        -1.0f, -1.0f, -1.0f, // left-right
        -1.0f, -1.0f, +1.0f, // left-left
    };

    // color
    const float pyramidColors[] =
    {
        // front
        1.0f, 0.0f, 0.0f, // front-top
        0.0f, 0.0f, 1.0f, // front-left
        0.0f, 1.0f, 0.0f, // front-right
        
        // right
        1.0f, 0.0f, 0.0f, // right-top
        0.0f, 1.0f, 0.0f, // right-right
        0.0f, 0.0f, 1.0f, // right-left
        
        // back
        1.0f, 0.0f, 0.0f, // back-top
        0.0f, 0.0f, 1.0f, // back-right
        0.0f, 1.0f, 0.0f, // back-left
        
        // left
        1.0f, 0.0f, 0.0f, // left-top
        0.0f, 1.0f, 0.0f, // left-right
        0.0f, 0.0f, 1.0f, // left-left
    };

    // position 
    D3D11_BUFFER_DESC d3d11BufferDesc; 
    ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC)); 

    // describe buffer 
    d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT; 
    d3d11BufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramidPosition); 
    d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 

    // initialize subresource of buffer for vertex data 
    D3D11_SUBRESOURCE_DATA d3d11SubresourceData; 
    ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA)); 
    d3d11SubresourceData.pSysMem = pyramidPosition; 

    // now create actual buffer 
    hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_PositionBuffer); 
    if(FAILED(hr)) 
    {        
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() failed for position data\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() succesfull for position data\n"); 
        fclose(gpFile); 
    } 

    // color 
    // describe buffer 
    ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC)); 
    d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT; 
    d3d11BufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramidColors); 
    d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 

    // initialize subresource of buffer for vertex data 
    ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA)); 
    d3d11SubresourceData.pSysMem = pyramidColors; 

    // now create actual buffer 
    hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_ColorBuffer); 
    if(FAILED(hr)) 
    {        
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() failed for color data\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() succesfull for color data\n"); 
        fclose(gpFile); 
    } 

    // CREATE AND SET CONSTANT DATA BUFFER INTO PIPELINE  
    // describe buffer 
    ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC)); 
    d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT; 
    d3d11BufferDesc.ByteWidth = sizeof(CBUFFER); 
    d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 

    // create buffer 
    hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_ConstantBuffer); 
    if(FAILED(hr)) 
    {        
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() failed for uniform data\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateBuffer() succesfull for uniform data\n"); 
        fclose(gpFile); 
    } 

    // set above empty constant buffer into pipeline 
    gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer); 

    // SET RASTERIZER STATE 
    // describe rasterizer state 
    D3D11_RASTERIZER_DESC d3d11RasterizerDesc; 
    ZeroMemory((void*)&d3d11RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC)); 
    d3d11RasterizerDesc.AntialiasedLineEnable = FALSE; 
    d3d11RasterizerDesc.CullMode = D3D11_CULL_NONE; 
    d3d11RasterizerDesc.DepthBias = 0; 
    d3d11RasterizerDesc.DepthBiasClamp = 0.0; 
    d3d11RasterizerDesc.DepthClipEnable = TRUE; 
    d3d11RasterizerDesc.FillMode = D3D11_FILL_SOLID; 
    d3d11RasterizerDesc.FrontCounterClockwise = FALSE; 
    d3d11RasterizerDesc.MultisampleEnable = FALSE; 
    d3d11RasterizerDesc.ScissorEnable = FALSE; 
    d3d11RasterizerDesc.SlopeScaledDepthBias = 0.0f; 

    hr = gpID3D11Device->CreateRasterizerState(&d3d11RasterizerDesc, &gpID3D11RasterizerState); 
    if(FAILED(hr)) 
    {        
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateRasterizerState() failed\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    else 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > gpID3D11Device::CreateRasterizerState() succesfull\n"); 
        fclose(gpFile); 
    } 

    // set into pipeline 
    gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState); 

    // set clear color 
    clearColor[0] = 0.0f; 
    clearColor[1] = 0.0f; 
    clearColor[2] = 0.0f; 
    clearColor[3] = 1.0f; 

    perspectiveProjectionMatrix = XMMatrixIdentity(); 

    // WARM-UP RESIZE 
    hr = resize(WIN_WIDTH, WIN_HEIGHT); 
    if(FAILED(hr)) 
    {
        fclose(gpFile); 
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > resize() failed\n"); 

        return (hr); 
    } 
    else 
    {
        fclose(gpFile); 
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "initialize() > warmup resize completed succesfully\n"); 
    } 

    return (hr); 
} 

HRESULT resize(int width, int height) 
{
    // variable declarations 
    HRESULT hr; 

    // code 
    // release depth stencil view if already present 
    if(gpID3D11DepthStencilView) 
    {
        gpID3D11DepthStencilView->Release(); 
        gpID3D11DepthStencilView = NULL; 
    } 

    // relase RTV if already present 
    if(gpID3D11RenderTargetView) 
    {
        gpID3D11RenderTargetView->Release(); 
        gpID3D11RenderTargetView = NULL; 
    } 

    // resize swap chain buffers according to new size 
    gpIDXGISwapChain->ResizeBuffers(
        1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, NULL); 

    // get buffer from swap chain for render target view 
    ID3D11Texture2D* pID3D11Texture2D_BackBuffer = NULL; 
    gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pID3D11Texture2D_BackBuffer); 

    // create render target using above buffer 
    hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView); 
    if(FAILED(hr)) 
    {
        if(pID3D11Texture2D_BackBuffer) 
        {
            pID3D11Texture2D_BackBuffer->Release(); 
            pID3D11Texture2D_BackBuffer = NULL; 
        } 

        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "resize() > resize() failed\n"); 
        fclose(gpFile); 

        return (hr); 
    } 
    if(pID3D11Texture2D_BackBuffer) 
    {
        pID3D11Texture2D_BackBuffer->Release(); 
        pID3D11Texture2D_BackBuffer = NULL; 
    } 
    
    D3D11_TEXTURE2D_DESC d3d11Texture2DDesc; 
    ZeroMemory((void*)&d3d11Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC)); 
    d3d11Texture2DDesc.Width = (UINT)width; 
    d3d11Texture2DDesc.Height = (UINT)height; 
    d3d11Texture2DDesc.ArraySize = 1; 
    d3d11Texture2DDesc.MipLevels = 1; 
    d3d11Texture2DDesc.SampleDesc.Count = 1; 
    d3d11Texture2DDesc.SampleDesc.Quality = 0; 
    d3d11Texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT; 
    d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT; 
    d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; 
    d3d11Texture2DDesc.CPUAccessFlags = 0; 
    d3d11Texture2DDesc.MiscFlags = 0; 

    // now create 2D texture from above structure 
    ID3D11Texture2D* pID3D11Texture2D_DepthStencilBuffer = NULL; 
    hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2DDesc, NULL, &pID3D11Texture2D_DepthStencilBuffer); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "resize() > gpID3D11Device::CreateTexture2D() failed\n"); 
        fclose(gpFile); 

        return (hr); 
    } 

    D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc; 
    ZeroMemory((void*)&d3d11DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_DESC)); 
    d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT; 
    d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    
    hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthStencilBuffer, &d3d11DepthStencilViewDesc, &gpID3D11DepthStencilView); 
    if(FAILED(hr)) 
    {
        gpFile = fopen(gszLogFileName, "a+"); 
        fprintf(gpFile, "resize() > gpID3D11Device::CreateDepthStencilView() failed\n"); 
        fclose(gpFile); 

        return (hr); 
    } 

    gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView); 

    // initialize viewport structure 
    D3D11_VIEWPORT d3d11Viewport; 
    ZeroMemory((void*)&d3d11Viewport, sizeof(D3D11_VIEWPORT)); 
    d3d11Viewport.TopLeftX = 0.0f; 
    d3d11Viewport.TopLeftY = 0.0f; 
    d3d11Viewport.Width = (float)width; 
    d3d11Viewport.Height = (float)height; 
    d3d11Viewport.MinDepth = 0.0f; 
    d3d11Viewport.MaxDepth = 1.0f; 

    // ste viewport in pipeline 
    gpID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport); 

    // set perspective projection matrix 
    perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.1f, 100.0f); 

    return (hr); 
} 

void display(void) 
{
    // code 
    gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, clearColor); 
    gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); 

    // set vertex buffers 
    UINT stride = sizeof(float) * 3; 
    UINT offset = 0; 
    gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer, &stride, &offset); 
    stride = sizeof(float) * 3; 
    offset = 0; 
    gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_ColorBuffer, &stride, &offset); 

    gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 

    XMMATRIX worldMatrix = XMMatrixIdentity(); 
    XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f); 
    XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(anglePyramid)); 
    worldMatrix = rotationMatrix * translationMatrix; 

    XMMATRIX viewMatrix = XMMatrixIdentity(); 
    XMMATRIX wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix; 

    // now push this matrix into vertex shader 
    CBUFFER constantBuffer; 
    ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER)); 
    constantBuffer.worldViewProjectionMatrix = wvpMatrix; 

    gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0); 

    // draw geometry 
    gpID3D11DeviceContext->Draw(12, 0); 

    gpIDXGISwapChain->Present(0, 0); 
} 

void update(void) 
{
    // code 
    if(anglePyramid >= 360.0f) 
        anglePyramid -= 360.0f; 
    anglePyramid += 0.4f; 
} 

void uninitialize(void) 
{
    // code 
    if(gpID3D11Buffer_ConstantBuffer) 
    {
        gpID3D11Buffer_ConstantBuffer->Release(); 
        gpID3D11Buffer_ConstantBuffer = NULL; 
    }
    if(gpID3D11Buffer_ColorBuffer) 
    {
        gpID3D11Buffer_ColorBuffer->Release(); 
        gpID3D11Buffer_ColorBuffer = NULL; 
    }
    if(gpID3D11Buffer_PositionBuffer) 
    {
        gpID3D11Buffer_PositionBuffer->Release(); 
        gpID3D11Buffer_PositionBuffer = NULL; 
    }

    if(gpID3D11PixelShader) 
    {
        gpID3D11PixelShader->Release(); 
        gpID3D11PixelShader = NULL; 
    } 
    if(gpID3D11VertexShader) 
    {
        gpID3D11VertexShader->Release(); 
        gpID3D11VertexShader = NULL; 
    } 

    if(gpID3D11DepthStencilView) 
    {
        gpID3D11DepthStencilView->Release(); 
        gpID3D11DepthStencilView = NULL; 
    }
    if(gpID3D11RenderTargetView) 
    {
        gpID3D11RenderTargetView->Release(); 
        gpID3D11RenderTargetView = NULL; 
    }

    if(gpID3D11DeviceContext) 
    {
        gpID3D11DeviceContext->Release(); 
        gpID3D11DeviceContext = NULL; 
    }

    if(gpIDXGISwapChain) 
    {
        gpIDXGISwapChain->Release(); 
        gpIDXGISwapChain = NULL; 
    }

    if(gpID3D11Device)  
    {
        gpID3D11Device->Release(); 
        gpID3D11Device = NULL; 
    }

    gpFile = fopen(gszLogFileName, "a+"); 
    fprintf(gpFile, "Program terminated successfully\n"); 
    fclose(gpFile); 
    gpFile = NULL; 
} 


