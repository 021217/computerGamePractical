//	Ask the compiler to include minimal header files for our program.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
//	include the Direct3D 9 library
#include <d3d9.h>

//--------------------------------------------------------------------

//	Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;
int R = 0;
int G = 0;
int B = 0;

int RI = 5;
int GI = 5;
int BI = 5;
//--------------------------------------------------------------------

//	Window Procedure, for event handling
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//	The message is post when we destroy the window.
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 0x58:
			PostQuitMessage(0);
			break;
		case 'R':
			if (R < 0 || R > 255) {
				RI *= -1;
			}
			R += RI;
			break;
		case 'G':
			if (G < 0 || G > 255) {
				GI *= -1;
			}
			G += GI;
			break;
		case 'B':
			if (B < 0 || B > 255) {
				BI *= -1;
			}
			B += BI;
			break;
			}
	
		
		/*
			Write your code here...
		*/

		//	Default handling for other messages.
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//--------------------------------------------------------------------

//	use int main if you want to have a console to print out message
//int main()
void createWindow(HINSTANCE hInstance) {
	//	Window's structure
	// Make a box to store how we want our game window to be like
	// This is NOT THE CLASS, IT IS JUST THE BOX to store how we want our game window class to be like
	
	//	Sset all members in wndClass to 0.
	// Make Sure that the memory within the box is clear before use
	ZeroMemory(&wndClass, sizeof(wndClass));

	//	Filling wndClass. You are to refer to MSDN for each of the members details.
	//	These are the fundamental structure members to be specify, in order to create your window.
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hInstance = hInstance;	//	GetModuleHandle(NULL);
	wndClass.lpfnWndProc = WindowProcedure;
	wndClass.lpszClassName = "My Window";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	//	Register the window.
	// 	We create the Game Class here
	RegisterClass(&wndClass);

	/*
		Step 2
		Create the Window.
	*/
	//	You are to refer to MSDN for each of the parameters details.
	g_hWnd = CreateWindowEx(0, wndClass.lpszClassName, "Mein Kahf", WS_OVERLAPPEDWINDOW, 0, 100, 400, 300, NULL, NULL, hInstance, NULL);
	ShowWindow(g_hWnd, 1);
	ZeroMemory(&msg, sizeof(msg));
}

bool windowIsRunning() {
	// Make a box for messages
	
	// Delete Everything in the box and replace it with 0
	

	/*
		Step 3
		Handling window messages
		NOTE! There are several ways of writing the message loop.
		You have to based on which kind of application your are writing.
	*/
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void cleanupWindow(HINSTANCE hInstance){
	UnregisterClass(wndClass.lpszClassName, hInstance);
}


//	use WinMain if you don't want the console
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	createWindow(hInstance);

	//Run Direct X Stuff
	IDirect3D9* direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	//	Define how the screen presents.
	// create a box to store how we want our graphic card to be like
	D3DPRESENT_PARAMETERS d3dPP;
	// Delete everything in the box
	ZeroMemory(&d3dPP, sizeof(d3dPP));

	//	Refer to Direct3D 9 documentation for the meaning of the members.
	// Fill up the box with how you want your graphic card to be
	d3dPP.Windowed = true;
	d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dPP.BackBufferCount = 1;
	// Make sure width and height matches for original solution but windows and buffer is a little different
	d3dPP.BackBufferWidth = 400;
	d3dPP.BackBufferHeight = 300;
	d3dPP.hDeviceWindow = g_hWnd;

	// Create a pointer to our virtual graphic card
	IDirect3DDevice9* d3dDevice;
	//	Create a Direct3D 9 device.							 D3DDEVTYPE_REF running in CPU instead (If your GPU cacat)
	HRESULT hr = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

	//	To Do: Cout out the message to indicate the failure.
	if (FAILED(hr))
		return 0;

	while (windowIsRunning())
	{
		// Do something… 
		//	Clear the back buffer.
		d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);

		//	Begin the scene
		// Unlock the canvas / buffer / paper
		d3dDevice->BeginScene();

		//	To Do:
		//	Drawing.

		//	End the scene
		// Lock back the canvas / buffer / paper
		d3dDevice->EndScene();

		//	Present the back buffer to screen
		d3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	//	Release the device when exiting.
	d3dDevice->Release();
	//	Reset pointer to NULL, a good practice.
	d3dDevice = NULL;
	cleanupWindow(hInstance);

	

	return 0;
}
//--------------------------------------------------------------------