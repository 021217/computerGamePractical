#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <dinput.h>
using namespace std;

// Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;

// Initialization
LPDIRECT3DTEXTURE9 texture = NULL;
LPDIRECT3DTEXTURE9 pointer = NULL;
LPD3DXSPRITE sprite = NULL;

int R = 255, G = 255, B = 255;
int RI = -51, GI = -51, BI = -51;

int windowScreenWidth = 1280, windowScreenHeight = 720;
int fullScreenWidth = 1920, fullScreenHeight = 1080;
bool windowMode = true;

int defaultScreenWidth = windowScreenWidth, defaultScreenHeight = windowScreenHeight;

IDirect3D9* direct3D9 = NULL;
IDirect3DDevice9* d3dDevice = NULL;


D3DXVECTOR3 mousePos;


LPDIRECT3DTEXTURE9 numbersTexture = NULL;
D3DXVECTOR3 numberPosition;
RECT numbersRect;
int numberTextureWidth = 32;
int numberTextureHeight = 32;
int numberRow = 2;
int numberCol = 2;
int numberMaxFrame = 4;

int numberSpriteWidth = numberTextureWidth / numberCol;
int numberSpriteHeight = numberTextureHeight / numberRow;
int numbersFrame = 0;
int frameCounter = 0;


//Input
//	Direct Input object.
LPDIRECTINPUT8 dInput;
//	Direct Input keyboard device.
LPDIRECTINPUTDEVICE8  dInputKeyboardDevice;
LPDIRECTINPUTDEVICE8  dInputMouseDevice;
//	Key input buffer
BYTE  diKeys[256];
DIMOUSESTATE mouse_state;

// Initial time
DWORD lastTickCount = GetTickCount();
// Global variables for timing
LARGE_INTEGER frequency; // High-performance counter frequency
LARGE_INTEGER startTime; // Starting time

void InitializeTimer() {
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
}

float GetElapsedTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
}

void UpdateTimer() {
    QueryPerformanceCounter(&startTime);
}

int setLimit(int colorInt) {
    if (colorInt >= 255) colorInt = 255;
    else if (colorInt <= 0) colorInt = 0;
    return colorInt;
}
void cleanupWindow(HINSTANCE hInstance) {
    UnregisterClass(wndClass.lpszClassName, hInstance);
}

void cleanUpSprite() {
    if (sprite != NULL) sprite->Release();
    if (texture != NULL) texture->Release();
    if (pointer != NULL) pointer->Release();
    if (numbersTexture != NULL) numbersTexture->Release();
}

void cleanUpInputDevice() {
    //	Release keyboard device.
    dInputKeyboardDevice->Unacquire();
    dInputKeyboardDevice->Release();
    dInputKeyboardDevice = NULL;

    dInputMouseDevice->Unacquire();
    dInputMouseDevice->Release();
    dInputMouseDevice = NULL;

    //	Release DirectInput.
    dInput->Release();
    dInput = NULL;
}

void createDevice() {
    if (d3dDevice != NULL) {
        d3dDevice->Release();
        d3dDevice = NULL;
    }

    D3DPRESENT_PARAMETERS d3dPP;
    ZeroMemory(&d3dPP, sizeof(d3dPP));

    d3dPP.Windowed = false;
    d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dPP.BackBufferCount = 1;
    d3dPP.BackBufferWidth = defaultScreenWidth;
    d3dPP.BackBufferHeight = defaultScreenHeight;
    d3dPP.hDeviceWindow = g_hWnd;

    HRESULT hr = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

    if (FAILED(hr)) {
        cout << "Failed to create Direct3D device." << endl;
        PostQuitMessage(0);
    }
}

