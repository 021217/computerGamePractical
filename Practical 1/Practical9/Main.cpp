#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <dinput.h>
#include <string>
#include <cmath> 
#include "Frametime.h"
using namespace std;

// Window handle
HWND g_hWnd = NULL;
WNDCLASS wndClass;
MSG msg;

// Initialization
LPDIRECT3DTEXTURE9 texture = NULL;
LPDIRECT3DTEXTURE9 pointer = NULL;
LPD3DXSPRITE sprite = NULL;
LPD3DXFONT font = NULL;

int R = 0, G = 0, B = 0;
int RI = -5, GI = -5, BI = -5;

int windowScreenWidth = 1280, windowScreenHeight = 720;
int fullScreenWidth = 1920, fullScreenHeight = 1080;
bool windowMode = true;

int defaultScreenWidth = windowScreenWidth, defaultScreenHeight = windowScreenHeight;

IDirect3D9* direct3D9 = NULL;
IDirect3DDevice9* d3dDevice = NULL;

Frametime* gameTimer = new Frametime;

D3DXVECTOR3 mousePos;


LPDIRECT3DTEXTURE9 player1Texture = NULL;
int player1TextureWidth = 64;
int player1TextureHeight = 64;
int player1Row = 2;
int player1Col = 2;
int player1MaxFrame = 2;
float player1Mass = 10.0f;
float player1EnginePower = 10.0f;

D3DXVECTOR2 player1EngineForce;
D3DXVECTOR2 player1Acceleration;
D3DXVECTOR2 player1Velocity;
D3DXVECTOR2 player1Position;
RECT player1Rect;
int player1SpriteWidth = player1TextureWidth / player1Col;
int player1SpriteHeight = player1TextureHeight / player1Row;
enum movementDirection { MOVEDOWN, MOVELEFT, MOVERIGHT, MOVEUP, IDLE };
int player1FrameCounter = 0;
int player1Direction = MOVEDOWN;
D3DXMATRIX player1Matrix;
float player1Rotation;
D3DXVECTOR2 player1Scaling(1.0f, 1.0f);
D3DXVECTOR2 player1SpriteCenter(player1SpriteWidth / 2, player1SpriteHeight / 2);

LPDIRECT3DTEXTURE9 player2Texture = NULL;
int player2TextureWidth = 64;
int player2TextureHeight = 64;
int player2Row = 2;
int player2Col = 2;
int player2MaxFrame = 2;
float player2Mass = 10.0f;
float player2EnginePower = 10.0f;

D3DXVECTOR2 player2EngineForce;
D3DXVECTOR2 player2Acceleration;
D3DXVECTOR2 player2Velocity;
D3DXVECTOR2 player2Position;
RECT player2Rect;
int player2SpriteWidth = player2TextureWidth / player2Col;
int player2SpriteHeight = player2TextureHeight / player2Row;
int player2FrameCounter = 0;
int player2Direction = MOVEDOWN;
D3DXMATRIX player2Matrix;
float player2Rotation;
D3DXVECTOR2 player2Scaling(1.0f, 1.0f);
D3DXVECTOR2 player2SpriteCenter(player2SpriteWidth / 2, player2SpriteHeight / 2);

float maxVelocity = 10.0f; // Define the maximum velocity


D3DXVECTOR2 gravity = D3DXVECTOR2(0, 2);
float friction = 0.01f;

//Input
//	Direct Input object.
LPDIRECTINPUT8 dInput;
//	Direct Input keyboard device.
LPDIRECTINPUTDEVICE8  dInputKeyboardDevice;
LPDIRECTINPUTDEVICE8  dInputMouseDevice;
//	Key input buffer
BYTE  diKeys[256];
DIMOUSESTATE mouse_state;

bool CircleCollision(D3DXVECTOR2 posA, D3DXVECTOR2 posB, float widthA, float widthB) {
    D3DXVECTOR2 distanceVector = posB - posA;

    if (widthA / 2 + widthB / 2 > D3DXVec2Length(&distanceVector)) {
        //Collision Detected
        return true;
    }
    else {
        return false;
    }
}

