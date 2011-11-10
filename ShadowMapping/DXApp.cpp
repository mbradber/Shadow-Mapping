#include"DXApp.h"

DXApp::DXApp():
mDevice(0), mSwapChain(0), rtView(0), dsView(0), dsBuffer(0), font(0),
deltaA(0), deltaB(0), deltaTime(0)
{
	CLIENT_WIDTH = 800;
	CLIENT_HEIGHT = 600;

	mainWindow = NULL;
	mClearColor = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	fontColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	someRect.left = 0;
	someRect.top = 0;
	someRect.bottom = 5;
	someRect.right = 5;

	frames = 0;
	fps = 0;
	mpf = 0;

	resizing = false;

	QueryPerformanceFrequency((LARGE_INTEGER*)&cps);
}

DXApp::~DXApp()
{
	mDevice->Release();
	mSwapChain->Release();
	rtView->Release();
	dsView->Release();
	dsBuffer->Release();
	font->Release();

	mDevice = NULL;
	mSwapChain = NULL;
	rtView = NULL;
	dsView = NULL;
	dsBuffer = NULL;
	font = NULL;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DXApp* app = 0;
		
	switch( msg )
	{
		case WM_CREATE:
		{
			//get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			app = (DXApp*)cs->lpCreateParams;
			return 0;
		}
	}

	//don't process messages until the window has been created
	if( app )
		return app->cbProc(hwnd, msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DXApp::handleResize()
{
	//release the old views and the ds buffer
	rtView->Release();
	rtView = 0;
	dsView->Release();
	dsView = 0;
	dsBuffer->Release();
	dsBuffer = 0;

	//resize the swap chain and rebuild the render target
	mSwapChain->ResizeBuffers(1, CLIENT_WIDTH, CLIENT_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D10Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer));
	mDevice->CreateRenderTargetView(backBuffer, 0, &rtView);
	backBuffer->Release();
	backBuffer = NULL;

	//create the new depth stencil buffer and view
	D3D10_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = CLIENT_WIDTH;
	depthStencilDesc.Height    = CLIENT_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	mDevice->CreateTexture2D(&depthStencilDesc, 0, &dsBuffer);
	mDevice->CreateDepthStencilView(dsBuffer, 0, &dsView);

	//bind the render target view and the ds view to the pipeline
	mDevice->OMSetRenderTargets(1, &rtView, dsView);
	
	//set the new viewport
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = CLIENT_WIDTH;
	vp.Height   = CLIENT_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mDevice->RSSetViewports(1, &vp);
}

LRESULT DXApp::cbProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_ACTIVATE:
		return 0;
	case WM_SIZE:
		CLIENT_WIDTH = LOWORD(lp);
		CLIENT_HEIGHT = HIWORD(lp);
		if(wp == SIZE_MAXIMIZED || wp == SIZE_RESTORED)
			handleResize();
		return 0;
	case WM_ENTERSIZEMOVE:
		resizing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		resizing = false;
		handleResize();
		return 0;
	case WM_LBUTTONDOWN:
		return 0;
	case WM_KEYDOWN:
		if(wp == VK_ESCAPE)
			DestroyWindow(mainWindow);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hw, msg, wp, lp);
}

bool DXApp::initWindow(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wDes;
	ZeroMemory(&wDes, sizeof(WNDCLASS));

	wDes.style = CS_HREDRAW | CS_VREDRAW;
	wDes.lpfnWndProc = MainWndProc;
	wDes.cbClsExtra = 0;
	wDes.cbWndExtra = 0;
	wDes.hInstance = instanceHandle;
	wDes.hIcon = LoadIcon(0, IDI_APPLICATION);
	wDes.hCursor = LoadCursor(0, IDC_ARROW);
	wDes.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wDes.lpszClassName = L"First Window";

	RegisterClass(&wDes);

	mainWindow = CreateWindow(L"First Window", L"Shadow Map Demo", WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT, CLIENT_WIDTH, CLIENT_HEIGHT,
								0, 0, instanceHandle, this);

	if(mainWindow == NULL)
		return false;

	ShowWindow(mainWindow, show);
	UpdateWindow(mainWindow);

	return true;
}

