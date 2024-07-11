// WARNING!
// The following code is incomplete!
// You are required to merge it to your work in previous Lab.

//	include the Direct3D 9 library
#include <d3d9.h>
//--------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//	To Do: Ensure window is created successfully then only create D3D9 device.
	createWindow();

	//	Define Direct3D 9.
	// Create D3D Object - Use the D3D Class, instantiate an object
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
		//	To Do:
		//	Update.

		//	Clear the back buffer.
		d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

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

	cleanupWindow();

	return 0;
}
//--------------------------------------------------------------------