// Function to resolve collision
void resolveCollision(D3DXVECTOR2& pos1, D3DXVECTOR2& vel1, float mass1,
    D3DXVECTOR2& pos2, D3DXVECTOR2& vel2, float mass2) {
    // Normal vector (the direction of the collision)
    D3DXVECTOR2 normal = pos2 - pos1;
    float distance = sqrtf(normal.x * normal.x + normal.y * normal.y);

    // Prevent division by zero
    if (distance == 0.0f) distance = 0.1f;

    // Normalize the normal vector
    normal /= distance;

    // Relative velocity
    D3DXVECTOR2 relativeVelocity = vel2 - vel1;

    // Calculate the velocity along the normal direction
    float velocityAlongNormal = D3DXVec2Dot(&relativeVelocity, &normal);

    // If velocities are separating, skip the collision response
    if (velocityAlongNormal > 0) return;

    // Calculate the restitution (bounciness), set to 1 for a perfect elastic collision
    float restitution = 1.0f;

    // Calculate impulse scalar
    float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal;
    impulseMagnitude /= (1 / mass1 + 1 / mass2);

    // Apply impulse to both players
    D3DXVECTOR2 impulse = impulseMagnitude * normal;
    vel1 -= impulse / mass1;
    vel2 += impulse / mass2;
}