void DXApp::initD3D()
{
	//create the device and swap chain
	DXGI_MODE_DESC md;
	DXGI_SAMPLE_DESC sd;
	DXGI_SWAP_CHAIN_DESC sc;

	md.Width = CLIENT_WIDTH;
	md.Height = CLIENT_HEIGHT;
	md.RefreshRate.Numerator = 60;
	md.RefreshRate.Denominator = 1;
	md.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	md.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.Count = 1;
	sd.Quality = 0;

	sc.BufferDesc = md;
	sc.SampleDesc = sd;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.BufferCount = 1;
	sc.OutputWindow = mainWindow;
	sc.Windowed = true;
	sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sc.Flags = 0;

	D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE, 0, 
									0, D3D10_SDK_VERSION, &sc, &mSwapChain, &mDevice);

	//create the initial rendre target view
	ID3D10Texture2D * backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer));
	mDevice->CreateRenderTargetView(backBuffer, 0, &rtView);
	backBuffer->Release();
	backBuffer = NULL;

	//create depth/stencil buffer and view
	D3D10_TEXTURE2D_DESC dsDesc;
	dsDesc.Width = CLIENT_WIDTH;
	dsDesc.Height = CLIENT_HEIGHT;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Usage = D3D10_USAGE_DEFAULT;
	dsDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;

	mDevice->CreateTexture2D(&dsDesc, 0, &dsBuffer);
	mDevice->CreateDepthStencilView(dsBuffer, 0, &dsView);

	//bind to output merger stage
	mDevice->OMSetRenderTargets(1, &rtView, dsView); 

	//set the viewport
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = CLIENT_WIDTH;
	vp.Height = CLIENT_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mDevice->RSSetViewports(1, &vp);

	//init font
	D3DX10_FONT_DESC fd;
	fd.Height = 24;
	fd.Width = 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = DEFAULT_CHARSET;
	fd.OutputPrecision = OUT_DEFAULT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy(fd.FaceName, L"Times New Roman");

	D3DX10CreateFontIndirect(mDevice, &fd, &font);
}

void DXApp::initDInput(HINSTANCE hi, HWND hWnd)
{
	DirectInput8Create(hi, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&din, NULL);
	//create keyboard device and mouse device
	din->CreateDevice(GUID_SysKeyboard, &dinKeyboard, NULL);
	din->CreateDevice(GUID_SysMouse, &dinMouse, NULL);
	//set data format for keyboard and mouse
	dinKeyboard->SetDataFormat(&c_dfDIKeyboard);
	dinMouse->SetDataFormat(&c_dfDIMouse);
	//set cooperative levels for keyboard and mouse
	dinKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dinMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}

void DXApp::detectInput()
{
	//access input devices
	dinKeyboard->Acquire();
	dinMouse->Acquire();
	//get the input data
	dinKeyboard->GetDeviceState(256, (LPVOID)keystate);
	dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
}

void DXApp::resetTargets()
{
	mDevice->OMSetRenderTargets(1, &rtView, dsView);

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = CLIENT_WIDTH;
	vp.Height = CLIENT_HEIGHT;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	mDevice->RSSetViewports(1, &vp);
}

void DXApp::updateScene(float delta)
{
	findRelativeMousePos();

	QueryPerformanceFrequency((LARGE_INTEGER*)&cps);

	frames++;
	std::wostringstream out;
	out.precision(4);
	float currTime = gameTimer.getGameTime();

	if(gameTimer.updateFPS())
	{
		fps = frames;
		frames = 0;

		mpf = (double)1/fps * 1000;
	}

	//out << L"Time: " << currTime << L"\n" 
	out	<<L"FPS: " << fps << L"\n" <<L"MPF: " << mpf;
	totalTime = out.str();
}

POINT DXApp::findRelativeMousePos()
{
	POINT mouseCoords;

	GetCursorPos(points);
	GetWindowRect(mainWindow, clientLoc);

	mouseCoords.x = points[0].x - clientLoc[0].left;
	mouseCoords.y = points[0].y - clientLoc[0].top;

	return mouseCoords;
}

void DXApp::draw()
{
	mDevice->ClearRenderTargetView(rtView, mClearColor);
	mDevice->ClearDepthStencilView(dsView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	font->DrawText(0, totalTime.c_str(), -1, &someRect, DT_NOCLIP, fontColor);
}

int DXApp::run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(msg.message!=WM_QUIT)
	{

		QueryPerformanceCounter((LARGE_INTEGER*)&deltaA);

		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			updateScene(deltaTime);
			draw();
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&deltaB);

		deltaTime = (float)(deltaB - deltaA) / cps;
	}

	return (int)msg.wParam;
}