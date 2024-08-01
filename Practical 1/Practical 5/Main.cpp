#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;

// Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;

// Initialization
LPD3DXSPRITE sprite = NULL;
LPD3DXFONT font = NULL;
LPD3DXLINE line = NULL;

int R = 0, G = 0, B = 0;
int RI = -5, GI = -5, BI = -5;

int windowScreenWidth = 1280, windowScreenHeight = 720;
int fullScreenWidth = 1920, fullScreenHeight = 1080;
bool windowMode = true;

int defaultScreenWidth = windowScreenWidth, defaultScreenHeight = windowScreenHeight;

IDirect3D9* direct3D9 = NULL;
IDirect3DDevice9* d3dDevice = NULL;


D3DXVECTOR3 mousePos;
D3DXVECTOR2 clockRotationSeconds = { 500,100 };
D3DXVECTOR2 clockRotationMinutes = { 500,150 };
D3DXVECTOR2 clockRotationHours = { 500,200 };
D3DXVECTOR2 rotationCenterPoint = { 500, 300 };
float myPI = 2 * M_PI;
float rotationDuration = 60.0f;
float rotationAnglePerSecond = myPI / rotationDuration;
float rotationAnglePerMinute = myPI / rotationDuration / rotationDuration;
float rotationAnglePerHour = myPI / rotationDuration / rotationDuration / rotationDuration;
// Initial time
DWORD lastTickCount = GetTickCount();




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
    if (font != NULL) font->Release();
    if (line != NULL) line->Release();
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


    hr = D3DXCreateFont(d3dDevice, 25, 0, 0, 1, false,
        DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

    hr = D3DXCreateLine(d3dDevice, &line);
    

}

void render() {
    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);
    d3dDevice->BeginScene();

    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    RECT textRect;
    textRect.left = 100;
    textRect.top = 100;
    textRect.right = 300;
    textRect.bottom = 125;
    font->DrawText(sprite, "Hello World!", 12, &textRect, 0, D3DCOLOR_XRGB(255, 255, 255));

    //	Define the line vertices.
    D3DXVECTOR2 lineSeconds[] = { D3DXVECTOR2(rotationCenterPoint.x, rotationCenterPoint.y), D3DXVECTOR2(clockRotationSeconds.x, clockRotationSeconds.y) };
    D3DXVECTOR2 lineMinutes[] = { D3DXVECTOR2(rotationCenterPoint.x, rotationCenterPoint.y), D3DXVECTOR2(clockRotationMinutes.x, clockRotationMinutes.y) };
    D3DXVECTOR2 lineHours[] = { D3DXVECTOR2(rotationCenterPoint.x, rotationCenterPoint.y), D3DXVECTOR2(clockRotationHours.x, clockRotationHours.y) };

    //	Begin to draw the lines.
    line->Begin();
    line->Draw(lineSeconds, 2, D3DCOLOR_XRGB(255, 255, 255));
    line->Draw(lineMinutes, 2, D3DCOLOR_XRGB(255, 0, 0));
    line->Draw(lineHours, 2, D3DCOLOR_XRGB(0, 255, 0));
    line->End();

    sprite->End();



    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void generateAngle() {
    // Get the current system time
    DWORD currentTickCount = GetTickCount();
    // Calculate elapsed time in seconds
    float elapsedTime = (currentTickCount - lastTickCount) / 1000.0f;
    lastTickCount = currentTickCount;

    // Calculate the current rotation angle increment
    float rotationAngleSeconds = rotationAnglePerSecond * elapsedTime;
    float rotationAngleMinutes = rotationAnglePerMinute * elapsedTime;
    float rotationAngleHours = rotationAnglePerHour * elapsedTime;

    float relativeSecondsX = clockRotationSeconds.x - rotationCenterPoint.x;
    float relativeSecondsY = clockRotationSeconds.y - rotationCenterPoint.y;
    float relativeMinutesX = clockRotationMinutes.x - rotationCenterPoint.x;
    float relativeMinutesY = clockRotationMinutes.y - rotationCenterPoint.y;
    float relativeHoursX = clockRotationHours.x - rotationCenterPoint.x;
    float relativeHoursY = clockRotationHours.y - rotationCenterPoint.y;

    float newSecondsX = rotationCenterPoint.x + relativeSecondsX * cos(rotationAngleSeconds) - relativeSecondsY * sin(rotationAngleSeconds);
    float newSecondsY = rotationCenterPoint.y + relativeSecondsX * sin(rotationAngleSeconds) + relativeSecondsY * cos(rotationAngleSeconds);
    float newMinutesX = rotationCenterPoint.x + relativeMinutesX * cos(rotationAngleMinutes) - relativeMinutesY * sin(rotationAngleMinutes);
    float newMinutesY = rotationCenterPoint.y + relativeMinutesX * sin(rotationAngleMinutes) + relativeMinutesY * cos(rotationAngleMinutes);
    float newHoursX = rotationCenterPoint.x + relativeHoursX * cos(rotationAngleHours) - relativeHoursY * sin(rotationAngleHours);
    float newHoursY = rotationCenterPoint.y + relativeHoursX * sin(rotationAngleHours) + relativeHoursY * cos(rotationAngleHours);

    clockRotationSeconds.x = newSecondsX;
    clockRotationSeconds.y = newSecondsY;
    clockRotationMinutes.x = newMinutesX;
    clockRotationMinutes.y = newMinutesY;
    clockRotationHours.x = newHoursX;
    clockRotationHours.y = newHoursY;
}

void update() {
    
    generateAngle(); 

}


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        /*cout << "Current BG: " << defaultBg << endl;*/
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
            //cout << "Full Screen Mode: " << !windowMode << " Width: " << defaultScreenWidth << " Height: " << defaultScreenHeight << endl;
            break;

        }

        break;

    case WM_MOUSEMOVE:
        mousePos.x = (short)LOWORD(lParam);
        mousePos.y = (short)HIWORD(lParam);
        //cout << "X: " << mousePos.x << " Y: " << mousePos.y << endl;
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