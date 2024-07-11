//DON'T COPY MY CODE, IT'S JUST FOR YOUR REFERENCE - JOHN
// Ask the compiler to include minimal header files for our program.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// include the Direct3D 9 library
#include <d3d9.h>
#include <iostream>
using namespace std;

//--------------------------------------------------------------------

// Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;

// This section is dynamic which you can look it as game setting

// Default Value For RGB
int R = 0;
int G = 0;
int B = 0;

// Increment of RGB
int RI = -5;
int GI = -5;
int BI = -5;

// Default Screen Size For Windowed and Fullscreen
int windowScreenWidth = 1280;
int windowScreenHeight = 720;
int fullScreenWidth = 1920;
int fullScreenHeight = 1080;
bool windowMode = true;

// Application of Screen Size
int defaultScreenWidth = windowScreenWidth;
int defaultScreenHeight = windowScreenHeight;

// Set Variable Pointer
IDirect3D9* direct3D9 = NULL;
IDirect3DDevice9* d3dDevice = NULL;

// Function that make sure RGB does not exceed below 0 and above 255
int setLimit(int colorInt) {
	if (colorInt >= 255) {
		colorInt = 255;
	}
	else if (colorInt <= 0) {
		colorInt = 0;
	}
	return colorInt;
}

// Function that create virtual graphics device
// It was used to update the resolution of the graphics
// Might have more configuration can be explored in the future
void createDevice() {
	if (d3dDevice != NULL) {
		d3dDevice->Release();
		d3dDevice = NULL;
	}

	// Define how the screen presents.
	D3DPRESENT_PARAMETERS d3dPP;
	ZeroMemory(&d3dPP, sizeof(d3dPP));

	d3dPP.Windowed = windowMode;
	d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dPP.BackBufferCount = 1;
	d3dPP.BackBufferWidth = defaultScreenWidth;
	d3dPP.BackBufferHeight = defaultScreenHeight;
	d3dPP.hDeviceWindow = g_hWnd;

	// Initiate the Virtual Graphics Card
	HRESULT hr = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

	if (FAILED(hr)) {
		cout << "Failed to create Direct3D device." << endl;
		PostQuitMessage(0);
	}
}

// Window Procedure, for event handling
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// The message is post when we destroy the window.
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		cout << "Current Width : " << defaultScreenWidth << " " << "Current Height : " << defaultScreenHeight << " " << endl;
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 0x58:
			PostQuitMessage(0);
			break;
		case 'R':
			R = setLimit(R);
			if (R == 0 || R == 255) {
				RI *= -1;
			}
			R += RI;
			break;
		case 'G':
			G = setLimit(G);
			if (G == 0 || G == 255) {
				GI *= -1;
			}
			G += GI;
			break;
		case 'B':
			B = setLimit(B);
			if (B == 0 || B == 255) {
				BI *= -1;
			}
			B += BI;
			break;
		//Full Screen Mode
		case 'F':
			if (windowMode) {
				windowMode = false;
				defaultScreenWidth = fullScreenWidth;
				defaultScreenHeight = fullScreenHeight;
			}
			else {
				windowMode = true;
				defaultScreenWidth = windowScreenWidth;
				defaultScreenHeight = windowScreenHeight;
			}
			// Function that setup the windows settings
			SetWindowPos(g_hWnd, NULL, 0, 0, defaultScreenWidth, defaultScreenHeight, SWP_NOMOVE | SWP_NOZORDER);
			// Function that update and refresh the current window
			UpdateWindow(g_hWnd);
			// Create new Virtual Graphics Card as the Window Screen Size were configured
			createDevice();
			cout << "Full Screen Mode: " << !windowMode << " " << "Width : " << defaultScreenWidth << " " << "Height : " << defaultScreenHeight << " " << endl;
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Generate new window for the game
void createWindow(HINSTANCE hInstance) {
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WindowProcedure;
	wndClass.lpszClassName = "My Window";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndClass);
	g_hWnd = CreateWindowEx(0, wndClass.lpszClassName, "Mein Kahf", WS_OVERLAPPEDWINDOW, 0, 100, defaultScreenWidth, defaultScreenHeight, NULL, NULL, hInstance, NULL);
	ShowWindow(g_hWnd, 1);
	ZeroMemory(&msg, sizeof(msg));
}

// Listening for input
bool windowIsRunning() {
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

// Clear memory before closing the application
void cleanupWindow(HINSTANCE hInstance) {
	UnregisterClass(wndClass.lpszClassName, hInstance);
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	createWindow(hInstance);

	direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	createDevice();

	while (windowIsRunning())
	{
		d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);
		d3dDevice->BeginScene();
		// Drawing.
		d3dDevice->EndScene();
		d3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	d3dDevice->Release();
	d3dDevice = NULL;
	direct3D9->Release();
	direct3D9 = NULL;
	cleanupWindow(hInstance);

	return 0;
}
