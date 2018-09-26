// WindowsCommunication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>

#include <thread>
#include <iostream>
#include <chrono> 

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>
#include <xnamath.h>

///////////////**************DirectX11**************////////////////////
//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;

//Global Declarations - Others//
LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;
HRESULT hr;

const int Width = 300;
const int Height = 300;

//Function Prototypes//
bool InitializeDirect3d11App(HINSTANCE hInstance);
bool InitializeWindow1(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
void CleanUp();
bool InitScene();
void UpdateScene();
void DrawScene();
int messageloopwindow2();

//Vertex Structure and Vertex Layout (Input Layout)//
struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z)
		: pos(x, y, z) {}

	XMFLOAT3 pos;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);
///////////////**************DirectX11**************////////////////////

// Global variables

bool window1closed = false;
bool window2closed = false;
bool endprogram = false;

// The main window class name.  
static TCHAR szWindowClass1[] = _T("win32app1");
static TCHAR szWindowClass2[] = _T("win32app2");


// The string that appears in the application's title bar.  
static TCHAR szTitle1[] = _T("Win32 Guided Tour Application 1");
static TCHAR szTitle2[] = _T("Win32 Guided Tour Application 2");

HINSTANCE hInst1;
HINSTANCE hInst2;

HWND    hwnd1;
HWND    hwnd2;


// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc1(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc2(HWND, UINT, WPARAM, LPARAM);


// Thread
std::thread Window1Thread;
int RunWindow1Loop();

std::thread Window2Thread;
int RunWindow2Loop();

int CALLBACK main()
{
	Window1Thread = std::thread(&RunWindow1Loop);
	std::this_thread::sleep_for(std::chrono::milliseconds(150));

	Window2Thread = std::thread([](){
		 if (!InitializeWindow1(GetModuleHandle(NULL), 1, 500, 500, true))
		 {
			 MessageBox(0, L"Window Initialization - Failed",
				 L"Error", MB_OK);
			 return 0;
		 }

		 if (!InitializeDirect3d11App(GetModuleHandle(NULL)))    //Initialize Direct3D
		 {
			 MessageBox(0, L"Direct3D Initialization - Failed",
				 L"Error", MB_OK);
			 return 0;
		 }

		 if (!InitScene())    //Initialize our scene
		 {
			 MessageBox(0, L"Scene Initialization - Failed",
				 L"Error", MB_OK);
			 return 0;
		 }

		 messageloopwindow2();

		 CleanUp();
    });

	while (endprogram == false) {
		if (window1closed == true && window2closed == true) {
			endprogram = true;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}


	Window1Thread.join();
	Window2Thread.join();


	return 0;
}

int RunWindow1Loop()
{
	std::cout << "RunWindow1Loop\n";
	
	WNDCLASSEX wcex;
	// Message
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc1;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass1;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst1 = GetModuleHandle(NULL); // Store instance handle in our global variable  

								   // The parameters to CreateWindow explained:  
								   // szWindowClass: the name of the application  
								   // szTitle: the text that appears in the title bar  
								   // WS_OVERLAPPEDWINDOW: the type of window to create  
								   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
								   // 500, 100: initial size (width, length)  
								   // NULL: the parent of this window  
								   // NULL: this application does not have a menu bar  
								   // hInstance: the first parameter from WinMain  
								   // NULL: not used in this application  
	hwnd1 = CreateWindow(
		szWindowClass1,
		szTitle1,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	if (!hwnd1)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hwnd1, 1);
	UpdateWindow(hwnd1);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int RunWindow2Loop()
{
	std::cout << "RunWindow2Loop\n";

	WNDCLASSEX wcex;
	// Message
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc2;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass2;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst2 = GetModuleHandle(NULL); // Store instance handle in our global variable  

									// The parameters to CreateWindow explained:  
									// szWindowClass: the name of the application  
									// szTitle: the text that appears in the title bar  
									// WS_OVERLAPPEDWINDOW: the type of window to create  
									// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
									// 500, 100: initial size (width, length)  
									// NULL: the parent of this window  
									// NULL: this application does not have a menu bar  
									// hInstance: the first parameter from WinMain  
									// NULL: not used in this application  
	hwnd2 = CreateWindow(
		szWindowClass2,
		szTitle2,
		WS_CHILD | WS_VISIBLE
		| WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500,
		hwnd1, // parent
		NULL,
		GetModuleHandle(NULL),
		NULL
	);

	if (!hwnd2)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hwnd2, 1);
	UpdateWindow(hwnd2);

	if (!InitializeDirect3d11App(GetModuleHandle(NULL)))    //Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!InitScene())    //Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloopwindow2();

	CleanUp();
}

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
//  
LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World! 1");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.  
		// For this introduction, we just print out "Hello, World!"  
		// in the top left corner.  
		TextOut(hdc,
			5, 5,
			greeting, _tcslen(greeting));
		// End application-specific layout section.  

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		window1closed = true;
		break;
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			std::cout << "WndProc1 space bar" << std::endl;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World! 2");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.  
		// For this introduction, we just print out "Hello, World!"  
		// in the top left corner.  
		TextOut(hdc,
			5, 5,
			greeting, _tcslen(greeting));
		// End application-specific layout section.  

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		window2closed = true;
		break;
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			std::cout << "WndProc2 space bar" << std::endl;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

///////////////**************DirectX11**************////////////////////
bool InitializeWindow1(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	typedef struct _WNDCLASS {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc2;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"Lesson 4 - Begin Drawing",
		//WS_CHILD | WS_VISIBLE
		//| WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		hwnd1,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);

	return true;
}

void CleanUp()
{
	//Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	triangleVertBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
}

bool InitScene()
{
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	//Create the vertex buffer
	Vertex v[] =
	{
		Vertex(0.0f, 0.5f, 0.5f),
		Vertex(0.5f, -0.5f, 0.5f),
		Vertex(-0.5f, -0.5f, 0.5f),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);

	//Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &triangleVertBuffer, &stride, &offset);

	//Create the Input Layout
	d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);

	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);

	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	return true;
}

void UpdateScene()
{

}

void DrawScene()
{
	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	//Draw the triangle
	d3d11DevCon->Draw(3, 0);

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

int messageloopwindow2() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		BOOL PeekMessageL(
			LPMSG lpMsg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg
		);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// run game code            
			UpdateScene();
			DrawScene();
		}
	}
	return msg.wParam;
}


///////////////**************DirectX11**************////////////////////
