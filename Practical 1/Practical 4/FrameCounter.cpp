#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
using namespace std;

// Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;

// Initialization
LPDIRECT3DTEXTURE9 texture = NULL;
LPDIRECT3DTEXTURE9 pointer = NULL;
LPDIRECT3DTEXTURE9 bg1 = NULL;
LPDIRECT3DTEXTURE9 bg2 = NULL;
LPDIRECT3DTEXTURE9 bg3 = NULL;
LPD3DXSPRITE sprite = NULL;

int R = 0, G = 0, B = 0;
int RI = -5, GI = -5, BI = -5;

int windowScreenWidth = 1280, windowScreenHeight = 720;
int fullScreenWidth = 1920, fullScreenHeight = 1080;
bool windowMode = true;

int defaultScreenWidth = windowScreenWidth, defaultScreenHeight = windowScreenHeight;

IDirect3D9* direct3D9 = NULL;
IDirect3DDevice9* d3dDevice = NULL;

LPDIRECT3DTEXTURE9 bg[3];
LPDIRECT3DTEXTURE9 defaultBg;

D3DXVECTOR3 mousePos;


LPDIRECT3DTEXTURE9 numbersTexture = NULL;
D3DXVECTOR3 numberPosition;
RECT numbersRect;
int numberTextureWidth = 128;
int numberTextureHeight = 128;
int numberRow = 4;
int numberCol = 4;
int numberMaxFrame = 10;

int numberSpriteWidth = numberTextureWidth / numberCol;
int numberSpriteHeight = numberTextureHeight / numberRow;
int numbersFrame = 0;

LPDIRECT3DTEXTURE9 explosionTexture = NULL;
D3DXVECTOR3 explosionPosition;
RECT explosionRect;
int explosionTextureWidth = 256;
int explosionTextureHeight = 256;
int explosionRow = 4;
int explosionCol = 4;
int explosionMaxFrame = 16;

int explosionSpriteWidth = explosionTextureWidth / explosionCol;
int explosionSpriteHeight = explosionTextureHeight / explosionRow;
int explosionFrame = 0;

int frameCounter = 0;


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
    if (bg1 != NULL) bg1->Release();
    if (bg2 != NULL) bg2->Release();
    if (bg3 != NULL) bg3->Release();
    if (numbersTexture != NULL) numbersTexture->Release();
    if (explosionTexture != NULL) explosionTexture->Release();
}
void createDevice() {
    if (d3dDevice != NULL) {
        d3dDevice->Release();
        d3dDevice = NULL;
    }

    D3DPRESENT_PARAMETERS d3dPP;
    ZeroMemory(&d3dPP, sizeof(d3dPP));

    d3dPP.Windowed = windowMode;
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
    hr = D3DXCreateTextureFromFile(d3dDevice, "numbers.bmp", &numbersTexture);
    numberPosition.x = 100;
    numberPosition.y = 100;

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
    hr = D3DXCreateTextureFromFile(d3dDevice, "explosion.png", &explosionTexture);
    explosionPosition.x = 200;
    explosionPosition.y = 200;

    explosionRect.top = 0;
    explosionRect.bottom = explosionRect.top + explosionSpriteHeight;
    explosionRect.left = 0;
    explosionRect.right = explosionRect.left + explosionSpriteWidth;


    if (FAILED(hr)) {
        cout << "Failed to load explosion" << endl;
    }

    hr = D3DXCreateTextureFromFile(d3dDevice, "pointer.png", &pointer);
    mousePos.x = 0;
    mousePos.y = 0;
    if (FAILED(hr)) {
        cout << "Failed to load Pointer" << endl;
    }

    hr = D3DXCreateTextureFromFile(d3dDevice, "bg1.png", &bg1);
    hr = D3DXCreateTextureFromFile(d3dDevice, "bg2.png", &bg2);
    hr = D3DXCreateTextureFromFile(d3dDevice, "bg3.png", &bg3);
    bg[0] = defaultBg = bg1;
    bg[1] = bg2;
    bg[2] = bg3;

}

void render() {
    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);
    d3dDevice->BeginScene();

    sprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT spriteRect = { 0, 0, 500, 500 };
    sprite->Draw(defaultBg, &spriteRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

    numbersRect.top = (frameCounter % numberMaxFrame / numberCol) * numberSpriteHeight;
    numbersRect.bottom = numbersRect.top + numberSpriteHeight;
    numbersRect.left = (numbersFrame % numberRow) * numberSpriteWidth;
    numbersRect.right = numbersRect.left + numberSpriteWidth;
    sprite->Draw(numbersTexture, &numbersRect, NULL, &numberPosition, D3DCOLOR_XRGB(255, 255, 255));

    explosionRect.top = (frameCounter % explosionMaxFrame / explosionCol) * explosionSpriteHeight;
    explosionRect.bottom = explosionRect.top + explosionSpriteHeight;
    explosionRect.left = (explosionFrame % explosionRow) * explosionSpriteWidth;
    explosionRect.right = explosionRect.left + explosionSpriteWidth;
    sprite->Draw(explosionTexture, &explosionRect, NULL, &explosionPosition, D3DCOLOR_XRGB(255, 255, 255));

    D3DXVECTOR3 spritePosition((float)mousePos.x, (float)mousePos.y, 0.0f);
    sprite->Draw(pointer, NULL, NULL, &spritePosition, D3DCOLOR_XRGB(255, 255, 255));


    sprite->End();



    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void update() {
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

    case WM_KEYDOWN:
        cout << "Current BG: " << defaultBg << endl;
        switch (wParam) {
        case VK_ESCAPE:
        case 0x58: // 'X' key
            PostQuitMessage(0);
            break;
        case 'R':
            R = setLimit(R);
            if (R == 0 || R == 255) RI *= -1;
            R += RI;
            break;
        case 'G':
            G = setLimit(G);
            if (G == 0 || G == 255) GI *= -1;
            G += GI;
            break;
        case 'B':
            B = setLimit(B);
            if (B == 0 || B == 255) BI *= -1;
            B += BI;
            break;
        case 'F':
            // Forbidden Code, U press ur pc die
            windowMode = !windowMode;
            defaultScreenWidth = windowMode ? windowScreenWidth : fullScreenWidth;
            defaultScreenHeight = windowMode ? windowScreenHeight : fullScreenHeight;
            SetWindowPos(g_hWnd, NULL, 0, 0, defaultScreenWidth, defaultScreenHeight, SWP_NOMOVE | SWP_NOZORDER);
            UpdateWindow(g_hWnd);
            createDevice();
            createSprite();
            cout << "Full Screen Mode: " << !windowMode << " Width: " << defaultScreenWidth << " Height: " << defaultScreenHeight << endl;
            break;
        case '1':
            defaultBg = bg[0];
            break;
        case '2':
            defaultBg = bg[1];
            break;
        case '3':
            defaultBg = bg[2];
            break;
        case VK_UP:
            frameCounter++;
            break;

        }

        break;

    case WM_MOUSEMOVE:
        mousePos.x = (short)LOWORD(lParam);
        mousePos.y = (short)HIWORD(lParam);
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

    while (windowIsRunning()) {
        update();
        render();
    }

    d3dDevice->Release();
    direct3D9->Release();
    cleanUpSprite();
    cleanupWindow(hInstance);

    return 0;
}