void createSprite() {
    cleanUpSprite();

    HRESULT hr = D3DXCreateSprite(d3dDevice, &sprite);
    if (FAILED(hr)) {
        cout << "Failed to create sprite" << endl;
    }

    /*hr = D3DXCreateTextureFromFileEx(d3dDevice, "numbers.bmp", D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 128, 0),
        NULL, NULL, &numbersTexture);*/
    hr = D3DXCreateTextureFromFile(d3dDevice, "hehe.png", &numbersTexture);
    numberPosition.x = 500;
    numberPosition.y = 500;

    numbersRect.top = 0;
    numbersRect.bottom = numbersRect.top + numberSpriteHeight;
    numbersRect.left = 0;
    numbersRect.right = numbersRect.left + numberSpriteWidth;


    if (FAILED(hr)) {
        cout << "Failed to load Numbers" << endl;
    }

    /*hr = D3DXCreateTextureFromFileEx(d3dDevice, "explosion.png", D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 128, 0),
        NULL, NULL, &explosionTexture);*/

    hr = D3DXCreateTextureFromFile(d3dDevice, "pointer.png", &pointer);
    mousePos.x = 0;
    mousePos.y = 0;
    if (FAILED(hr)) {
        cout << "Failed to load Pointer" << endl;
    }

}

void createInputDevice() {
    //	Create the Direct Input object.
    HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);
    //	Create the keyboard device.
    hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboardDevice, NULL);
    hr = dInput->CreateDevice(GUID_SysMouse, &dInputMouseDevice, NULL);
    //	Set the input data format.
    dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    dInputMouseDevice->SetDataFormat(&c_dfDIMouse);

    //	Set the cooperative level.
    //	To Do:
    //	Try with different combination.
    dInputKeyboardDevice->SetCooperativeLevel(g_hWnd , DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    dInputMouseDevice->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

    /*---
        For buffered data
        //	Define buffer for input.
        //const int DEVICE_BUFFER_SIZE = 4;
        //DIDEVICEOBJECTDATA deviceBuffer[DEVICE_BUFFER_SIZE];

        ////	Set the event buffer / properties.
        //DIPROPDWORD dipdw;
        //dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        //dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        //dipdw.diph.dwObj = 0;
        //dipdw.diph.dwHow = DIPH_DEVICE;
        //dipdw.dwData = DEVICE_BUFFER_SIZE;

        //hr = dInputKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
    --*/

    //	Acquire the device.
    dInputKeyboardDevice->Acquire();
    dInputMouseDevice->Acquire();
}

void getInput() {
    //	Get immediate Keyboard Data.
    dInputKeyboardDevice->GetDeviceState(256, diKeys);
    dInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
}