// Function to clamp the velocity to the maximum value
void clampVelocity(D3DXVECTOR2& velocity, float maxVelocity) {
    // Calculate the magnitude (speed) of the velocity vector
    float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);

    // If the speed exceeds the maximum velocity, clamp it
    if (speed > maxVelocity) {
        // Normalize the velocity and scale it to the maximum velocity
        velocity *= (maxVelocity / speed);
    }
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
    if (player1Texture != NULL) player1Texture->Release();
    if (player2Texture != NULL) player2Texture->Release();
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

    d3dPP.Windowed = true;
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

    /*hr = D3DXCreateTextureFromFileEx(d3dDevice, "player1.bmp", D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 128, 0),
        NULL, NULL, &player1Texture);*/
        /*hr = D3DXCreateTextureFromFile(d3dDevice, "hehe.png", &player1Texture);
        player1Position.x = 500;
        player1Position.y = 500;

        player1Rect.top = 0;
        player1Rect.bottom = player1Rect.top + player1priteHeight;
        player1Rect.left = 0;
        player1Rect.right = player1Rect.left + player1priteWidth;*/

    hr = D3DXCreateTextureFromFile(d3dDevice, "practical9.png", &player1Texture);
    player1Position.x = 500;
    player1Position.y = 500 ;

    player1Rect.top = 0;
    player1Rect.bottom = player1Rect.top + player1SpriteHeight;
    player1Rect.left = 0;
    player1Rect.right = player1Rect.left + player1SpriteWidth;

    player2Position.x = 800;
    player2Position.y = 500;

    player2Rect.top = 0;
    player2Rect.bottom = player2Rect.top + player2SpriteHeight;
    player2Rect.left = 0;
    player2Rect.right = player2Rect.left + player2SpriteWidth;


    if (FAILED(hr)) {
        cout << "Failed to load player1" << endl;
    }

    /*hr = D3DXCreateTextureFromFileEx(d3dDevice, "explosion.png", D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 128, 0),
        NULL, NULL, &explosionTexture);*/
    if (FAILED(hr)) {
        cout << "Failed to load Pointer" << endl;
    }

    hr = D3DXCreateFont(d3dDevice, 25, 0, 0, 1, false,
        DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

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
    dInputKeyboardDevice->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
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

    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(R, G, B), 1.0f, 0);
    d3dDevice->BeginScene();

    sprite->Begin(D3DXSPRITE_ALPHABLEND);

    // Update frame counter and reset if needed
    player1FrameCounter = (player1FrameCounter + 1) % player1MaxFrame;

    // Calculate frame rectangle for vertically aligned spritesheet
    player1Rect.top = (player1FrameCounter % player1Row) * player1SpriteHeight;
    player1Rect.bottom = player1Rect.top + player1SpriteHeight;
    player1Rect.left = ((player1FrameCounter / player1Row) % player1Col) * player1SpriteWidth;
    player1Rect.right = player1Rect.left + player1SpriteWidth;
    D3DXMatrixTransformation2D(&player1Matrix, NULL, 0.0, &player1Scaling, &player1SpriteCenter, player1Rotation, &player1Position);

    sprite->SetTransform(&player1Matrix);
    sprite->Draw(player1Texture, &player1Rect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

    // Update frame counter and reset if needed
    player2FrameCounter = (player2FrameCounter + 1) % player2MaxFrame;

    // Calculate frame rectangle for player2 on the second column of a vertically aligned spritesheet
    player2Rect.top = (player2FrameCounter % player2Row) * player2SpriteHeight;
    player2Rect.bottom = player2Rect.top + player2SpriteHeight;

    // Since player2 is in the second column, add player1SpriteWidth to shift to the second column
    player2Rect.left = player1SpriteWidth + ((player2FrameCounter / player2Row) % player2Col) * player2SpriteWidth;
    player2Rect.right = player2Rect.left + player2SpriteWidth;

    D3DXMatrixTransformation2D(&player2Matrix, NULL, 0.0, &player2Scaling, &player2SpriteCenter, player2Rotation, &player2Position);

    sprite->SetTransform(&player2Matrix);
    sprite->Draw(player1Texture, &player2Rect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));


    // Define scaling factors
    //float scaleFactor = 2.0f; // Scale the sprite 2x its original size
    //float spriteSpeed = 0.0001f; // Base speed for movement

    //// Draw multiple instances of the sprite with random movement and scaling
    //for (int i = 0; i < 20; ++i) { // Draw 20 instances
    //    // Random position and scaling
    //    float randomX = (rand() % 201 - 100) / 100.0f; // Random float between -1 and 1
    //    float randomY = (rand() % 201 - 100) / 100.0f; // Random float between -1 and 1

    //    D3DXVECTOR3 position((float)(rand() % 800), (float)(rand() % 600), 0.0f);
    //    position.x += randomX * spriteSpeed * elapsedTime;
    //    position.y += randomY * spriteSpeed * elapsedTime;

    //    // Set up a scaling transformation
    //    D3DXMATRIX scalingMatrix;
    //    D3DXMatrixScaling(&scalingMatrix, scaleFactor, scaleFactor, 1.0f);
    //    sprite->SetTransform(&scalingMatrix);

    //    // Draw the scaled sprite
    //    sprite->Draw(player1Texture, &player1Rect, NULL, &position, D3DCOLOR_XRGB(255, 255, 255));

    //    // Reset the transformation to avoid affecting other sprites
    //    D3DXMatrixIdentity(&scalingMatrix);
    //    sprite->SetTransform(&scalingMatrix);
    //}



    sprite->End();



    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void update(int frames) {

    for (int i = 0; i < frames; i++) {

        if (diKeys[DIK_LEFT] & 0x80) {
            player1Rotation -= 0.1;
        }

        if (diKeys[DIK_RIGHT] & 0x80) {
            player1Rotation += 0.1;
        }

        if (diKeys[DIK_UP] & 0x80) {
            player1EngineForce.x = sin(player1Rotation) * player1EnginePower;
            player1EngineForce.y = -cos(player1Rotation) * player1EnginePower;
            player1Acceleration = player1EngineForce / player1Mass;
        }

        if (diKeys[DIK_DOWN] & 0x80) {
            player1EngineForce.x = -sin(player1Rotation) * player1EnginePower;
            player1EngineForce.y = cos(player1Rotation) * player1EnginePower;
            player1Acceleration = player1EngineForce / player1Mass;
        }

        if (diKeys[DIK_A] & 0x80) {
            player2Rotation -= 0.1;
        }

        if (diKeys[DIK_D] & 0x80) {
            player2Rotation += 0.1;
        }

        if (diKeys[DIK_W] & 0x80) {
            player2EngineForce.x = sin(player2Rotation) * player2EnginePower;
            player2EngineForce.y = -cos(player2Rotation) * player2EnginePower;
            player2Acceleration = player2EngineForce / player2Mass;
        }

        if (diKeys[DIK_S] & 0x80) {
            player2EngineForce.x = -sin(player2Rotation) * player2EnginePower;
            player2EngineForce.y = cos(player2Rotation) * player2EnginePower;
            player2Acceleration = player2EngineForce / player2Mass;
        }

        // Clamp the velocity of both players
        clampVelocity(player1Velocity, maxVelocity);
        clampVelocity(player2Velocity, maxVelocity);


        // Boundary check for top of the screen
        if (player1Position.y < 0) {
            player1Position.y = 0;
            player1Velocity.y *= -1;  // Stop upward velocity
        }

        // Boundary check for bottom of the screen
        if (player1Position.y > windowScreenHeight - player1SpriteHeight) {
            player1Position.y = windowScreenHeight - player1SpriteHeight;
            player1Velocity.y *= -1;  // Stop downward velocity
        }

        // Boundary check for left of the screen
        if (player1Position.x < 0) {
            player1Position.x = 0;
            player1Velocity.x *= -1;  // Stop leftward velocity
        }

        // Boundary check for right of the screen
        if (player1Position.x > windowScreenWidth - player1SpriteWidth) {
            player1Position.x = windowScreenWidth - player1SpriteWidth;
            player1Velocity.x *= -1;  // Stop rightward velocity
            
        }

        // Boundary check for top of the screen
        if (player2Position.y < 0) {
            player2Position.y = 0;
            player2Velocity.y *= -1;
        }

        // Boundary check for bottom of the screen
        if (player2Position.y > windowScreenHeight - player2SpriteHeight) {
            player2Position.y = windowScreenHeight - player2SpriteHeight;
            player2Velocity.y *= -1;  // Stop downward velocity
        }

        // Boundary check for left of the screen
        if (player2Position.x < 0) {
            player2Position.x = 0;
            player2Velocity.x *= -1;  // Stop leftward velocity
        }

        // Boundary check for right of the screen
        if (player2Position.x > windowScreenWidth - player2SpriteWidth) {
            player2Position.x = windowScreenWidth - player2SpriteWidth;
            player2Velocity.x *= -1;  // Stop rightward velocity
        }
        
        player1Velocity += player1Acceleration;
        player1Position += player1Velocity;
        player1Acceleration = D3DXVECTOR2(0, 0);
        

        player2Velocity += player2Acceleration;
        player2Position += player2Velocity;
        player2Acceleration = D3DXVECTOR2(0, 0);


        if (CircleCollision(player1Position, player2Position, player1SpriteWidth, player2SpriteWidth)) {
            resolveCollision(player1Position, player1Velocity, player1Mass,
                player2Position, player2Velocity, player2Mass);
        }

        player1FrameCounter++;
        player2FrameCounter++;





    }
    if (diKeys[DIK_X] || diKeys[DIK_ESCAPE])
    {
        PostQuitMessage(0);
    }


    cout << "R: " << R << " G: " << G << " B: " << B << endl;
    mousePos.x += mouse_state.lX;
    mousePos.y += mouse_state.lY;

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
    ShowCursor(false);
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
    gameTimer->init(60);
    direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    createDevice();
    createSprite();
    createInputDevice();
    while (windowIsRunning()) {
        getInput();
        update(gameTimer->FramesToUpdate());
        render();
    }

    d3dDevice->Release();
    direct3D9->Release();
    cleanUpSprite();
    cleanupWindow(hInstance);
    cleanUpInputDevice();

    return 0;
}