void render() {
    // Calculate elapsed time
    float elapsedTime = GetElapsedTime();
    UpdateTimer();
    cout << "Elapsed Time: " << elapsedTime << endl;

    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);
    d3dDevice->BeginScene();

    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    numbersRect.top = ((frameCounter % numberMaxFrame) / numberCol) * numberSpriteHeight;
    numbersRect.bottom = numbersRect.top + numberSpriteHeight;
    numbersRect.left = ((frameCounter % numberMaxFrame) % numberRow) * numberSpriteWidth;
    numbersRect.right = numbersRect.left + numberSpriteWidth;

    // Define scaling factors
    float scaleFactor = 2.0f; // Scale the sprite 2x its original size
    float spriteSpeed = 0.0001f; // Base speed for movement

    // Draw multiple instances of the sprite with random movement and scaling
    for (int i = 0; i < 20; ++i) { // Draw 20 instances
        // Random position and scaling
        float randomX = (rand() % 201 - 100) / 100.0f; // Random float between -1 and 1
        float randomY = (rand() % 201 - 100) / 100.0f; // Random float between -1 and 1

        D3DXVECTOR3 position((float)(rand() % 800), (float)(rand() % 600), 0.0f);
        position.x += randomX * spriteSpeed * elapsedTime;
        position.y += randomY * spriteSpeed * elapsedTime;

        // Set up a scaling transformation
        D3DXMATRIX scalingMatrix;
        D3DXMatrixScaling(&scalingMatrix, scaleFactor, scaleFactor, 1.0f);
        sprite->SetTransform(&scalingMatrix);

        // Draw the scaled sprite
        sprite->Draw(numbersTexture, &numbersRect, NULL, &position, D3DCOLOR_XRGB(255, 255, 255));

        // Reset the transformation to avoid affecting other sprites
        D3DXMatrixIdentity(&scalingMatrix);
        sprite->SetTransform(&scalingMatrix);
    }

    
    D3DXVECTOR3 spritePosition((float)mousePos.x, (float)mousePos.y, 0.0f);
    sprite->Draw(pointer, NULL, NULL, &spritePosition, D3DCOLOR_XRGB(255, 255, 255));


    sprite->End();



    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void update() {

    if (diKeys[DIK_UP])
    {
        frameCounter++;
    }
    if (diKeys[DIK_DOWN])
    {
        frameCounter--;
    }
    /*if (diKeys[DIK_R])
    {
        R = setLimit(R);
        if (R == 0 || R == 255) RI *= -1;
        R += RI;
    }
    if (diKeys[DIK_G])
    {
        G = setLimit(G);
        if (G == 0 || G == 255) GI *= -1;
        G += GI;
    }
    if (diKeys[DIK_B])
    {
        B = setLimit(B);
        if (B == 0 || B == 255) BI *= -1;
        B += BI;
    }*/
    if (diKeys[DIK_F])
    {
        // Forbidden Code, U press ur pc die
        windowMode = !windowMode;
        defaultScreenWidth = windowMode ? windowScreenWidth : fullScreenWidth;
        defaultScreenHeight = windowMode ? windowScreenHeight : fullScreenHeight;
        SetWindowPos(g_hWnd, NULL, 0, 0, defaultScreenWidth, defaultScreenHeight, SWP_NOMOVE | SWP_NOZORDER);
        UpdateWindow(g_hWnd);
        createDevice();
        createSprite();
    }
    if (diKeys[DIK_X] || diKeys[DIK_ESCAPE])
    {
        PostQuitMessage(0);
    }

    R = setLimit(R);
    if (R == 0 || R == 255) RI *= -1;
    R += RI;

    G = setLimit(G);
    if (G == 0 || G == 255) GI *= -1;
    G += GI;

    B = setLimit(B);
    if (B == 0 || B == 255) BI *= -1;
    B += BI;

    cout << "R: " << R << " G: " << G << " B: " << B << endl;
    mousePos.x += mouse_state.lX;
    mousePos.y += mouse_state.lY;
    

    /*if (numbersFrame > numberMaxFrame) {
        numbersFrame = 0;
    }
    if (explosionFrame > explosionMaxFrame) {
        explosionFrame = 0;
    }*/

}


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;


    case WM_MOUSEMOVE:
        /*mousePos.x = (short)LOWORD(lParam);
        mousePos.y = (short)HIWORD(lParam);*/
        cout << "X: " << mousePos.x << " Y: " << mousePos.y << endl;
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void createWindow(HINSTANCE hInstance) {
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.hCursor = LoadCursor(NULL, NULL);
    wndClass.hInstance = hInstance;
    wndClass.lpfnWndProc = WindowProcedure;
    wndClass.lpszClassName = "My Window";
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wndClass);
    g_hWnd = CreateWindowEx(0, wndClass.lpszClassName, "Mein Kahf", WS_OVERLAPPEDWINDOW, 0, 100, defaultScreenWidth, defaultScreenHeight, NULL, NULL, hInstance, NULL);
    ShowWindow(g_hWnd, 1);
    ZeroMemory(&msg, sizeof(msg));
}

bool windowIsRunning() {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    createWindow(hInstance);

    direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    createDevice();
    createSprite();
    createInputDevice();
    InitializeTimer();
    while (windowIsRunning()) {
        getInput();
        update();
        render();
    }

    d3dDevice->Release();
    direct3D9->Release();
    cleanUpSprite();
    cleanupWindow(hInstance);
    cleanUpInputDevice();

    return 0;
}