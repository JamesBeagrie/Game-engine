// James Beagrie
// Last update: 01/12/2023
// Began coding: 06/09/2023


// Game engine.cpp : Defines the entry point for the application.
//
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include "framework.h"
#include "Game engine.h"
#include "ExtraMaths.h"
#include "object.h"
#include <malloc.h>
#include <iostream>
#include <math.h>

#include "Triangle.cpp"
#include "Player.cpp"
#include "Enemy.h"
#include "Button.cpp"
#include "Slider.cpp"
#include "inventory.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HBITMAP hBitmap;

//maybe??? 
int screenWidth = 0;
int screenHeight = 0;

bool escapeDown = false;
bool eDown = false;

POINT cursorPos;

// CURRENT TEXTURES
//  0 - tree bark
//  1 - leaves
//  2 - stone1
//  3 - stone2
//  4 - wood
//  5 - dirt
//  6 - grass

TEXTURE textures[11];

TEXTURE** font1;

TEXTURE mainMenuTexture;
TEXTURE gameLogoTexture;
TEXTURE buttonTexture;
TEXTURE buttonHoverTexture;
TEXTURE inventoryTexture;
TEXTURE iconHolderTexture;

TEXTURE crosshairTexture;
TEXTURE HPbarTexture;
TEXTURE HPfullTexture;

TEXTURE rocketLauncherTexture;
TEXTURE handGunTexture;

TEXTURE sliderTexture;
TEXTURE sliderHolderTexture;

weapon rocketLauncher;
weapon handGun;

weapon** weaponArr;

//No idea why this has two **
Object** objects;

Enemy** enemies;

Button** buttons;
Slider** sliders;

//Object triangleEnemy;

//setting up triangle array
Triangle** gameMap1;
Triangle** tree;

Object* triangleEnemy;

Player player;

int frameCount = 0;
int totalFrameCount = 0;

int wheelDeltaG = 0;

int fps = 0;

float* zBuffer = nullptr;
int currentBufferSize;

unsigned char* bits;

DWORD lastTime;

size_t size;

const int numObjects = 5;
const int numButtons = 11;
const int numWeapons = 2;
const int numSliders = 2;

int currentMode = 0; //The current screen of the game.
// 0 - main menu
// 1 - enter game
// 2 - Pause (draws map but does not update)
// 3 - Inventory
// 4 - Settings
// 5 - end program
// 6 - in the game settings

int currentMap = 0; //The current map
// 0 - test map

int maxEnemies = 1;
int currentEnemies = 0;

int deltaX = 0;
int deltaY = 0;

void newEnemy(Enemy* newEnemy) {
    if (currentEnemies >= maxEnemies) {
        int previousMaxEnemies = maxEnemies;

        maxEnemies *= 2;
        Enemy** enemiesCopy = new Enemy*[maxEnemies];

        for (int i = 0; i < previousMaxEnemies; i++) {
            enemiesCopy[i] = enemies[i];
        }

        delete[] enemies;

        enemies = enemiesCopy;
    }

    // Add the enemy newEnemy
    enemies[currentEnemies] = newEnemy;
    currentEnemies++;
}

void deleteEnemies() {
    int aliveEnemies = 0;

    for (int i = 0; i < currentEnemies; i++) {
        if (enemies[i]->getState() == 0) {
            // The enemy is dead
            delete enemies[i];
            enemies[i] = nullptr;
        }
        else {
            aliveEnemies++;
        }
    }

    for (int i = 0; i < currentEnemies - 1; i++) {
        bool swapped = false;

        for (int j = 1; j < currentEnemies - i; j++) {
            if (!enemies[j - 1] && enemies[j]) {
                Enemy* temp = enemies[j - 1];
                enemies[j - 1] = enemies[j];
                enemies[j] = temp;
                swapped = true;
            }
        }

        if (swapped == false) {
            break;
        }
    }

    currentEnemies = aliveEnemies;
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    screenWidth = 1920;
    screenHeight = 1080;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZE | WS_OVERLAPPEDWINDOW,
        0, 0, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);

    HDC hDC = GetDC(hWnd);
    hBitmap = CreateCompatibleBitmap(hDC, screenWidth, screenHeight);

    //AllocConsole();
    //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    std::cout << sizeof(uint32_t) << std::endl;

    size = screenWidth * screenHeight * 4;
    bits = (unsigned char*)malloc(size);

    //textures[3].manualTexture(1, 1); //stone
    //textures[7].manualTexture(1, 1); //grass
    //textures[5].manualTexture(1, 1); //stone 2
    //textures[6].manualTexture(1, 1); //dirt 

    //do stuff maybe???
    const char* filename2 = "Texture Files/stone.bmp";
    textures[3].read(filename2); // Read the BMP file into the texture object

    const char* filename = "Texture Files/grass.bmp";
    textures[7].read(filename); // Read the BMP file into the texture object

    const char* filename3 = "Texture Files/darkStone.bmp";
    textures[5].read(filename3); // Read the BMP file into the texture object

    const char* filename4 = "Texture Files/dirt.bmp";
    textures[6].read(filename4); // Read the BMP file into the texture object

    const char* filename5 = "Texture Files/bark.bmp";
    textures[0].read(filename5); // Read the BMP file into the texture object

    const char* filename6 = "Texture Files/leaves.bmp";
    textures[1].read(filename6); // Read the BMP file into the texture object

    const char* filename7 = "Texture Files/dirt.bmp";
    textures[4].read(filename7); // Read the BMP file into the texture object

    const char* filename8 = "Texture Files/metal1.bmp";
    textures[2].read(filename8); // Read the BMP file into the texture object

    const char* filename9 = "Texture Files/red.bmp";
    textures[8].read(filename9); // Read the BMP file into the texture object

    const char* filename10 = "Texture Files/yellow.bmp";
    textures[9].read(filename10); // Read the BMP file into the texture object

    const char* filename11 = "Texture Files/black.bmp";
    textures[10].read(filename11); // Read the BMP file into the texture object


    font1 = new TEXTURE * [37];

    for (int i = 0; i < 37; i++) {
        font1[i] = new TEXTURE();
    }

    //Put letters into array
    const char* filePathA = "Texture Files/Font1/A.bmp";
    const char* filePathB = "Texture Files/Font1/B.bmp";
    const char* filePathC = "Texture Files/Font1/C.bmp";
    const char* filePathD = "Texture Files/Font1/D.bmp";
    const char* filePathE = "Texture Files/Font1/E.bmp";
    const char* filePathF = "Texture Files/Font1/F.bmp";
    const char* filePathG = "Texture Files/Font1/G.bmp";
    const char* filePathH = "Texture Files/Font1/H.bmp";
    const char* filePathI = "Texture Files/Font1/I.bmp";
    const char* filePathJ = "Texture Files/Font1/J.bmp";
    const char* filePathK = "Texture Files/Font1/K.bmp";
    const char* filePathL = "Texture Files/Font1/L.bmp";
    const char* filePathM = "Texture Files/Font1/M.bmp";
    const char* filePathN = "Texture Files/Font1/N.bmp";
    const char* filePathO = "Texture Files/Font1/O.bmp";
    const char* filePathP = "Texture Files/Font1/P.bmp";
    const char* filePathQ = "Texture Files/Font1/Q.bmp";
    const char* filePathR = "Texture Files/Font1/R.bmp";
    const char* filePathS = "Texture Files/Font1/S.bmp";
    const char* filePathT = "Texture Files/Font1/T.bmp";
    const char* filePathU = "Texture Files/Font1/U.bmp";
    const char* filePathV = "Texture Files/Font1/V.bmp";
    const char* filePathW = "Texture Files/Font1/W.bmp";
    const char* filePathX = "Texture Files/Font1/X.bmp";
    const char* filePathY = "Texture Files/Font1/Y.bmp";
    const char* filePathZ = "Texture Files/Font1/Z.bmp";

    const char* filePathEmpty = "Texture Files/Font1/empty.bmp";

    const char* filePath0 = "Texture Files/Font1/0.bmp";
    const char* filePath1 = "Texture Files/Font1/1.bmp";
    const char* filePath2 = "Texture Files/Font1/2.bmp";
    const char* filePath3 = "Texture Files/Font1/3.bmp";
    const char* filePath4 = "Texture Files/Font1/4.bmp";
    const char* filePath5 = "Texture Files/Font1/5.bmp";
    const char* filePath6 = "Texture Files/Font1/6.bmp";
    const char* filePath7 = "Texture Files/Font1/7.bmp";
    const char* filePath8 = "Texture Files/Font1/8.bmp";
    const char* filePath9 = "Texture Files/Font1/9.bmp";

    font1[0]->read(filePathA);
    font1[1]->read(filePathB);
    font1[2]->read(filePathC);
    font1[3]->read(filePathD);
    font1[4]->read(filePathE);
    font1[5]->read(filePathF);
    font1[6]->read(filePathG);
    font1[7]->read(filePathH);
    font1[8]->read(filePathI);
    font1[9]->read(filePathJ);
    font1[10]->read(filePathK);
    font1[11]->read(filePathL);
    font1[12]->read(filePathM);
    font1[13]->read(filePathN);
    font1[14]->read(filePathO);
    font1[15]->read(filePathP);
    font1[16]->read(filePathQ);
    font1[17]->read(filePathR);
    font1[18]->read(filePathS);
    font1[19]->read(filePathT);
    font1[20]->read(filePathU);
    font1[21]->read(filePathV);
    font1[22]->read(filePathW);
    font1[23]->read(filePathX);
    font1[24]->read(filePathY);
    font1[25]->read(filePathZ);

    font1[26]->read(filePathEmpty);

    font1[27]->read(filePath0);
    font1[28]->read(filePath1);
    font1[29]->read(filePath2);
    font1[30]->read(filePath3);
    font1[31]->read(filePath4);
    font1[32]->read(filePath5);
    font1[33]->read(filePath6);
    font1[34]->read(filePath7);
    font1[35]->read(filePath8);
    font1[36]->read(filePath9);


    const char* mainMenuLocation = "Texture Files/MainPage.bmp";
    mainMenuTexture.read(mainMenuLocation);

    const char* gameLogoLocation = "Texture Files/gameLogo.bmp";
    gameLogoTexture.read(gameLogoLocation);

    const char* buttonLocation = "Texture Files/Button.bmp";
    buttonTexture.read(buttonLocation);

    const char* buttonHoverLocation = "Texture Files/Button - hover.bmp";
    buttonHoverTexture.read(buttonHoverLocation);

    const char* inventoryLocation = "Texture Files/inventory.bmp";
    inventoryTexture.read(inventoryLocation);

    const char* iconHolderLocation = "Texture Files/iconHolder.bmp";
    iconHolderTexture.read(iconHolderLocation);


    const char* crosshairLocation = "Texture Files/crosshair.bmp";
    crosshairTexture.read(crosshairLocation);

    const char* HPbarTextureLocation = "Texture Files/HPbar.bmp";
    HPbarTexture.read(HPbarTextureLocation);

    const char* HPfullTextureLocation = "Texture Files/HPfull.bmp";
    HPfullTexture.read(HPfullTextureLocation);


    const char* rocketLauncherLocation = "Texture Files/rocketLauncher.bmp";
    rocketLauncherTexture.read(rocketLauncherLocation);

    const char* handGunLocation = "Texture Files/handgun.bmp";
    handGunTexture.read(handGunLocation);

    const char* sliderLocation = "Texture Files/slider.bmp";
    sliderTexture.read(sliderLocation);

    const char* sliderHolderLocation = "Texture Files/sliderHolder.bmp";
    sliderHolderTexture.read(sliderHolderLocation);

    /*
    textures[3].pixels[0] = 208;
    textures[3].pixels[1] = 214;
    textures[3].pixels[2] = 219;
    textures[3].pixels[3] = 255;

    textures[7].pixels[0] = 86;
    textures[7].pixels[1] = 184;
    textures[7].pixels[2] = 33;
    textures[7].pixels[3] = 255;

    textures[6].pixels[0] = 37;
    textures[6].pixels[1] = 81;
    textures[6].pixels[2] = 122;
    textures[6].pixels[3] = 255;

    textures[5].pixels[0] = 130;
    textures[5].pixels[1] = 118;
    textures[5].pixels[2] = 112;
    textures[5].pixels[3] = 255;
    */

    // Weapons that the player and enemies will use 
    /*
     float damage;
    float bulletSpeed;
    int maxBullets;
    int currentBullets;
    int timer;
    int fireRate;

    TEXTURE icon;
    Object* bullet3D;
    Object* hit3D;
   
    bullet bulletArr[MAX_BULLETS]; // Define an array for bullets
    int bulletIndex; // Track the current bullet index
    */

    //float damagep, float bulletSpeedp, bool bouncesp, int maxBulletsp, int currentBulletsp, TEXTURE* iconp, Object* hit3Dp, Object* bullet3Dp, float fireRatep, char* namep
    weaponArr = new weapon * [numWeapons];

    Object* handgunBullet3D = new Object(4, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    handgunBullet3D->triangles[0] = *new Triangle(0.0f, -2.0f, 0.0f, 0.0f, 1.0f, -6.0f, -4.0f, 0.0f, -2.0f, -1.0f, static_cast<float>(textures[10].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[10].height), static_cast<float>(textures[10].width), static_cast<float>(textures[10].height), textures[10]);
    handgunBullet3D->triangles[1] = *new Triangle(0.0f, 1.0f, -6.0f, -4.0f, 0.0f, -2.0f, -2.0f, 6.0f, 1.0f, 1.0f, static_cast<float>(textures[10].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[10].height), static_cast<float>(textures[10].width), static_cast<float>(textures[10].height), textures[10]);
    handgunBullet3D->triangles[2] = *new Triangle(-2.0f, 6.0f, 1.0f, 0.0f, -2.0f, 0.0f, -4.0f, 0.0f, -2.0f, -1.0f, static_cast<float>(textures[10].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[10].height), static_cast<float>(textures[10].width), static_cast<float>(textures[10].height), textures[10]);
    handgunBullet3D->triangles[3] = *new Triangle(0.0f, -2.0f, 0.0f, 0.0f, 1.0f, -6.0f, -2.0f, 6.0f, 1.0f, 1.0f, static_cast<float>(textures[10].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[10].height), static_cast<float>(textures[10].width), static_cast<float>(textures[10].height), textures[10]);
    
    handgunBullet3D->scale(1.0f, 0.5f, { 0,0,0 });

    Object* handgunHit3D = new Object(35, 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    handgunHit3D->triangles[0] = *new Triangle(0.0f, 0.0f, -7.0f, -5.0f, 0.0f, 5.0f, 6.0f, 0.0f, 3.0f, 1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[1] = *new Triangle(0.0f, 0.0f, -7.0f, -1.0f, 16.0f, -5.0f, -5.0f, 0.0f, 5.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[2] = *new Triangle(-5.0f, 0.0f, 5.0f, 6.0f, 0.0f, 3.0f, -1.0f, 16.0f, -5.0f, 1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[3] = *new Triangle(-1.0f, 16.0f, -5.0f, 0.0f, 0.0f, -7.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[4] = *new Triangle(0.0f, 0.0f, -7.0f, -5.0f, 0.0f, 5.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[5] = *new Triangle(0.0f, 0.0f, -7.0f, 0.0f, -13.0f, 3.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[6] = *new Triangle(6.0f, 0.0f, 3.0f, -5.0f, 0.0f, 5.0f, 0.0f, -13.0f, 3.0f, 1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[7] = *new Triangle(0.0f, -13.0f, 3.0f, -5.0f, 0.0f, 5.0f, 0.0f, 0.0f, -7.0f, 1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[8] = *new Triangle(0.0f, 0.0f, -7.0f, 6.0f, 0.0f, 3.0f, 8.0f, 4.0f, -4.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[9] = *new Triangle(6.0f, 0.0f, 3.0f, 8.0f, 4.0f, -4.0f, -1.0f, 16.0f, -5.0f, 1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[10] = *new Triangle(-1.0f, 16.0f, -5.0f, 6.0f, 0.0f, 3.0f, 8.0f, 4.0f, -4.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[11] = *new Triangle(0.0f, 0.0f, -7.0f, 8.0f, 4.0f, -4.0f, -1.0f, 16.0f, -5.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[12] = *new Triangle(0.0f, 0.0f, -7.0f, -5.0f, 0.0f, 5.0f, -7.0f, 0.0f, -4.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[13] = *new Triangle(-7.0f, 0.0f, -4.0f, 0.0f, -13.0f, 3.0f, 0.0f, 0.0f, -7.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[14] = *new Triangle(-7.0f, 0.0f, -4.0f, -5.0f, 0.0f, 5.0f, 0.0f, -13.0f, 3.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[15] = *new Triangle(6.0f, 0.0f, 3.0f, 0.0f, -13.0f, 3.0f, 4.0f, -3.0f, 9.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[16] = *new Triangle(6.0f, 0.0f, 3.0f, 4.0f, -3.0f, 9.0f, -5.0f, 0.0f, 5.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[17] = *new Triangle(0.0f, -13.0f, 3.0f, -5.0f, 0.0f, 5.0f, 4.0f, -3.0f, 9.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[18] = *new Triangle(8.0f, 4.0f, -4.0f, 0.0f, 0.0f, -7.0f, 4.0f, -4.0f, -11.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[19] = *new Triangle(4.0f, -4.0f, -11.0f, 0.0f, 0.0f, -7.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[20] = *new Triangle(8.0f, 4.0f, -4.0f, 4.0f, -4.0f, -11.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[21] = *new Triangle(6.0f, 0.0f, 3.0f, -5.0f, 0.0f, 5.0f, 1.0f, 8.0f, 8.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[22] = *new Triangle(1.0f, 8.0f, 8.0f, -1.0f, 16.0f, -5.0f, 6.0f, 0.0f, 3.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[23] = *new Triangle(-5.0f, 0.0f, 5.0f, 1.0f, 8.0f, 8.0f, -8.0f, 7.0f, 7.0f, 1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[24] = *new Triangle(1.0f, 8.0f, 8.0f, -8.0f, 7.0f, 7.0f, -1.0f, 16.0f, -5.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[25] = *new Triangle(-1.0f, 16.0f, -5.0f, -8.0f, 7.0f, 7.0f, -5.0f, 0.0f, 5.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[26] = *new Triangle(0.0f, -13.0f, 3.0f, -3.0f, -8.0f, 11.0f, 4.0f, -3.0f, 9.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[27] = *new Triangle(4.0f, -3.0f, 9.0f, -5.0f, 0.0f, 5.0f, -2.0f, -1.0f, 10.0f, 1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[28] = *new Triangle(4.0f, -3.0f, 9.0f, -2.0f, -1.0f, 10.0f, -3.0f, -8.0f, 11.0f, 1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[29] = *new Triangle(-3.0f, -8.0f, 11.0f, 0.0f, -13.0f, 3.0f, -5.0f, 0.0f, 5.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[30] = *new Triangle(-5.0f, 0.0f, 5.0f, -2.0f, -1.0f, 10.0f, -3.0f, -8.0f, 11.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[31] = *new Triangle(8.0f, 5.0f, 3.0f, 9.0f, 10.0f, 7.0f, 11.0f, 7.0f, 2.0f, 1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);
    handgunHit3D->triangles[32] = *new Triangle(8.0f, 5.0f, 3.0f, 11.0f, 5.0f, 3.0f, 11.0f, 7.0f, 2.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[33] = *new Triangle(11.0f, 7.0f, 2.0f, 11.0f, 5.0f, 3.0f, 9.0f, 10.0f, 7.0f, -1.0f, static_cast<float>(textures[9].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[9].height), static_cast<float>(textures[9].width), static_cast<float>(textures[9].height), textures[9]);
    handgunHit3D->triangles[34] = *new Triangle(9.0f, 10.0f, 7.0f, 11.0f, 5.0f, 3.0f, 8.0f, 5.0f, 3.0f, -1.0f, static_cast<float>(textures[8].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[8].height), static_cast<float>(textures[8].width), static_cast<float>(textures[8].height), textures[8]);

    handgunHit3D->scale(1.0f, 0.1f, { 0,0,0 });

    Object* copiedHandgunBullet3D = new Object(*handgunBullet3D);
    Object* copiedhandgunHit3D = new Object(*handgunHit3D);

    copiedhandgunHit3D->scale(1.0f, 5.0f, { 0,0,0 });

    
    weaponArr[0] = new weapon(150.0f, 80.0f, false, 20, &handGunTexture, handgunHit3D, handgunBullet3D, 5, "HANDGUN", { 5.0f,5.0f,5.0f }, 1.0f);
    weaponArr[1] = new weapon(1000.0f, 30.0f, false, 5, &rocketLauncherTexture, copiedhandgunHit3D, copiedHandgunBullet3D, 30, "LAUNCHER", { 6.0f,6.0f,6.0f }, 1.0f);
    
    player.setWeaponPtr(weaponArr[1]);
    player.setWeaponIndex(1);

    //Objects
    
    objects = new Object * [numObjects];
    
    objects[0] = new Object(694, 0, 0, 0, 0, 0, 1.0f); // map1
    objects[1] = new Object(264, 0, 0, 0, 0, 0, 1.0f); // tree

    triangleEnemy = new Object(4, 0, 0, 0, 0, 0, 1.0f);
    //Object* rocketLauncherObject = new Object(4, 0, 0, 0, 0, 0, 1.0f);

    GetCursorPos(&cursorPos);

    buttons = new Button * [numButtons];

    buttons[0] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.2), floor(screenWidth * 0.8), floor(screenHeight * 0.3), buttonHoverTexture, buttonTexture, 0, 1, "NEW GAME", floor(screenWidth * 0.4), floor(screenHeight * 0.22), floor(screenWidth * 0.75), floor(screenHeight * 0.28));
    buttons[1] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.35), floor(screenWidth * 0.8), floor(screenHeight * 0.45), buttonHoverTexture, buttonTexture, 0, 0, "LOAD GAME", floor(screenWidth * 0.4), floor(screenHeight * 0.37), floor(screenWidth * 0.75), floor(screenHeight * 0.43));
    buttons[2] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.5), floor(screenWidth * 0.8), floor(screenHeight * 0.6), buttonHoverTexture, buttonTexture, 0, 4, "SETTINGS", floor(screenWidth * 0.4), floor(screenHeight * 0.52), floor(screenWidth * 0.75), floor(screenHeight * 0.58));
    buttons[3] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.65), floor(screenWidth * 0.8), floor(screenHeight * 0.75), buttonHoverTexture, buttonTexture, 0, 5, "EXIT GAME", floor(screenWidth * 0.4), floor(screenHeight * 0.67), floor(screenWidth * 0.75), floor(screenHeight * 0.73));

    buttons[4] = new Button(0, 0, floor(screenWidth * 0.2), floor(screenHeight * 0.1), buttonHoverTexture, buttonTexture, 3, 1, "BACK", floor(screenWidth * 0.01), floor(screenHeight * 0.01), floor(screenWidth * 0.19), floor(screenHeight * 0.09));
    buttons[5] = new Button(floor(screenWidth * 0.2), 0, floor(screenWidth * 0.4), floor(screenHeight * 0.1), buttonHoverTexture, buttonTexture, 3, 0, "ITEMS", floor(screenWidth * 0.21), floor(screenHeight * 0.01), floor(screenWidth * 0.39), floor(screenHeight * 0.09));
    buttons[6] = new Button(floor(screenWidth * 0.4), 0, floor(screenWidth * 0.6), floor(screenHeight * 0.1), buttonHoverTexture, buttonTexture, 3, 0, "SHOP", floor(screenWidth * 0.41), floor(screenHeight * 0.01), floor(screenWidth * 0.59), floor(screenHeight * 0.09));
    buttons[7] = new Button(floor(screenWidth * 0.6), 0, floor(screenWidth * 0.8), floor(screenHeight * 0.1), buttonHoverTexture, buttonTexture, 3, 6, "SETTINGS", floor(screenWidth * 0.61), floor(screenHeight * 0.01), floor(screenWidth * 0.79), floor(screenHeight * 0.09));
    buttons[8] = new Button(floor(screenWidth * 0.8), 0, floor(screenWidth * 1), floor(screenHeight * 0.1), buttonHoverTexture, buttonTexture, 3, 5, "QUIT", floor(screenWidth * 0.81), floor(screenHeight * 0.01), floor(screenWidth * 0.99), floor(screenHeight * 0.09));
    
    buttons[9] = new Button(floor(screenWidth * 0.2), floor(screenHeight * 0.75), floor(screenWidth * 0.8), floor(screenHeight * 0.95), buttonHoverTexture, buttonTexture, 4, 0, "BACK", floor(screenWidth * 0.31), floor(screenHeight * 0.78), floor(screenWidth * 0.69), floor(screenHeight * 0.92));
    buttons[10] = new Button(floor(screenWidth * 0.2), floor(screenHeight * 0.75), floor(screenWidth * 0.8), floor(screenHeight * 0.95), buttonHoverTexture, buttonTexture, 6, 3, "BACK", floor(screenWidth * 0.31), floor(screenHeight * 0.78), floor(screenWidth * 0.69), floor(screenHeight * 0.92));
    
    sliders = new Slider * [numSliders];
    //Slider(int sxp, int syp, int exp, int eyp, TEXTURE& sliderp, TEXTURE& normalp, int modep, const char* namep, int sxtp, int sytp, int extp, int eytp, float* variablep, float maxValuep, float minValuep)

    float* FOVpointer = player.getFOVpointer();
    
    sliders[0] = new Slider(floor(screenWidth * 0.2), floor(screenHeight * 0.4), floor(screenWidth * 0.8), floor(screenHeight * 0.6), sliderTexture, sliderHolderTexture, 4, "FOV", floor(screenWidth * 0.21), floor(screenHeight * 0.43), floor(screenWidth * 0.39), floor(screenHeight * 0.57), FOVpointer, player.getMaxFOV(), player.getMinFOV());
    sliders[1] = new Slider(floor(screenWidth * 0.2), floor(screenHeight * 0.4), floor(screenWidth * 0.8), floor(screenHeight * 0.6), sliderTexture, sliderHolderTexture, 6, "FOV", floor(screenWidth * 0.21), floor(screenHeight * 0.43), floor(screenWidth * 0.39), floor(screenHeight * 0.57), FOVpointer, player.getMaxFOV(), player.getMinFOV());
    
    gameMap1 = new Triangle * [694];

    gameMap1[0] = new Triangle(-9.0f, 0.0f, 0.0f, 36.0f, 0.0f, 0.0f, -1.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[1] = new Triangle(36.0f, 0.0f, 0.0f, 44.0f, 0.0f, -25.0f, -1.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[2] = new Triangle(-1.0f, 0.0f, -25.0f, 44.0f, 0.0f, -25.0f, 2.0f, 0.0f, -57.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[3] = new Triangle(44.0f, 0.0f, -25.0f, 50.0f, 0.0f, -67.0f, 2.0f, 0.0f, -57.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[4] = new Triangle(50.0f, 0.0f, -67.0f, 2.0f, 0.0f, -57.0f, 43.0f, 0.0f, -94.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[5] = new Triangle(0.0f, 0.0f, -84.0f, 2.0f, 0.0f, -57.0f, 43.0f, 0.0f, -94.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[6] = new Triangle(43.0f, 0.0f, -94.0f, 20.0f, 0.0f, -128.0f, 0.0f, 0.0f, -84.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[7] = new Triangle(-11.0f, 0.0f, -97.0f, 0.0f, 0.0f, -84.0f, 20.0f, 0.0f, -128.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[8] = new Triangle(-29.0f, 0.0f, -108.0f, -11.0f, 0.0f, -97.0f, 20.0f, 0.0f, -128.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[9] = new Triangle(-6.0f, 0.0f, -147.0f, -29.0f, 0.0f, -108.0f, 20.0f, 0.0f, -128.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[10] = new Triangle(-22.0f, 0.0f, -151.0f, -29.0f, 0.0f, -108.0f, -6.0f, 0.0f, -147.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[11] = new Triangle(-56.0f, 0.0f, -127.0f, -29.0f, 0.0f, -108.0f, -22.0f, 0.0f, -151.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[12] = new Triangle(-68.0f, 0.0f, -154.0f, -56.0f, 0.0f, -127.0f, -22.0f, 0.0f, -151.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[13] = new Triangle(-22.0f, 0.0f, -151.0f, -31.0f, 0.0f, -162.0f, -68.0f, 0.0f, -154.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[14] = new Triangle(-61.0f, 0.0f, -188.0f, -68.0f, 0.0f, -154.0f, -31.0f, 0.0f, -162.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[15] = new Triangle(-31.0f, 0.0f, -162.0f, -25.0f, 0.0f, -176.0f, -61.0f, 0.0f, -188.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[16] = new Triangle(-61.0f, 0.0f, -188.0f, -36.0f, 0.0f, -212.0f, -25.0f, 0.0f, -176.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[17] = new Triangle(-14.0f, 0.0f, -214.0f, -36.0f, 0.0f, -212.0f, -25.0f, 0.0f, -176.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[18] = new Triangle(36.0f, 0.0f, 0.0f, 77.0f, 25.0f, 5.0f, 44.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[19] = new Triangle(-9.0f, 0.0f, 0.0f, -1.0f, 0.0f, -25.0f, -43.0f, 14.0f, -1.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[20] = new Triangle(-1.0f, 0.0f, -25.0f, 2.0f, 0.0f, -57.0f, -20.0f, 25.0f, -39.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[21] = new Triangle(0.0f, 0.0f, -84.0f, 2.0f, 0.0f, -57.0f, -14.0f, 34.0f, -65.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[22] = new Triangle(44.0f, 0.0f, -25.0f, 50.0f, 0.0f, -67.0f, 95.0f, 19.0f, -43.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[23] = new Triangle(50.0f, 0.0f, -67.0f, 43.0f, 0.0f, -94.0f, 78.0f, 26.0f, -90.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[24] = new Triangle(43.0f, 0.0f, -94.0f, 20.0f, 0.0f, -128.0f, 52.0f, 36.0f, -118.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[25] = new Triangle(20.0f, 0.0f, -128.0f, -6.0f, 0.0f, -147.0f, 15.0f, 48.0f, -145.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[26] = new Triangle(0.0f, 0.0f, -84.0f, -11.0f, 0.0f, -97.0f, -11.0f, 47.0f, -84.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[27] = new Triangle(-11.0f, 0.0f, -97.0f, -29.0f, 0.0f, -108.0f, -24.0f, 36.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[28] = new Triangle(-29.0f, 0.0f, -108.0f, -56.0f, 0.0f, -127.0f, -49.0f, 26.0f, -106.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[29] = new Triangle(-56.0f, 0.0f, -127.0f, -68.0f, 0.0f, -154.0f, -76.0f, 28.0f, -131.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[30] = new Triangle(-68.0f, 0.0f, -154.0f, -61.0f, 0.0f, -188.0f, -91.0f, 33.0f, -173.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[31] = new Triangle(-61.0f, 0.0f, -188.0f, -36.0f, 0.0f, -212.0f, -64.0f, 31.0f, -215.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[32] = new Triangle(-20.0f, 25.0f, -39.0f, -27.0f, 31.0f, -18.0f, -1.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[33] = new Triangle(-27.0f, 31.0f, -18.0f, -43.0f, 14.0f, -1.0f, -1.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[34] = new Triangle(79.0f, 26.0f, -21.0f, 95.0f, 19.0f, -43.0f, 44.0f, 0.0f, -25.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[35] = new Triangle(44.0f, 0.0f, -25.0f, 79.0f, 26.0f, -21.0f, 77.0f, 25.0f, 5.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[36] = new Triangle(77.0f, 25.0f, 5.0f, 79.0f, 26.0f, -21.0f, 88.0f, 65.0f, -2.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[37] = new Triangle(88.0f, 74.0f, -19.0f, 79.0f, 26.0f, -21.0f, 88.0f, 65.0f, -2.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[38] = new Triangle(88.0f, 65.0f, -2.0f, 88.0f, 74.0f, -19.0f, 72.0f, 90.0f, -5.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[39] = new Triangle(72.0f, 90.0f, -5.0f, 88.0f, 74.0f, -19.0f, 72.0f, 87.0f, -18.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[40] = new Triangle(-43.0f, 14.0f, -1.0f, -27.0f, 31.0f, -18.0f, -34.0f, 54.0f, -16.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[41] = new Triangle(-34.0f, 54.0f, -16.0f, -43.0f, 14.0f, -1.0f, -36.0f, 70.0f, -11.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[42] = new Triangle(-36.0f, 70.0f, -11.0f, -34.0f, 54.0f, -16.0f, -16.0f, 82.0f, -8.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[43] = new Triangle(-20.0f, 25.0f, -39.0f, -27.0f, 31.0f, -18.0f, -25.0f, 61.0f, -25.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[44] = new Triangle(-25.0f, 61.0f, -25.0f, -34.0f, 54.0f, -16.0f, -27.0f, 31.0f, -18.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[45] = new Triangle(-25.0f, 61.0f, -25.0f, -34.0f, 54.0f, -16.0f, -16.0f, 82.0f, -8.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[46] = new Triangle(-5.0f, 82.0f, -24.0f, -16.0f, 82.0f, -8.0f, -25.0f, 61.0f, -25.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[47] = new Triangle(-16.0f, 82.0f, -8.0f, -5.0f, 82.0f, -24.0f, 16.0f, 92.0f, -10.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[48] = new Triangle(16.0f, 92.0f, -10.0f, 25.0f, 92.0f, -29.0f, -5.0f, 82.0f, -24.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[49] = new Triangle(16.0f, 92.0f, -10.0f, 25.0f, 92.0f, -29.0f, 43.0f, 97.0f, -5.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[50] = new Triangle(55.0f, 97.0f, -23.0f, 25.0f, 92.0f, -29.0f, 43.0f, 97.0f, -5.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[51] = new Triangle(72.0f, 90.0f, -5.0f, 43.0f, 97.0f, -5.0f, 55.0f, 97.0f, -23.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[52] = new Triangle(72.0f, 90.0f, -5.0f, 72.0f, 87.0f, -18.0f, 55.0f, 97.0f, -23.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[53] = new Triangle(-20.0f, 25.0f, -39.0f, -14.0f, 34.0f, -65.0f, 2.0f, 0.0f, -57.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[54] = new Triangle(-11.0f, 63.0f, -52.0f, -14.0f, 34.0f, -65.0f, -20.0f, 25.0f, -39.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[55] = new Triangle(-2.0f, 74.0f, -39.0f, -20.0f, 25.0f, -39.0f, -25.0f, 61.0f, -25.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[56] = new Triangle(-11.0f, 63.0f, -52.0f, -2.0f, 74.0f, -39.0f, -20.0f, 25.0f, -39.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[57] = new Triangle(-5.0f, 82.0f, -24.0f, -2.0f, 74.0f, -39.0f, -25.0f, 61.0f, -25.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[58] = new Triangle(-2.0f, 74.0f, -39.0f, -5.0f, 82.0f, -24.0f, 25.0f, 92.0f, -29.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[59] = new Triangle(50.0f, 0.0f, -67.0f, 90.0f, 34.0f, -72.0f, 78.0f, 26.0f, -90.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[60] = new Triangle(90.0f, 34.0f, -72.0f, 95.0f, 19.0f, -43.0f, 50.0f, 0.0f, -67.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[61] = new Triangle(60.0f, 43.0f, -104.0f, 52.0f, 36.0f, -118.0f, 43.0f, 0.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[62] = new Triangle(43.0f, 0.0f, -94.0f, 60.0f, 43.0f, -104.0f, 78.0f, 26.0f, -90.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[63] = new Triangle(52.0f, 36.0f, -118.0f, 31.0f, 59.0f, -133.0f, 20.0f, 0.0f, -128.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[64] = new Triangle(20.0f, 0.0f, -128.0f, 31.0f, 59.0f, -133.0f, 15.0f, 48.0f, -145.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[65] = new Triangle(-11.0f, 47.0f, -84.0f, 0.0f, 0.0f, -84.0f, -14.0f, 34.0f, -65.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[66] = new Triangle(-7.0f, 70.0f, -66.0f, -14.0f, 34.0f, -65.0f, -11.0f, 47.0f, -84.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[67] = new Triangle(-14.0f, 34.0f, -65.0f, -7.0f, 70.0f, -66.0f, -11.0f, 63.0f, -52.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[68] = new Triangle(87.0f, 63.0f, -33.0f, 88.0f, 74.0f, -19.0f, 79.0f, 26.0f, -21.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[69] = new Triangle(95.0f, 19.0f, -43.0f, 87.0f, 63.0f, -33.0f, 79.0f, 26.0f, -21.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[70] = new Triangle(-13.0f, 47.0f, -92.0f, -11.0f, 47.0f, -84.0f, -11.0f, 0.0f, -97.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[71] = new Triangle(-13.0f, 47.0f, -92.0f, -11.0f, 0.0f, -97.0f, -24.0f, 36.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[72] = new Triangle(-35.0f, 46.0f, -98.0f, -24.0f, 36.0f, -94.0f, -29.0f, 0.0f, -108.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[73] = new Triangle(-29.0f, 0.0f, -108.0f, -49.0f, 26.0f, -106.0f, -35.0f, 46.0f, -98.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[74] = new Triangle(-61.0f, 46.0f, -117.0f, -49.0f, 26.0f, -106.0f, -56.0f, 0.0f, -127.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[75] = new Triangle(-56.0f, 0.0f, -127.0f, -76.0f, 28.0f, -131.0f, -61.0f, 46.0f, -117.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[76] = new Triangle(-82.0f, 46.0f, -151.0f, -91.0f, 33.0f, -173.0f, -68.0f, 0.0f, -154.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[77] = new Triangle(-68.0f, 0.0f, -154.0f, -76.0f, 28.0f, -131.0f, -82.0f, 46.0f, -151.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[78] = new Triangle(-82.0f, 51.0f, -193.0f, -61.0f, 0.0f, -188.0f, -91.0f, 33.0f, -173.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[79] = new Triangle(-64.0f, 31.0f, -215.0f, -61.0f, 0.0f, -188.0f, -82.0f, 51.0f, -193.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[80] = new Triangle(-6.0f, 0.0f, -147.0f, -22.0f, 0.0f, -151.0f, -4.0f, 60.0f, -151.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[81] = new Triangle(15.0f, 48.0f, -145.0f, -4.0f, 60.0f, -151.0f, -6.0f, 0.0f, -147.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[82] = new Triangle(-22.0f, 0.0f, -151.0f, -31.0f, 0.0f, -162.0f, -23.0f, 71.0f, -154.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[83] = new Triangle(-4.0f, 60.0f, -151.0f, -23.0f, 71.0f, -154.0f, -22.0f, 0.0f, -151.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[84] = new Triangle(76.0f, 73.0f, -48.0f, 95.0f, 19.0f, -43.0f, 87.0f, 63.0f, -33.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[85] = new Triangle(76.0f, 73.0f, -48.0f, 90.0f, 34.0f, -72.0f, 95.0f, 19.0f, -43.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[86] = new Triangle(87.0f, 63.0f, -33.0f, 76.0f, 73.0f, -48.0f, 73.0f, 87.0f, -29.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[87] = new Triangle(73.0f, 87.0f, -29.0f, 87.0f, 63.0f, -33.0f, 88.0f, 74.0f, -19.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[88] = new Triangle(88.0f, 74.0f, -19.0f, 72.0f, 87.0f, -18.0f, 73.0f, 87.0f, -29.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[89] = new Triangle(54.0f, 94.0f, -41.0f, 55.0f, 97.0f, -23.0f, 72.0f, 87.0f, -18.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[90] = new Triangle(72.0f, 87.0f, -18.0f, 73.0f, 87.0f, -29.0f, 54.0f, 94.0f, -41.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[91] = new Triangle(76.0f, 73.0f, -48.0f, 73.0f, 87.0f, -29.0f, 54.0f, 94.0f, -41.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[92] = new Triangle(19.0f, 89.0f, -50.0f, -5.0f, 82.0f, -24.0f, 25.0f, 92.0f, -29.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[93] = new Triangle(55.0f, 97.0f, -23.0f, 19.0f, 89.0f, -50.0f, 25.0f, 92.0f, -29.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[94] = new Triangle(54.0f, 94.0f, -41.0f, 55.0f, 97.0f, -23.0f, 19.0f, 89.0f, -50.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[95] = new Triangle(4.0f, 78.0f, -58.0f, -2.0f, 74.0f, -39.0f, -11.0f, 63.0f, -52.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[96] = new Triangle(-7.0f, 70.0f, -66.0f, -11.0f, 63.0f, -52.0f, 4.0f, 78.0f, -58.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[97] = new Triangle(4.0f, 78.0f, -58.0f, 19.0f, 89.0f, -50.0f, -2.0f, 74.0f, -39.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[98] = new Triangle(-10.0f, 74.0f, -94.0f, -13.0f, 47.0f, -92.0f, -11.0f, 47.0f, -84.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[99] = new Triangle(-35.0f, 46.0f, -98.0f, -24.0f, 36.0f, -94.0f, -10.0f, 74.0f, -94.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[100] = new Triangle(-10.0f, 74.0f, -94.0f, -13.0f, 47.0f, -92.0f, -24.0f, 36.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[101] = new Triangle(72.0f, 59.0f, -72.0f, 76.0f, 73.0f, -48.0f, 90.0f, 34.0f, -72.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[102] = new Triangle(90.0f, 34.0f, -72.0f, 78.0f, 26.0f, -90.0f, 60.0f, 61.0f, -87.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[103] = new Triangle(60.0f, 61.0f, -87.0f, 72.0f, 59.0f, -72.0f, 90.0f, 34.0f, -72.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[104] = new Triangle(78.0f, 26.0f, -90.0f, 60.0f, 43.0f, -104.0f, 60.0f, 61.0f, -87.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[105] = new Triangle(60.0f, 84.0f, -63.0f, 72.0f, 59.0f, -72.0f, 60.0f, 61.0f, -87.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[106] = new Triangle(60.0f, 84.0f, -63.0f, 72.0f, 59.0f, -72.0f, 76.0f, 73.0f, -48.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[107] = new Triangle(76.0f, 73.0f, -48.0f, 54.0f, 94.0f, -41.0f, 60.0f, 84.0f, -63.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[108] = new Triangle(42.0f, 92.0f, -61.0f, 60.0f, 84.0f, -63.0f, 54.0f, 94.0f, -41.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[109] = new Triangle(54.0f, 94.0f, -41.0f, 42.0f, 92.0f, -61.0f, 19.0f, 89.0f, -50.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[110] = new Triangle(47.0f, 70.0f, -107.0f, 60.0f, 43.0f, -104.0f, 52.0f, 36.0f, -118.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[111] = new Triangle(52.0f, 36.0f, -118.0f, 47.0f, 70.0f, -107.0f, 31.0f, 59.0f, -133.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[112] = new Triangle(5.0f, 82.0f, -139.0f, -23.0f, 71.0f, -154.0f, -4.0f, 60.0f, -151.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[113] = new Triangle(15.0f, 48.0f, -145.0f, -4.0f, 60.0f, -151.0f, 5.0f, 82.0f, -139.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[114] = new Triangle(18.0f, 74.0f, -119.0f, 5.0f, 82.0f, -139.0f, 15.0f, 48.0f, -145.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[115] = new Triangle(15.0f, 48.0f, -145.0f, 18.0f, 74.0f, -119.0f, 31.0f, 59.0f, -133.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[116] = new Triangle(31.0f, 59.0f, -133.0f, 18.0f, 74.0f, -119.0f, 47.0f, 70.0f, -107.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[117] = new Triangle(48.0f, 66.0f, -92.0f, 60.0f, 43.0f, -104.0f, 47.0f, 70.0f, -107.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[118] = new Triangle(48.0f, 66.0f, -92.0f, 60.0f, 43.0f, -104.0f, 60.0f, 61.0f, -87.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[119] = new Triangle(39.0f, 82.0f, -78.0f, 48.0f, 66.0f, -92.0f, 60.0f, 61.0f, -87.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[120] = new Triangle(60.0f, 61.0f, -87.0f, 39.0f, 82.0f, -78.0f, 60.0f, 84.0f, -63.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[121] = new Triangle(23.0f, 81.0f, -67.0f, 39.0f, 82.0f, -78.0f, 42.0f, 92.0f, -61.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[122] = new Triangle(42.0f, 92.0f, -61.0f, 39.0f, 82.0f, -78.0f, 60.0f, 84.0f, -63.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[123] = new Triangle(7.0f, 80.0f, -64.0f, 23.0f, 81.0f, -67.0f, 19.0f, 89.0f, -50.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[124] = new Triangle(19.0f, 89.0f, -50.0f, 23.0f, 81.0f, -67.0f, 42.0f, 92.0f, -61.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[125] = new Triangle(19.0f, 89.0f, -50.0f, 7.0f, 80.0f, -64.0f, 4.0f, 78.0f, -58.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[126] = new Triangle(7.0f, 80.0f, -64.0f, 4.0f, 78.0f, -58.0f, -7.0f, 70.0f, -66.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[127] = new Triangle(5.0f, 74.0f, -81.0f, -10.0f, 74.0f, -94.0f, -11.0f, 47.0f, -84.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[128] = new Triangle(-11.0f, 47.0f, -84.0f, -7.0f, 70.0f, -66.0f, 5.0f, 74.0f, -81.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[129] = new Triangle(5.0f, 74.0f, -81.0f, 7.0f, 80.0f, -64.0f, -7.0f, 70.0f, -66.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[130] = new Triangle(-42.0f, 59.0f, -116.0f, -61.0f, 46.0f, -117.0f, -49.0f, 26.0f, -106.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[131] = new Triangle(-42.0f, 59.0f, -116.0f, -49.0f, 26.0f, -106.0f, -35.0f, 46.0f, -98.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[132] = new Triangle(-26.0f, 64.0f, -108.0f, -42.0f, 59.0f, -116.0f, -35.0f, 46.0f, -98.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[133] = new Triangle(-35.0f, 46.0f, -98.0f, -26.0f, 64.0f, -108.0f, -10.0f, 74.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[134] = new Triangle(19.0f, 79.0f, -79.0f, 23.0f, 81.0f, -67.0f, 7.0f, 80.0f, -64.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[135] = new Triangle(5.0f, 74.0f, -81.0f, 7.0f, 80.0f, -64.0f, 19.0f, 79.0f, -79.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[136] = new Triangle(32.0f, 79.0f, -93.0f, 48.0f, 66.0f, -92.0f, 47.0f, 70.0f, -107.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[137] = new Triangle(47.0f, 70.0f, -107.0f, 18.0f, 74.0f, -119.0f, 32.0f, 79.0f, -93.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[138] = new Triangle(32.0f, 79.0f, -93.0f, 48.0f, 66.0f, -92.0f, 39.0f, 82.0f, -78.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[139] = new Triangle(28.0f, 78.0f, -83.0f, 23.0f, 81.0f, -67.0f, 19.0f, 79.0f, -79.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[140] = new Triangle(28.0f, 78.0f, -83.0f, 23.0f, 81.0f, -67.0f, 39.0f, 82.0f, -78.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[141] = new Triangle(32.0f, 79.0f, -93.0f, 39.0f, 82.0f, -78.0f, 28.0f, 78.0f, -83.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[142] = new Triangle(-61.0f, 72.0f, -135.0f, -82.0f, 46.0f, -151.0f, -76.0f, 28.0f, -131.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[143] = new Triangle(-76.0f, 28.0f, -131.0f, -61.0f, 46.0f, -117.0f, -61.0f, 72.0f, -135.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[144] = new Triangle(-68.0f, 64.0f, -166.0f, -61.0f, 72.0f, -135.0f, -82.0f, 46.0f, -151.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[145] = new Triangle(-82.0f, 46.0f, -151.0f, -91.0f, 33.0f, -173.0f, -68.0f, 64.0f, -166.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[146] = new Triangle(-82.0f, 51.0f, -193.0f, -68.0f, 64.0f, -166.0f, -91.0f, 33.0f, -173.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[147] = new Triangle(-60.0f, 78.0f, -196.0f, -82.0f, 51.0f, -193.0f, -64.0f, 31.0f, -215.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[148] = new Triangle(-82.0f, 51.0f, -193.0f, -60.0f, 78.0f, -196.0f, -68.0f, 64.0f, -166.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[149] = new Triangle(-41.0f, 61.0f, -132.0f, -42.0f, 59.0f, -116.0f, -61.0f, 46.0f, -117.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[150] = new Triangle(-61.0f, 46.0f, -117.0f, -61.0f, 72.0f, -135.0f, -41.0f, 61.0f, -132.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[151] = new Triangle(15.0f, 75.0f, -92.0f, 5.0f, 74.0f, -81.0f, 19.0f, 79.0f, -79.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[152] = new Triangle(28.0f, 78.0f, -83.0f, 19.0f, 79.0f, -79.0f, 15.0f, 75.0f, -92.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[153] = new Triangle(15.0f, 75.0f, -92.0f, 32.0f, 79.0f, -93.0f, 28.0f, 78.0f, -83.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[154] = new Triangle(18.0f, 74.0f, -119.0f, 15.0f, 75.0f, -92.0f, 32.0f, 79.0f, -93.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[155] = new Triangle(15.0f, 75.0f, -92.0f, -10.0f, 74.0f, -94.0f, 5.0f, 74.0f, -81.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[156] = new Triangle(4.0f, 77.0f, -106.0f, 15.0f, 75.0f, -92.0f, 18.0f, 74.0f, -119.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[157] = new Triangle(4.0f, 77.0f, -106.0f, -10.0f, 74.0f, -94.0f, 15.0f, 75.0f, -92.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[158] = new Triangle(1.0f, 84.0f, -124.0f, 18.0f, 74.0f, -119.0f, 5.0f, 82.0f, -139.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[159] = new Triangle(1.0f, 84.0f, -124.0f, 18.0f, 74.0f, -119.0f, 4.0f, 77.0f, -106.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[160] = new Triangle(-14.0f, 79.0f, -134.0f, 5.0f, 82.0f, -139.0f, -23.0f, 71.0f, -154.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[161] = new Triangle(-14.0f, 79.0f, -134.0f, 5.0f, 82.0f, -139.0f, 1.0f, 84.0f, -124.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[162] = new Triangle(-8.0f, 84.0f, -109.0f, 4.0f, 77.0f, -106.0f, -10.0f, 74.0f, -94.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[163] = new Triangle(-10.0f, 74.0f, -94.0f, -8.0f, 84.0f, -109.0f, -26.0f, 64.0f, -108.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[164] = new Triangle(4.0f, 77.0f, -106.0f, 1.0f, 84.0f, -124.0f, -8.0f, 84.0f, -109.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[165] = new Triangle(-23.0f, 84.0f, -122.0f, -8.0f, 84.0f, -109.0f, -14.0f, 79.0f, -134.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[166] = new Triangle(-14.0f, 79.0f, -134.0f, -8.0f, 84.0f, -109.0f, 1.0f, 84.0f, -124.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[167] = new Triangle(-23.0f, 84.0f, -122.0f, -26.0f, 64.0f, -108.0f, -8.0f, 84.0f, -109.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[168] = new Triangle(-23.0f, 84.0f, -122.0f, -26.0f, 64.0f, -108.0f, -42.0f, 59.0f, -116.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[169] = new Triangle(-42.0f, 59.0f, -116.0f, -41.0f, 61.0f, -132.0f, -23.0f, 84.0f, -122.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[170] = new Triangle(-32.0f, 74.0f, -142.0f, -23.0f, 71.0f, -154.0f, -14.0f, 79.0f, -134.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[171] = new Triangle(-23.0f, 84.0f, -122.0f, -14.0f, 79.0f, -134.0f, -32.0f, 74.0f, -142.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[172] = new Triangle(-32.0f, 74.0f, -142.0f, -23.0f, 84.0f, -122.0f, -41.0f, 61.0f, -132.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[173] = new Triangle(-49.0f, 74.0f, -153.0f, -23.0f, 71.0f, -154.0f, -32.0f, 74.0f, -142.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[174] = new Triangle(-32.0f, 74.0f, -142.0f, -41.0f, 61.0f, -132.0f, -49.0f, 74.0f, -153.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[175] = new Triangle(-61.0f, 72.0f, -135.0f, -41.0f, 61.0f, -132.0f, -49.0f, 74.0f, -153.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[176] = new Triangle(-49.0f, 74.0f, -153.0f, -68.0f, 64.0f, -166.0f, -61.0f, 72.0f, -135.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[177] = new Triangle(-68.0f, 64.0f, -166.0f, -49.0f, 74.0f, -153.0f, -60.0f, 78.0f, -196.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[178] = new Triangle(-42.0f, 84.0f, -173.0f, -60.0f, 78.0f, -196.0f, -49.0f, 74.0f, -153.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[179] = new Triangle(-23.0f, 71.0f, -154.0f, -49.0f, 74.0f, -153.0f, -42.0f, 84.0f, -173.0f, 1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[180] = new Triangle(-2.0f, 74.0f, -39.0f, -5.0f, 82.0f, -24.0f, 19.0f, 89.0f, -50.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[181] = new Triangle(-9.0f, 0.0f, 0.0f, 36.0f, 0.0f, 0.0f, 38.0f, 0.0f, 27.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[182] = new Triangle(-15.0f, 0.0f, 27.0f, -9.0f, 0.0f, 0.0f, 38.0f, 0.0f, 27.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[183] = new Triangle(67.0f, 0.0f, 59.0f, 38.0f, 0.0f, 27.0f, -15.0f, 0.0f, 27.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[184] = new Triangle(-47.0f, 0.0f, 63.0f, -15.0f, 0.0f, 27.0f, 67.0f, 0.0f, 59.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[185] = new Triangle(13.0f, 0.0f, 76.0f, 67.0f, 0.0f, 59.0f, -47.0f, 0.0f, 63.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[186] = new Triangle(-23.0f, 0.0f, 97.0f, 13.0f, 0.0f, 76.0f, -47.0f, 0.0f, 63.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[187] = new Triangle(-66.0f, 0.0f, 118.0f, -23.0f, 0.0f, 97.0f, -47.0f, 0.0f, 63.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[188] = new Triangle(-19.0f, 0.0f, 123.0f, 13.0f, 0.0f, 76.0f, -66.0f, 0.0f, 118.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[189] = new Triangle(-17.0f, 11.0f, 151.0f, -19.0f, 0.0f, 123.0f, -66.0f, 0.0f, 118.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[190] = new Triangle(-59.0f, 0.0f, 152.0f, -17.0f, 11.0f, 151.0f, -66.0f, 0.0f, 118.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[191] = new Triangle(13.0f, 0.0f, 76.0f, 25.0f, 0.0f, 79.0f, 67.0f, 0.0f, 59.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[192] = new Triangle(25.0f, 0.0f, 79.0f, 88.0f, 0.0f, 92.0f, 67.0f, 0.0f, 59.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[193] = new Triangle(51.0f, 0.0f, 110.0f, 25.0f, 0.0f, 79.0f, 88.0f, 0.0f, 92.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[194] = new Triangle(55.0f, 0.0f, 132.0f, 51.0f, 0.0f, 110.0f, 88.0f, 0.0f, 92.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[195] = new Triangle(36.0f, 0.0f, 0.0f, 51.0f, 7.0f, 19.0f, 38.0f, 0.0f, 27.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[196] = new Triangle(51.0f, 7.0f, 19.0f, 36.0f, 0.0f, 0.0f, 77.0f, 25.0f, 5.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    gameMap1[197] = new Triangle(67.0f, 0.0f, 59.0f, 38.0f, 0.0f, 27.0f, 51.0f, 7.0f, 19.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[198] = new Triangle(90.0f, 7.0f, 40.0f, 51.0f, 7.0f, 19.0f, 67.0f, 0.0f, 59.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[199] = new Triangle(114.0f, 0.0f, 61.0f, 67.0f, 0.0f, 59.0f, 90.0f, 7.0f, 40.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[200] = new Triangle(139.0f, 7.0f, 48.0f, 90.0f, 7.0f, 40.0f, 114.0f, 0.0f, 61.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[201] = new Triangle(114.0f, 0.0f, 61.0f, 88.0f, 0.0f, 92.0f, 67.0f, 0.0f, 59.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[202] = new Triangle(13.0f, 0.0f, 76.0f, 18.0f, 15.0f, 101.0f, 25.0f, 0.0f, 79.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[203] = new Triangle(51.0f, 0.0f, 110.0f, 18.0f, 15.0f, 101.0f, 25.0f, 0.0f, 79.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[204] = new Triangle(13.0f, 0.0f, 76.0f, 18.0f, 15.0f, 101.0f, -19.0f, 0.0f, 123.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[205] = new Triangle(3.0f, 20.0f, 133.0f, -19.0f, 0.0f, 123.0f, -17.0f, 11.0f, 151.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[206] = new Triangle(3.0f, 20.0f, 133.0f, 18.0f, 15.0f, 101.0f, -19.0f, 0.0f, 123.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[207] = new Triangle(55.0f, 0.0f, 132.0f, 51.0f, 0.0f, 110.0f, 18.0f, 15.0f, 101.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[208] = new Triangle(30.0f, 15.0f, 134.0f, 55.0f, 0.0f, 132.0f, 18.0f, 15.0f, 101.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[209] = new Triangle(3.0f, 20.0f, 133.0f, 30.0f, 15.0f, 134.0f, 18.0f, 15.0f, 101.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[210] = new Triangle(84.0f, 33.0f, 24.0f, 77.0f, 25.0f, 5.0f, 51.0f, 7.0f, 19.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[211] = new Triangle(139.0f, 7.0f, 48.0f, 128.0f, 5.0f, 79.0f, 114.0f, 0.0f, 61.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[212] = new Triangle(128.0f, 5.0f, 79.0f, 88.0f, 0.0f, 92.0f, 114.0f, 0.0f, 61.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[213] = new Triangle(95.0f, 16.0f, 29.0f, 90.0f, 7.0f, 40.0f, 51.0f, 7.0f, 19.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[214] = new Triangle(95.0f, 16.0f, 29.0f, 139.0f, 7.0f, 48.0f, 90.0f, 7.0f, 40.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[215] = new Triangle(95.0f, 16.0f, 29.0f, 84.0f, 33.0f, 24.0f, 51.0f, 7.0f, 19.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[216] = new Triangle(128.0f, 17.0f, 27.0f, 95.0f, 16.0f, 29.0f, 139.0f, 7.0f, 48.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[217] = new Triangle(128.0f, 17.0f, 27.0f, 95.0f, 16.0f, 29.0f, 111.0f, 45.0f, 17.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[218] = new Triangle(111.0f, 45.0f, 17.0f, 95.0f, 16.0f, 29.0f, 84.0f, 33.0f, 24.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[219] = new Triangle(92.0f, 70.0f, 12.0f, 84.0f, 33.0f, 24.0f, 77.0f, 25.0f, 5.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[220] = new Triangle(77.0f, 25.0f, 5.0f, 88.0f, 65.0f, -2.0f, 92.0f, 70.0f, 12.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[221] = new Triangle(92.0f, 70.0f, 12.0f, 111.0f, 45.0f, 17.0f, 84.0f, 33.0f, 24.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[222] = new Triangle(-15.0f, 0.0f, 27.0f, -9.0f, 0.0f, 0.0f, -29.0f, 6.0f, 23.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[223] = new Triangle(-29.0f, 6.0f, 23.0f, -15.0f, 0.0f, 27.0f, -47.0f, 0.0f, 63.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[224] = new Triangle(-47.0f, 0.0f, 63.0f, -71.0f, 5.0f, 40.0f, -29.0f, 6.0f, 23.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[225] = new Triangle(-122.0f, 11.0f, 42.0f, -71.0f, 5.0f, 40.0f, -47.0f, 0.0f, 63.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[226] = new Triangle(-80.0f, 4.0f, 84.0f, -47.0f, 0.0f, 63.0f, -66.0f, 0.0f, 118.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[227] = new Triangle(-47.0f, 0.0f, 63.0f, -80.0f, 4.0f, 84.0f, -122.0f, 11.0f, 42.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[228] = new Triangle(-105.0f, 4.0f, 98.0f, -80.0f, 4.0f, 84.0f, -122.0f, 11.0f, 42.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[229] = new Triangle(-105.0f, 4.0f, 98.0f, -80.0f, 4.0f, 84.0f, -66.0f, 0.0f, 118.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[230] = new Triangle(-96.0f, 0.0f, 140.0f, -59.0f, 0.0f, 152.0f, -66.0f, 0.0f, 118.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[231] = new Triangle(-66.0f, 0.0f, 118.0f, -105.0f, 4.0f, 98.0f, -96.0f, 0.0f, 140.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[232] = new Triangle(-42.0f, 25.0f, 12.0f, -43.0f, 14.0f, -1.0f, -9.0f, 0.0f, 0.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[233] = new Triangle(-29.0f, 6.0f, 23.0f, -9.0f, 0.0f, 0.0f, -42.0f, 25.0f, 12.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[234] = new Triangle(-29.0f, 6.0f, 23.0f, -62.0f, 6.0f, 30.0f, -51.0f, 40.0f, 17.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[235] = new Triangle(-51.0f, 40.0f, 17.0f, -42.0f, 25.0f, 12.0f, -29.0f, 6.0f, 23.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[236] = new Triangle(-29.0f, 6.0f, 23.0f, -62.0f, 6.0f, 30.0f, -71.0f, 5.0f, 40.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[237] = new Triangle(-71.0f, 24.0f, 26.0f, -62.0f, 6.0f, 30.0f, -71.0f, 5.0f, 40.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[238] = new Triangle(-62.0f, 6.0f, 30.0f, -71.0f, 24.0f, 26.0f, -51.0f, 40.0f, 17.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[239] = new Triangle(-41.0f, 59.0f, 8.0f, -42.0f, 25.0f, 12.0f, -43.0f, 14.0f, -1.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[240] = new Triangle(-43.0f, 14.0f, -1.0f, -41.0f, 59.0f, 8.0f, -36.0f, 70.0f, -11.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[241] = new Triangle(-41.0f, 59.0f, 8.0f, -42.0f, 25.0f, 12.0f, -51.0f, 40.0f, 17.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[242] = new Triangle(-96.0f, 15.0f, 31.0f, -71.0f, 5.0f, 40.0f, -122.0f, 11.0f, 42.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[243] = new Triangle(-96.0f, 15.0f, 31.0f, -71.0f, 5.0f, 40.0f, -71.0f, 24.0f, 26.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[244] = new Triangle(-35.0f, 90.0f, 10.0f, -36.0f, 70.0f, -11.0f, -16.0f, 82.0f, -8.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[245] = new Triangle(-35.0f, 90.0f, 10.0f, -41.0f, 59.0f, 8.0f, -36.0f, 70.0f, -11.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[246] = new Triangle(2.0f, 105.0f, -1.0f, -16.0f, 82.0f, -8.0f, -35.0f, 90.0f, 10.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[247] = new Triangle(16.0f, 92.0f, -10.0f, -16.0f, 82.0f, -8.0f, 2.0f, 105.0f, -1.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[248] = new Triangle(43.0f, 97.0f, -5.0f, 22.0f, 106.0f, -4.0f, 16.0f, 92.0f, -10.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[249] = new Triangle(16.0f, 92.0f, -10.0f, 22.0f, 106.0f, -4.0f, 2.0f, 105.0f, -1.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[250] = new Triangle(43.0f, 97.0f, -5.0f, 57.0f, 100.0f, -1.0f, 72.0f, 90.0f, -5.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[251] = new Triangle(43.0f, 97.0f, -5.0f, 57.0f, 100.0f, -1.0f, 22.0f, 106.0f, -4.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[252] = new Triangle(57.0f, 100.0f, -1.0f, 72.0f, 90.0f, -5.0f, 76.0f, 90.0f, 2.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[253] = new Triangle(92.0f, 70.0f, 12.0f, 88.0f, 65.0f, -2.0f, 76.0f, 90.0f, 2.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[254] = new Triangle(76.0f, 90.0f, 2.0f, 72.0f, 90.0f, -5.0f, 88.0f, 65.0f, -2.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[255] = new Triangle(98.0f, 97.0f, 8.0f, 92.0f, 70.0f, 12.0f, 76.0f, 90.0f, 2.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[256] = new Triangle(98.0f, 97.0f, 8.0f, 92.0f, 70.0f, 12.0f, 101.0f, 69.0f, 12.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[257] = new Triangle(101.0f, 69.0f, 12.0f, 111.0f, 45.0f, 17.0f, 92.0f, 70.0f, 12.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[258] = new Triangle(76.0f, 109.0f, -6.0f, 72.0f, 90.0f, -5.0f, 76.0f, 90.0f, 2.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[259] = new Triangle(76.0f, 90.0f, 2.0f, 76.0f, 109.0f, -6.0f, 98.0f, 97.0f, 8.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[260] = new Triangle(72.0f, 90.0f, -5.0f, 76.0f, 109.0f, -6.0f, 57.0f, 100.0f, -1.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[261] = new Triangle(53.0f, 115.0f, -5.0f, 76.0f, 109.0f, -6.0f, 57.0f, 100.0f, -1.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[262] = new Triangle(57.0f, 100.0f, -1.0f, 22.0f, 106.0f, -4.0f, 53.0f, 115.0f, -5.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[263] = new Triangle(19.0f, 123.0f, -10.0f, 22.0f, 106.0f, -4.0f, 2.0f, 105.0f, -1.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[264] = new Triangle(22.0f, 106.0f, -4.0f, 19.0f, 123.0f, -10.0f, 53.0f, 115.0f, -5.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[265] = new Triangle(2.0f, 105.0f, -1.0f, -21.0f, 112.0f, -9.0f, -35.0f, 90.0f, 10.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[266] = new Triangle(2.0f, 105.0f, -1.0f, -3.0f, 123.0f, -14.0f, -21.0f, 112.0f, -9.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[267] = new Triangle(2.0f, 105.0f, -1.0f, -3.0f, 123.0f, -14.0f, 19.0f, 123.0f, -10.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[268] = new Triangle(41.0f, 132.0f, -13.0f, 19.0f, 123.0f, -10.0f, 53.0f, 115.0f, -5.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[269] = new Triangle(64.0f, 123.0f, -11.0f, 41.0f, 132.0f, -13.0f, 53.0f, 115.0f, -5.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[270] = new Triangle(64.0f, 123.0f, -11.0f, 53.0f, 115.0f, -5.0f, 76.0f, 109.0f, -6.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[271] = new Triangle(-96.0f, 15.0f, 31.0f, -104.0f, 20.0f, 12.0f, -71.0f, 24.0f, 26.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[272] = new Triangle(-71.0f, 24.0f, 26.0f, -104.0f, 20.0f, 12.0f, -69.0f, 45.0f, 11.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[273] = new Triangle(-69.0f, 45.0f, 11.0f, -71.0f, 24.0f, 26.0f, -51.0f, 40.0f, 17.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[274] = new Triangle(-54.0f, 68.0f, 13.0f, -51.0f, 40.0f, 17.0f, -69.0f, 45.0f, 11.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[275] = new Triangle(-51.0f, 40.0f, 17.0f, -54.0f, 68.0f, 13.0f, -41.0f, 59.0f, 8.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[276] = new Triangle(-41.0f, 59.0f, 8.0f, -54.0f, 68.0f, 13.0f, -35.0f, 90.0f, 10.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[277] = new Triangle(-72.0f, 72.0f, 0.0f, -69.0f, 45.0f, 11.0f, -54.0f, 68.0f, 13.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[278] = new Triangle(-54.0f, 68.0f, 13.0f, -72.0f, 72.0f, 0.0f, -35.0f, 90.0f, 10.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[279] = new Triangle(-86.0f, 45.0f, -6.0f, -69.0f, 45.0f, 11.0f, -104.0f, 20.0f, 12.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[280] = new Triangle(-72.0f, 72.0f, 0.0f, -69.0f, 45.0f, 11.0f, -86.0f, 45.0f, -6.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[281] = new Triangle(-72.0f, 72.0f, 0.0f, -47.0f, 97.0f, -4.0f, -35.0f, 90.0f, 10.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[282] = new Triangle(-35.0f, 90.0f, 10.0f, -47.0f, 97.0f, -4.0f, -21.0f, 112.0f, -9.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[283] = new Triangle(-59.0f, 0.0f, 152.0f, -17.0f, 11.0f, 151.0f, -20.0f, 18.0f, 164.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[284] = new Triangle(-62.0f, 18.0f, 161.0f, -59.0f, 0.0f, 152.0f, -20.0f, 18.0f, 164.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[285] = new Triangle(-28.0f, 23.0f, 181.0f, -62.0f, 18.0f, 161.0f, -20.0f, 18.0f, 164.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[286] = new Triangle(-28.0f, 23.0f, 181.0f, -62.0f, 18.0f, 161.0f, -76.0f, 25.0f, 170.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[287] = new Triangle(-62.0f, 18.0f, 161.0f, -96.0f, 0.0f, 140.0f, -59.0f, 0.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[288] = new Triangle(-28.0f, 23.0f, 181.0f, -76.0f, 25.0f, 170.0f, -47.0f, 30.0f, 209.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[289] = new Triangle(-76.0f, 25.0f, 170.0f, -90.0f, 33.0f, 177.0f, -47.0f, 30.0f, 209.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[290] = new Triangle(-72.0f, 38.0f, 219.0f, -90.0f, 33.0f, 177.0f, -47.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[291] = new Triangle(-72.0f, 38.0f, 219.0f, -90.0f, 33.0f, 177.0f, -91.0f, 33.0f, 224.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[292] = new Triangle(-91.0f, 33.0f, 224.0f, -90.0f, 33.0f, 177.0f, -103.0f, 33.0f, 181.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[293] = new Triangle(-103.0f, 33.0f, 181.0f, -108.0f, 37.0f, 221.0f, -91.0f, 33.0f, 224.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[294] = new Triangle(-121.0f, 37.0f, 179.0f, -103.0f, 33.0f, 181.0f, -108.0f, 37.0f, 221.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[295] = new Triangle(-126.0f, 42.0f, 219.0f, -108.0f, 37.0f, 221.0f, -121.0f, 37.0f, 179.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[296] = new Triangle(-121.0f, 37.0f, 179.0f, -126.0f, 42.0f, 219.0f, -144.0f, 47.0f, 204.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[297] = new Triangle(-144.0f, 47.0f, 204.0f, -121.0f, 37.0f, 179.0f, -167.0f, 47.0f, 189.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[298] = new Triangle(-105.0f, 4.0f, 98.0f, -125.0f, 14.0f, 98.0f, -122.0f, 11.0f, 42.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[299] = new Triangle(-125.0f, 14.0f, 98.0f, -105.0f, 4.0f, 98.0f, -96.0f, 0.0f, 140.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[300] = new Triangle(-96.0f, 0.0f, 140.0f, -78.0f, 29.0f, 162.0f, -62.0f, 18.0f, 161.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[301] = new Triangle(-62.0f, 18.0f, 161.0f, -78.0f, 29.0f, 162.0f, -76.0f, 25.0f, 170.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[302] = new Triangle(-76.0f, 25.0f, 170.0f, -78.0f, 29.0f, 162.0f, -90.0f, 33.0f, 177.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[303] = new Triangle(-122.0f, 11.0f, 42.0f, -131.0f, 21.0f, 69.0f, -125.0f, 14.0f, 98.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[304] = new Triangle(-122.0f, 11.0f, 42.0f, -96.0f, 15.0f, 31.0f, -104.0f, 20.0f, 12.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[305] = new Triangle(-132.0f, 34.0f, 49.0f, -122.0f, 11.0f, 42.0f, -131.0f, 21.0f, 69.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[306] = new Triangle(-122.0f, 11.0f, 42.0f, -122.0f, 31.0f, 35.0f, -132.0f, 34.0f, 49.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[307] = new Triangle(-122.0f, 31.0f, 35.0f, -122.0f, 11.0f, 42.0f, -104.0f, 20.0f, 12.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[308] = new Triangle(-111.0f, 43.0f, 14.0f, -122.0f, 31.0f, 35.0f, -104.0f, 20.0f, 12.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[309] = new Triangle(-104.0f, 20.0f, 12.0f, -111.0f, 43.0f, 14.0f, -86.0f, 45.0f, -6.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[310] = new Triangle(-134.0f, 32.0f, 88.0f, -125.0f, 14.0f, 98.0f, -131.0f, 21.0f, 69.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[311] = new Triangle(-125.0f, 14.0f, 98.0f, -96.0f, 0.0f, 140.0f, -120.0f, 28.0f, 123.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[312] = new Triangle(-93.0f, 40.0f, 148.0f, -96.0f, 0.0f, 140.0f, -78.0f, 29.0f, 162.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[313] = new Triangle(-100.0f, 49.0f, 140.0f, -93.0f, 40.0f, 148.0f, -96.0f, 0.0f, 140.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[314] = new Triangle(-100.0f, 49.0f, 140.0f, -120.0f, 28.0f, 123.0f, -96.0f, 0.0f, 140.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[315] = new Triangle(-100.0f, 49.0f, 140.0f, -118.0f, 56.0f, 114.0f, -120.0f, 28.0f, 123.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[316] = new Triangle(-125.0f, 37.0f, 103.0f, -134.0f, 32.0f, 88.0f, -125.0f, 14.0f, 98.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[317] = new Triangle(-125.0f, 14.0f, 98.0f, -125.0f, 37.0f, 103.0f, -120.0f, 28.0f, 123.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[318] = new Triangle(-118.0f, 56.0f, 114.0f, -120.0f, 28.0f, 123.0f, -125.0f, 37.0f, 103.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[319] = new Triangle(-17.0f, 11.0f, 151.0f, 3.0f, 20.0f, 133.0f, 1.0f, 23.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[320] = new Triangle(-20.0f, 18.0f, 164.0f, 1.0f, 23.0f, 152.0f, -17.0f, 11.0f, 151.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[321] = new Triangle(30.0f, 15.0f, 134.0f, 39.0f, 15.0f, 152.0f, 55.0f, 0.0f, 132.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[322] = new Triangle(18.0f, 21.0f, 150.0f, 1.0f, 23.0f, 152.0f, 3.0f, 20.0f, 133.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[323] = new Triangle(18.0f, 21.0f, 150.0f, 3.0f, 20.0f, 133.0f, 30.0f, 15.0f, 134.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[324] = new Triangle(18.0f, 21.0f, 150.0f, 39.0f, 15.0f, 152.0f, 30.0f, 15.0f, 134.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[325] = new Triangle(1.0f, 23.0f, 152.0f, -5.0f, 23.0f, 176.0f, -20.0f, 18.0f, 164.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[326] = new Triangle(-28.0f, 23.0f, 181.0f, -5.0f, 23.0f, 176.0f, -15.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[327] = new Triangle(-5.0f, 23.0f, 176.0f, -28.0f, 23.0f, 181.0f, -20.0f, 18.0f, 164.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[328] = new Triangle(-28.0f, 23.0f, 181.0f, -15.0f, 30.0f, 209.0f, -47.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[329] = new Triangle(-72.0f, 38.0f, 219.0f, -41.0f, 38.0f, 229.0f, -47.0f, 30.0f, 209.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[330] = new Triangle(-15.0f, 30.0f, 209.0f, -41.0f, 38.0f, 229.0f, -47.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[331] = new Triangle(-41.0f, 38.0f, 229.0f, -72.0f, 38.0f, 219.0f, -80.0f, 33.0f, 245.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[332] = new Triangle(-91.0f, 33.0f, 224.0f, -80.0f, 33.0f, 245.0f, -72.0f, 38.0f, 219.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[333] = new Triangle(15.0f, 23.0f, 163.0f, -5.0f, 23.0f, 176.0f, 1.0f, 23.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[334] = new Triangle(18.0f, 21.0f, 150.0f, 15.0f, 23.0f, 163.0f, 1.0f, 23.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[335] = new Triangle(-5.0f, 23.0f, 176.0f, 9.0f, 26.0f, 188.0f, -15.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[336] = new Triangle(9.0f, 26.0f, 188.0f, 15.0f, 23.0f, 163.0f, -5.0f, 23.0f, 176.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[337] = new Triangle(32.0f, 23.0f, 173.0f, 18.0f, 21.0f, 150.0f, 39.0f, 15.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[338] = new Triangle(32.0f, 23.0f, 173.0f, 9.0f, 26.0f, 188.0f, 18.0f, 21.0f, 150.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[339] = new Triangle(55.0f, 0.0f, 132.0f, 97.0f, 0.0f, 123.0f, 88.0f, 0.0f, 92.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[340] = new Triangle(55.0f, 0.0f, 132.0f, 67.0f, 0.0f, 151.0f, 97.0f, 0.0f, 123.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[341] = new Triangle(97.0f, 0.0f, 123.0f, 114.0f, 0.0f, 141.0f, 67.0f, 0.0f, 151.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[342] = new Triangle(114.0f, 0.0f, 141.0f, 67.0f, 0.0f, 151.0f, 84.0f, 0.0f, 168.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[343] = new Triangle(84.0f, 0.0f, 168.0f, 85.0f, 0.0f, 190.0f, 154.0f, 0.0f, 187.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[344] = new Triangle(114.0f, 0.0f, 141.0f, 84.0f, 0.0f, 168.0f, 154.0f, 0.0f, 187.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[345] = new Triangle(85.0f, 0.0f, 190.0f, 84.0f, 0.0f, 228.0f, 154.0f, 0.0f, 187.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[346] = new Triangle(154.0f, 0.0f, 187.0f, 114.0f, 0.0f, 141.0f, 174.0f, 0.0f, 132.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[347] = new Triangle(174.0f, 0.0f, 132.0f, 192.0f, 0.0f, 206.0f, 154.0f, 0.0f, 187.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[348] = new Triangle(154.0f, 0.0f, 187.0f, 192.0f, 0.0f, 206.0f, 84.0f, 0.0f, 228.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[349] = new Triangle(85.0f, 0.0f, 190.0f, 46.0f, 0.0f, 273.0f, 84.0f, 0.0f, 228.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[350] = new Triangle(46.0f, 0.0f, 273.0f, 55.0f, 0.0f, 305.0f, 141.0f, 0.0f, 271.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[351] = new Triangle(141.0f, 0.0f, 271.0f, 84.0f, 0.0f, 228.0f, 46.0f, 0.0f, 273.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[352] = new Triangle(141.0f, 0.0f, 271.0f, 192.0f, 0.0f, 206.0f, 84.0f, 0.0f, 228.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[353] = new Triangle(55.0f, 0.0f, 305.0f, 90.0f, 0.0f, 374.0f, 141.0f, 0.0f, 271.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[354] = new Triangle(141.0f, 0.0f, 271.0f, 90.0f, 0.0f, 374.0f, 179.0f, 0.0f, 406.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[355] = new Triangle(192.0f, 0.0f, 206.0f, 237.0f, 0.0f, 397.0f, 179.0f, 0.0f, 406.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[356] = new Triangle(179.0f, 0.0f, 406.0f, 141.0f, 0.0f, 271.0f, 192.0f, 0.0f, 206.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[357] = new Triangle(97.0f, 0.0f, 123.0f, 118.0f, 3.0f, 108.0f, 88.0f, 0.0f, 92.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[358] = new Triangle(118.0f, 3.0f, 108.0f, 128.0f, 5.0f, 79.0f, 88.0f, 0.0f, 92.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[359] = new Triangle(118.0f, 3.0f, 108.0f, 97.0f, 0.0f, 123.0f, 114.0f, 0.0f, 141.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[360] = new Triangle(145.0f, 6.0f, 114.0f, 114.0f, 0.0f, 141.0f, 174.0f, 0.0f, 132.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[361] = new Triangle(114.0f, 0.0f, 141.0f, 145.0f, 6.0f, 114.0f, 118.0f, 3.0f, 108.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[362] = new Triangle(118.0f, 3.0f, 108.0f, 145.0f, 6.0f, 114.0f, 143.0f, 8.0f, 92.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[363] = new Triangle(143.0f, 8.0f, 92.0f, 128.0f, 5.0f, 79.0f, 118.0f, 3.0f, 108.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[364] = new Triangle(143.0f, 8.0f, 92.0f, 128.0f, 5.0f, 79.0f, 139.0f, 7.0f, 48.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[365] = new Triangle(55.0f, 0.0f, 132.0f, 67.0f, 0.0f, 151.0f, 39.0f, 15.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[366] = new Triangle(71.0f, 9.0f, 182.0f, 85.0f, 0.0f, 190.0f, 84.0f, 0.0f, 168.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[367] = new Triangle(84.0f, 0.0f, 168.0f, 71.0f, 9.0f, 182.0f, 67.0f, 0.0f, 151.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[368] = new Triangle(53.0f, 21.0f, 168.0f, 32.0f, 23.0f, 173.0f, 39.0f, 15.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[369] = new Triangle(39.0f, 15.0f, 152.0f, 53.0f, 21.0f, 168.0f, 67.0f, 0.0f, 151.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[370] = new Triangle(67.0f, 0.0f, 151.0f, 71.0f, 9.0f, 182.0f, 53.0f, 21.0f, 168.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[371] = new Triangle(85.0f, 0.0f, 190.0f, 46.0f, 0.0f, 273.0f, 62.0f, 0.0f, 223.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[372] = new Triangle(62.0f, 0.0f, 223.0f, 85.0f, 0.0f, 190.0f, 71.0f, 9.0f, 182.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[373] = new Triangle(58.0f, 21.0f, 194.0f, 71.0f, 9.0f, 182.0f, 53.0f, 21.0f, 168.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[374] = new Triangle(58.0f, 21.0f, 194.0f, 71.0f, 9.0f, 182.0f, 62.0f, 0.0f, 223.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[375] = new Triangle(58.0f, 21.0f, 194.0f, 53.0f, 21.0f, 168.0f, 32.0f, 23.0f, 173.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[376] = new Triangle(45.0f, 17.0f, 226.0f, 62.0f, 0.0f, 223.0f, 46.0f, 0.0f, 273.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[377] = new Triangle(45.0f, 17.0f, 226.0f, 62.0f, 0.0f, 223.0f, 58.0f, 21.0f, 194.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[378] = new Triangle(58.0f, 21.0f, 194.0f, 46.0f, 26.0f, 197.0f, 45.0f, 17.0f, 226.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[379] = new Triangle(46.0f, 26.0f, 197.0f, 58.0f, 21.0f, 194.0f, 32.0f, 23.0f, 173.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[380] = new Triangle(32.0f, 29.0f, 192.0f, 46.0f, 26.0f, 197.0f, 32.0f, 23.0f, 173.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[381] = new Triangle(32.0f, 29.0f, 192.0f, 9.0f, 26.0f, 188.0f, 32.0f, 23.0f, 173.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[382] = new Triangle(18.0f, 21.0f, 150.0f, 15.0f, 23.0f, 163.0f, 9.0f, 26.0f, 188.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[383] = new Triangle(29.0f, 34.0f, 219.0f, 32.0f, 29.0f, 192.0f, 46.0f, 26.0f, 197.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[384] = new Triangle(45.0f, 17.0f, 226.0f, 29.0f, 34.0f, 219.0f, 46.0f, 26.0f, 197.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[385] = new Triangle(32.0f, 29.0f, 192.0f, 29.0f, 34.0f, 219.0f, 9.0f, 26.0f, 188.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[386] = new Triangle(9.0f, 26.0f, 188.0f, 29.0f, 34.0f, 219.0f, -15.0f, 30.0f, 209.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[387] = new Triangle(-15.0f, 35.0f, 223.0f, -15.0f, 30.0f, 209.0f, -41.0f, 38.0f, 229.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[388] = new Triangle(-15.0f, 35.0f, 223.0f, -15.0f, 30.0f, 209.0f, 29.0f, 34.0f, 219.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[389] = new Triangle(35.0f, 26.0f, 247.0f, 29.0f, 34.0f, 219.0f, 45.0f, 17.0f, 226.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[390] = new Triangle(45.0f, 17.0f, 226.0f, 35.0f, 26.0f, 247.0f, 46.0f, 0.0f, 273.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[391] = new Triangle(46.0f, 0.0f, 273.0f, 55.0f, 0.0f, 305.0f, 28.0f, 24.0f, 288.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[392] = new Triangle(28.0f, 24.0f, 288.0f, 35.0f, 26.0f, 247.0f, 46.0f, 0.0f, 273.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[393] = new Triangle(55.0f, 0.0f, 305.0f, 66.0f, 0.0f, 339.0f, 90.0f, 0.0f, 374.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[394] = new Triangle(66.0f, 0.0f, 339.0f, 55.0f, 0.0f, 305.0f, 40.0f, 20.0f, 323.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[395] = new Triangle(28.0f, 24.0f, 288.0f, 40.0f, 20.0f, 323.0f, 55.0f, 0.0f, 305.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[396] = new Triangle(66.0f, 0.0f, 339.0f, 90.0f, 0.0f, 374.0f, 61.0f, 21.0f, 363.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[397] = new Triangle(40.0f, 20.0f, 323.0f, 61.0f, 21.0f, 363.0f, 66.0f, 0.0f, 339.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[398] = new Triangle(90.0f, 0.0f, 374.0f, 132.0f, 0.0f, 395.0f, 179.0f, 0.0f, 406.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[399] = new Triangle(132.0f, 0.0f, 395.0f, 90.0f, 0.0f, 374.0f, 98.0f, 9.0f, 391.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[400] = new Triangle(132.0f, 0.0f, 395.0f, 179.0f, 0.0f, 406.0f, 152.0f, 8.0f, 409.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[401] = new Triangle(123.0f, 11.0f, 407.0f, 98.0f, 9.0f, 391.0f, 132.0f, 0.0f, 395.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[402] = new Triangle(123.0f, 11.0f, 407.0f, 152.0f, 8.0f, 409.0f, 132.0f, 0.0f, 395.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[403] = new Triangle(79.0f, 15.0f, 382.0f, 98.0f, 9.0f, 391.0f, 90.0f, 0.0f, 374.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[404] = new Triangle(61.0f, 21.0f, 363.0f, 90.0f, 0.0f, 374.0f, 79.0f, 15.0f, 382.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[405] = new Triangle(179.0f, 0.0f, 406.0f, 210.0f, 9.0f, 413.0f, 237.0f, 0.0f, 397.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[406] = new Triangle(210.0f, 9.0f, 413.0f, 176.0f, 12.0f, 418.0f, 179.0f, 0.0f, 406.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[407] = new Triangle(176.0f, 12.0f, 418.0f, 152.0f, 8.0f, 409.0f, 179.0f, 0.0f, 406.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[408] = new Triangle(15.0f, 38.0f, 237.0f, 29.0f, 34.0f, 219.0f, 35.0f, 26.0f, 247.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[409] = new Triangle(29.0f, 34.0f, 219.0f, 15.0f, 38.0f, 237.0f, -15.0f, 35.0f, 223.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[410] = new Triangle(15.0f, 38.0f, 237.0f, 35.0f, 26.0f, 247.0f, 28.0f, 24.0f, 288.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[411] = new Triangle(-91.0f, 33.0f, 224.0f, -80.0f, 33.0f, 245.0f, -105.0f, 42.0f, 237.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[412] = new Triangle(-105.0f, 42.0f, 237.0f, -108.0f, 37.0f, 221.0f, -91.0f, 33.0f, 224.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[413] = new Triangle(-105.0f, 42.0f, 237.0f, -126.0f, 42.0f, 219.0f, -108.0f, 37.0f, 221.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[414] = new Triangle(-16.0f, 38.0f, 243.0f, -41.0f, 38.0f, 229.0f, -15.0f, 35.0f, 223.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[415] = new Triangle(-15.0f, 35.0f, 223.0f, -16.0f, 38.0f, 243.0f, 15.0f, 38.0f, 237.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[416] = new Triangle(15.0f, 38.0f, 237.0f, 28.0f, 24.0f, 288.0f, -16.0f, 38.0f, 243.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[417] = new Triangle(-41.0f, 38.0f, 229.0f, -16.0f, 38.0f, 243.0f, -45.0f, 33.0f, 256.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[418] = new Triangle(-45.0f, 33.0f, 256.0f, -41.0f, 38.0f, 229.0f, -80.0f, 33.0f, 245.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[419] = new Triangle(-3.0f, 25.0f, 276.0f, -16.0f, 38.0f, 243.0f, -45.0f, 33.0f, 256.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[420] = new Triangle(-3.0f, 25.0f, 276.0f, -16.0f, 38.0f, 243.0f, 28.0f, 24.0f, 288.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[421] = new Triangle(15.0f, 33.0f, 304.0f, 28.0f, 24.0f, 288.0f, 40.0f, 20.0f, 323.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[422] = new Triangle(40.0f, 20.0f, 323.0f, 61.0f, 21.0f, 363.0f, 23.0f, 33.0f, 350.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[423] = new Triangle(28.0f, 24.0f, 288.0f, 15.0f, 33.0f, 304.0f, -3.0f, 25.0f, 276.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[424] = new Triangle(15.0f, 33.0f, 304.0f, 40.0f, 20.0f, 323.0f, 23.0f, 33.0f, 350.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[425] = new Triangle(68.0f, 20.0f, 403.0f, 61.0f, 21.0f, 363.0f, 79.0f, 15.0f, 382.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[426] = new Triangle(68.0f, 20.0f, 403.0f, 61.0f, 21.0f, 363.0f, 23.0f, 33.0f, 350.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[427] = new Triangle(79.0f, 15.0f, 382.0f, 68.0f, 20.0f, 403.0f, 98.0f, 9.0f, 391.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[428] = new Triangle(105.0f, 13.0f, 419.0f, 123.0f, 11.0f, 407.0f, 98.0f, 9.0f, 391.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[429] = new Triangle(98.0f, 9.0f, 391.0f, 105.0f, 13.0f, 419.0f, 68.0f, 20.0f, 403.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[430] = new Triangle(144.0f, 13.0f, 432.0f, 105.0f, 13.0f, 419.0f, 123.0f, 11.0f, 407.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[431] = new Triangle(152.0f, 8.0f, 409.0f, 144.0f, 13.0f, 432.0f, 123.0f, 11.0f, 407.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[432] = new Triangle(144.0f, 13.0f, 432.0f, 152.0f, 8.0f, 409.0f, 176.0f, 12.0f, 418.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[433] = new Triangle(201.0f, 13.0f, 442.0f, 176.0f, 12.0f, 418.0f, 210.0f, 9.0f, 413.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[434] = new Triangle(175.0f, 13.0f, 431.0f, 176.0f, 12.0f, 418.0f, 201.0f, 13.0f, 442.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[435] = new Triangle(175.0f, 13.0f, 431.0f, 176.0f, 12.0f, 418.0f, 144.0f, 13.0f, 432.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[436] = new Triangle(171.0f, 22.0f, 457.0f, 175.0f, 13.0f, 431.0f, 201.0f, 13.0f, 442.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[437] = new Triangle(171.0f, 22.0f, 457.0f, 175.0f, 13.0f, 431.0f, 144.0f, 13.0f, 432.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[438] = new Triangle(120.0f, 20.0f, 451.0f, 171.0f, 22.0f, 457.0f, 144.0f, 13.0f, 432.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[439] = new Triangle(144.0f, 13.0f, 432.0f, 120.0f, 20.0f, 451.0f, 105.0f, 13.0f, 419.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[440] = new Triangle(105.0f, 13.0f, 419.0f, 120.0f, 20.0f, 451.0f, 88.0f, 23.0f, 437.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[441] = new Triangle(88.0f, 23.0f, 437.0f, 105.0f, 13.0f, 419.0f, 68.0f, 20.0f, 403.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[442] = new Triangle(49.0f, 33.0f, 404.0f, 68.0f, 20.0f, 403.0f, 88.0f, 23.0f, 437.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[443] = new Triangle(68.0f, 20.0f, 403.0f, 49.0f, 33.0f, 404.0f, 23.0f, 33.0f, 350.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[444] = new Triangle(-16.0f, 42.0f, 302.0f, -3.0f, 25.0f, 276.0f, 15.0f, 33.0f, 304.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[445] = new Triangle(15.0f, 33.0f, 304.0f, -16.0f, 42.0f, 302.0f, 23.0f, 33.0f, 350.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[446] = new Triangle(-16.0f, 42.0f, 302.0f, -3.0f, 25.0f, 276.0f, -45.0f, 33.0f, 256.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[447] = new Triangle(-59.0f, 50.0f, 276.0f, -45.0f, 33.0f, 256.0f, -16.0f, 42.0f, 302.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[448] = new Triangle(-59.0f, 50.0f, 276.0f, -45.0f, 33.0f, 256.0f, -80.0f, 33.0f, 245.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[449] = new Triangle(-126.0f, 42.0f, 219.0f, -151.0f, 42.0f, 208.0f, -144.0f, 47.0f, 204.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[450] = new Triangle(-144.0f, 47.0f, 204.0f, -151.0f, 42.0f, 208.0f, -167.0f, 47.0f, 189.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[451] = new Triangle(-129.0f, 47.0f, 174.0f, -121.0f, 37.0f, 179.0f, -167.0f, 47.0f, 189.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[452] = new Triangle(-90.0f, 33.0f, 177.0f, -94.0f, 33.0f, 170.0f, -78.0f, 29.0f, 162.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[453] = new Triangle(-90.0f, 33.0f, 177.0f, -103.0f, 33.0f, 181.0f, -94.0f, 33.0f, 170.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[454] = new Triangle(-109.0f, 37.0f, 175.0f, -103.0f, 33.0f, 181.0f, -94.0f, 33.0f, 170.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[455] = new Triangle(-103.0f, 33.0f, 181.0f, -109.0f, 37.0f, 175.0f, -121.0f, 37.0f, 179.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[456] = new Triangle(-96.0f, 49.0f, 159.0f, -100.0f, 49.0f, 140.0f, -93.0f, 40.0f, 148.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[457] = new Triangle(-78.0f, 29.0f, 162.0f, -96.0f, 49.0f, 159.0f, -93.0f, 40.0f, 148.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[458] = new Triangle(-121.0f, 37.0f, 179.0f, -109.0f, 37.0f, 175.0f, -118.0f, 47.0f, 174.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[459] = new Triangle(-118.0f, 47.0f, 174.0f, -129.0f, 47.0f, 174.0f, -121.0f, 37.0f, 179.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[460] = new Triangle(-94.0f, 33.0f, 170.0f, -78.0f, 29.0f, 162.0f, -96.0f, 49.0f, 159.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[461] = new Triangle(-101.0f, 49.0f, 166.0f, -96.0f, 49.0f, 159.0f, -94.0f, 33.0f, 170.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[462] = new Triangle(-94.0f, 33.0f, 170.0f, -109.0f, 37.0f, 175.0f, -101.0f, 49.0f, 166.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[463] = new Triangle(-118.0f, 47.0f, 174.0f, -109.0f, 37.0f, 175.0f, -101.0f, 49.0f, 166.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[464] = new Triangle(-97.0f, 64.0f, 159.0f, -96.0f, 49.0f, 159.0f, -100.0f, 49.0f, 140.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[465] = new Triangle(-101.0f, 49.0f, 166.0f, -96.0f, 49.0f, 159.0f, -97.0f, 64.0f, 159.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[466] = new Triangle(-141.0f, 41.0f, 69.0f, -131.0f, 21.0f, 69.0f, -132.0f, 34.0f, 49.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[467] = new Triangle(-134.0f, 32.0f, 88.0f, -141.0f, 41.0f, 69.0f, -131.0f, 21.0f, 69.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[468] = new Triangle(-129.0f, 55.0f, 29.0f, -122.0f, 31.0f, 35.0f, -132.0f, 34.0f, 49.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[469] = new Triangle(-129.0f, 55.0f, 29.0f, -122.0f, 31.0f, 35.0f, -111.0f, 43.0f, 14.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[470] = new Triangle(-90.0f, 69.0f, 7.0f, -86.0f, 45.0f, -6.0f, -111.0f, 43.0f, 14.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[471] = new Triangle(-72.0f, 72.0f, 0.0f, -90.0f, 69.0f, 7.0f, -86.0f, 45.0f, -6.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[472] = new Triangle(-77.0f, 91.0f, -1.0f, -72.0f, 72.0f, 0.0f, -90.0f, 69.0f, 7.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[473] = new Triangle(-77.0f, 91.0f, -1.0f, -72.0f, 72.0f, 0.0f, -47.0f, 97.0f, -4.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[474] = new Triangle(-108.0f, 73.0f, 24.0f, -111.0f, 43.0f, 14.0f, -129.0f, 55.0f, 29.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[475] = new Triangle(-108.0f, 73.0f, 24.0f, -111.0f, 43.0f, 14.0f, -90.0f, 69.0f, 7.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[476] = new Triangle(-104.0f, 68.0f, 143.0f, -100.0f, 49.0f, 140.0f, -97.0f, 64.0f, 159.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[477] = new Triangle(-104.0f, 68.0f, 143.0f, -100.0f, 49.0f, 140.0f, -118.0f, 56.0f, 114.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[478] = new Triangle(-118.0f, 56.0f, 114.0f, -125.0f, 37.0f, 103.0f, -134.0f, 32.0f, 88.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[479] = new Triangle(-134.0f, 53.0f, 88.0f, -134.0f, 32.0f, 88.0f, -141.0f, 41.0f, 69.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[480] = new Triangle(-134.0f, 32.0f, 88.0f, -134.0f, 53.0f, 88.0f, -118.0f, 56.0f, 114.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[481] = new Triangle(-134.0f, 65.0f, 49.0f, -132.0f, 34.0f, 49.0f, -141.0f, 41.0f, 69.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[482] = new Triangle(-132.0f, 34.0f, 49.0f, -134.0f, 65.0f, 49.0f, -129.0f, 55.0f, 29.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[483] = new Triangle(-129.0f, 60.0f, 105.0f, -118.0f, 56.0f, 114.0f, -134.0f, 53.0f, 88.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[484] = new Triangle(-140.0f, 61.0f, 70.0f, -141.0f, 41.0f, 69.0f, -134.0f, 53.0f, 88.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[485] = new Triangle(-134.0f, 65.0f, 49.0f, -140.0f, 61.0f, 70.0f, -141.0f, 41.0f, 69.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[486] = new Triangle(-133.0f, 73.0f, 26.0f, -129.0f, 55.0f, 29.0f, -134.0f, 65.0f, 49.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[487] = new Triangle(-129.0f, 55.0f, 29.0f, -133.0f, 73.0f, 26.0f, -108.0f, 73.0f, 24.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[488] = new Triangle(-100.0f, 108.0f, 1.0f, -90.0f, 69.0f, 7.0f, -108.0f, 73.0f, 24.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[489] = new Triangle(-90.0f, 69.0f, 7.0f, -100.0f, 108.0f, 1.0f, -77.0f, 91.0f, -1.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[490] = new Triangle(-57.0f, 130.0f, -12.0f, -47.0f, 97.0f, -4.0f, -77.0f, 91.0f, -1.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[491] = new Triangle(-77.0f, 91.0f, -1.0f, -100.0f, 108.0f, 1.0f, -57.0f, 130.0f, -12.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[492] = new Triangle(-21.0f, 112.0f, -9.0f, -38.0f, 129.0f, -18.0f, -47.0f, 97.0f, -4.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[493] = new Triangle(-47.0f, 97.0f, -4.0f, -57.0f, 130.0f, -12.0f, -38.0f, 129.0f, -18.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[494] = new Triangle(-7.0f, 139.0f, -21.0f, -3.0f, 123.0f, -14.0f, -21.0f, 112.0f, -9.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[495] = new Triangle(-21.0f, 112.0f, -9.0f, -7.0f, 139.0f, -21.0f, -38.0f, 129.0f, -18.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[496] = new Triangle(-7.0f, 139.0f, -21.0f, -3.0f, 123.0f, -14.0f, 19.0f, 123.0f, -10.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[497] = new Triangle(-93.0f, 62.0f, 261.0f, -59.0f, 50.0f, 276.0f, -80.0f, 33.0f, 245.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[498] = new Triangle(-80.0f, 33.0f, 245.0f, -93.0f, 62.0f, 261.0f, -105.0f, 42.0f, 237.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[499] = new Triangle(-133.0f, 54.0f, 230.0f, -126.0f, 42.0f, 219.0f, -105.0f, 42.0f, 237.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[500] = new Triangle(-126.0f, 42.0f, 219.0f, -151.0f, 42.0f, 208.0f, -133.0f, 54.0f, 230.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[501] = new Triangle(-151.0f, 42.0f, 208.0f, -167.0f, 47.0f, 189.0f, -168.0f, 59.0f, 206.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[502] = new Triangle(-153.0f, 63.0f, 221.0f, -151.0f, 42.0f, 208.0f, -133.0f, 54.0f, 230.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[503] = new Triangle(-168.0f, 59.0f, 206.0f, -151.0f, 42.0f, 208.0f, -153.0f, 63.0f, 221.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[504] = new Triangle(-120.0f, 68.0f, 246.0f, -105.0f, 42.0f, 237.0f, -93.0f, 62.0f, 261.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[505] = new Triangle(-120.0f, 68.0f, 246.0f, -153.0f, 63.0f, 221.0f, -105.0f, 42.0f, 237.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[506] = new Triangle(-69.0f, 62.0f, 293.0f, -59.0f, 50.0f, 276.0f, -16.0f, 42.0f, 302.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[507] = new Triangle(-16.0f, 42.0f, 302.0f, 23.0f, 33.0f, 350.0f, -12.0f, 47.0f, 336.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[508] = new Triangle(16.0f, 47.0f, 364.0f, 23.0f, 33.0f, 350.0f, 49.0f, 33.0f, 404.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[509] = new Triangle(16.0f, 47.0f, 364.0f, 23.0f, 33.0f, 350.0f, -12.0f, 47.0f, 336.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[510] = new Triangle(-37.0f, 63.0f, 319.0f, -16.0f, 42.0f, 302.0f, -12.0f, 47.0f, 336.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[511] = new Triangle(-37.0f, 63.0f, 319.0f, -16.0f, 42.0f, 302.0f, -69.0f, 62.0f, 293.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[512] = new Triangle(-69.0f, 62.0f, 293.0f, -93.0f, 62.0f, 261.0f, -59.0f, 50.0f, 276.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[513] = new Triangle(-138.0f, 68.0f, 89.0f, -134.0f, 53.0f, 88.0f, -129.0f, 60.0f, 105.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[514] = new Triangle(-134.0f, 53.0f, 88.0f, -138.0f, 68.0f, 89.0f, -140.0f, 61.0f, 70.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[515] = new Triangle(-140.0f, 61.0f, 70.0f, -138.0f, 68.0f, 89.0f, -143.0f, 75.0f, 71.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[516] = new Triangle(-143.0f, 75.0f, 52.0f, -134.0f, 65.0f, 49.0f, -140.0f, 61.0f, 70.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[517] = new Triangle(-140.0f, 61.0f, 70.0f, -143.0f, 75.0f, 71.0f, -143.0f, 75.0f, 52.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[518] = new Triangle(-133.0f, 73.0f, 26.0f, -142.0f, 83.0f, 41.0f, -134.0f, 65.0f, 49.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[519] = new Triangle(-134.0f, 65.0f, 49.0f, -142.0f, 83.0f, 41.0f, -143.0f, 75.0f, 52.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[520] = new Triangle(-133.0f, 73.0f, 26.0f, -108.0f, 73.0f, 24.0f, -123.0f, 93.0f, 20.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[521] = new Triangle(-123.0f, 93.0f, 20.0f, -108.0f, 73.0f, 24.0f, -100.0f, 108.0f, 1.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[522] = new Triangle(-123.0f, 93.0f, 20.0f, -142.0f, 83.0f, 41.0f, -133.0f, 73.0f, 26.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[523] = new Triangle(-129.0f, 47.0f, 174.0f, -140.0f, 56.0f, 152.0f, -167.0f, 47.0f, 189.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[524] = new Triangle(-174.0f, 56.0f, 169.0f, -140.0f, 56.0f, 152.0f, -167.0f, 47.0f, 189.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[525] = new Triangle(-140.0f, 56.0f, 152.0f, -174.0f, 56.0f, 169.0f, -148.0f, 66.0f, 134.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[526] = new Triangle(-148.0f, 66.0f, 134.0f, -181.0f, 66.0f, 148.0f, -174.0f, 56.0f, 169.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[527] = new Triangle(-181.0f, 66.0f, 148.0f, -189.0f, 76.0f, 127.0f, -148.0f, 66.0f, 134.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[528] = new Triangle(-189.0f, 76.0f, 127.0f, -159.0f, 76.0f, 113.0f, -148.0f, 66.0f, 134.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[529] = new Triangle(-159.0f, 76.0f, 113.0f, -169.0f, 86.0f, 91.0f, -189.0f, 76.0f, 127.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[530] = new Triangle(-189.0f, 76.0f, 127.0f, -197.0f, 86.0f, 107.0f, -169.0f, 86.0f, 91.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[531] = new Triangle(-129.0f, 47.0f, 174.0f, -140.0f, 56.0f, 152.0f, -118.0f, 47.0f, 174.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[532] = new Triangle(-118.0f, 58.0f, 163.0f, -118.0f, 47.0f, 174.0f, -140.0f, 56.0f, 152.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[533] = new Triangle(-118.0f, 47.0f, 174.0f, -101.0f, 49.0f, 166.0f, -107.0f, 52.0f, 168.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[534] = new Triangle(-107.0f, 52.0f, 168.0f, -101.0f, 49.0f, 166.0f, -97.0f, 64.0f, 159.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[535] = new Triangle(-108.0f, 66.0f, 160.0f, -107.0f, 52.0f, 168.0f, -97.0f, 64.0f, 159.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[536] = new Triangle(-108.0f, 66.0f, 160.0f, -118.0f, 58.0f, 163.0f, -107.0f, 52.0f, 168.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[537] = new Triangle(-107.0f, 52.0f, 168.0f, -118.0f, 58.0f, 163.0f, -118.0f, 47.0f, 174.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[538] = new Triangle(-104.0f, 72.0f, 153.0f, -108.0f, 66.0f, 160.0f, -97.0f, 64.0f, 159.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[539] = new Triangle(-97.0f, 64.0f, 159.0f, -104.0f, 72.0f, 153.0f, -104.0f, 68.0f, 143.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[540] = new Triangle(-122.0f, 65.0f, 156.0f, -140.0f, 56.0f, 152.0f, -118.0f, 58.0f, 163.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[541] = new Triangle(-113.0f, 72.0f, 156.0f, -108.0f, 66.0f, 160.0f, -118.0f, 58.0f, 163.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[542] = new Triangle(-118.0f, 58.0f, 163.0f, -122.0f, 65.0f, 156.0f, -113.0f, 72.0f, 156.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[543] = new Triangle(-113.0f, 72.0f, 156.0f, -104.0f, 72.0f, 153.0f, -108.0f, 66.0f, 160.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[544] = new Triangle(-123.0f, 66.0f, 114.0f, -118.0f, 56.0f, 114.0f, -104.0f, 68.0f, 143.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[545] = new Triangle(-123.0f, 66.0f, 114.0f, -138.0f, 68.0f, 89.0f, -118.0f, 56.0f, 114.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[546] = new Triangle(-110.0f, 75.0f, 138.0f, -104.0f, 68.0f, 143.0f, -104.0f, 72.0f, 153.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[547] = new Triangle(-110.0f, 75.0f, 138.0f, -123.0f, 66.0f, 114.0f, -104.0f, 68.0f, 143.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[548] = new Triangle(-140.0f, 56.0f, 152.0f, -139.0f, 66.0f, 138.0f, -148.0f, 66.0f, 134.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[549] = new Triangle(-139.0f, 66.0f, 138.0f, -140.0f, 56.0f, 152.0f, -122.0f, 65.0f, 156.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[550] = new Triangle(-113.0f, 73.0f, 150.0f, -113.0f, 72.0f, 156.0f, -122.0f, 65.0f, 156.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[551] = new Triangle(-104.0f, 72.0f, 153.0f, -113.0f, 72.0f, 156.0f, -113.0f, 73.0f, 150.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[552] = new Triangle(-113.0f, 73.0f, 150.0f, -104.0f, 72.0f, 153.0f, -110.0f, 75.0f, 138.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[553] = new Triangle(-121.0f, 73.0f, 145.0f, -113.0f, 73.0f, 150.0f, -122.0f, 65.0f, 156.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[554] = new Triangle(-122.0f, 65.0f, 156.0f, -121.0f, 73.0f, 145.0f, -139.0f, 66.0f, 138.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[555] = new Triangle(-121.0f, 73.0f, 145.0f, -113.0f, 73.0f, 150.0f, -110.0f, 75.0f, 138.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[556] = new Triangle(-121.0f, 73.0f, 145.0f, -119.0f, 73.0f, 135.0f, -110.0f, 75.0f, 138.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[557] = new Triangle(-110.0f, 75.0f, 138.0f, -119.0f, 73.0f, 135.0f, -123.0f, 66.0f, 114.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[558] = new Triangle(-127.0f, 76.0f, 133.0f, -121.0f, 73.0f, 145.0f, -119.0f, 73.0f, 135.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[559] = new Triangle(-127.0f, 76.0f, 133.0f, -121.0f, 73.0f, 145.0f, -139.0f, 66.0f, 138.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[560] = new Triangle(-127.0f, 76.0f, 133.0f, -124.0f, 72.0f, 125.0f, -119.0f, 73.0f, 135.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[561] = new Triangle(-119.0f, 73.0f, 135.0f, -123.0f, 66.0f, 114.0f, -124.0f, 72.0f, 125.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[562] = new Triangle(-159.0f, 76.0f, 113.0f, -148.0f, 66.0f, 134.0f, -147.0f, 76.0f, 123.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[563] = new Triangle(-138.0f, 79.0f, 128.0f, -127.0f, 76.0f, 133.0f, -139.0f, 66.0f, 138.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[564] = new Triangle(-148.0f, 66.0f, 134.0f, -147.0f, 76.0f, 123.0f, -139.0f, 66.0f, 138.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[565] = new Triangle(-139.0f, 66.0f, 138.0f, -138.0f, 79.0f, 128.0f, -147.0f, 76.0f, 123.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[566] = new Triangle(-167.0f, 47.0f, 189.0f, -174.0f, 56.0f, 169.0f, -182.0f, 64.0f, 186.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[567] = new Triangle(-174.0f, 56.0f, 169.0f, -181.0f, 66.0f, 148.0f, -190.0f, 77.0f, 161.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[568] = new Triangle(-189.0f, 76.0f, 127.0f, -181.0f, 66.0f, 148.0f, -198.0f, 79.0f, 139.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[569] = new Triangle(-189.0f, 76.0f, 127.0f, -197.0f, 86.0f, 107.0f, -201.0f, 92.0f, 121.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[570] = new Triangle(-201.0f, 92.0f, 121.0f, -198.0f, 79.0f, 139.0f, -189.0f, 76.0f, 127.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[571] = new Triangle(-198.0f, 79.0f, 139.0f, -190.0f, 77.0f, 161.0f, -181.0f, 66.0f, 148.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[572] = new Triangle(-190.0f, 77.0f, 161.0f, -182.0f, 64.0f, 186.0f, -174.0f, 56.0f, 169.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[573] = new Triangle(-182.0f, 64.0f, 186.0f, -168.0f, 59.0f, 206.0f, -167.0f, 47.0f, 189.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[574] = new Triangle(-169.0f, 86.0f, 91.0f, -197.0f, 86.0f, 107.0f, -201.0f, 96.0f, 89.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[575] = new Triangle(-201.0f, 96.0f, 89.0f, -169.0f, 86.0f, 91.0f, -169.0f, 96.0f, 74.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[576] = new Triangle(-201.0f, 96.0f, 89.0f, -203.0f, 101.0f, 68.0f, -169.0f, 96.0f, 74.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[577] = new Triangle(-203.0f, 101.0f, 68.0f, -171.0f, 101.0f, 58.0f, -169.0f, 96.0f, 74.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[578] = new Triangle(-171.0f, 101.0f, 58.0f, -203.0f, 101.0f, 68.0f, -202.0f, 110.0f, 56.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[579] = new Triangle(-171.0f, 101.0f, 58.0f, -171.0f, 110.0f, 46.0f, -202.0f, 110.0f, 56.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[580] = new Triangle(-202.0f, 115.0f, 43.0f, -202.0f, 110.0f, 56.0f, -171.0f, 110.0f, 46.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[581] = new Triangle(-171.0f, 110.0f, 46.0f, -202.0f, 115.0f, 43.0f, -169.0f, 115.0f, 34.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[582] = new Triangle(-169.0f, 115.0f, 34.0f, -200.0f, 120.0f, 25.0f, -202.0f, 115.0f, 43.0f, 1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[583] = new Triangle(-166.0f, 120.0f, 19.0f, -169.0f, 115.0f, 34.0f, -200.0f, 120.0f, 25.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    gameMap1[584] = new Triangle(-138.0f, 68.0f, 89.0f, -143.0f, 75.0f, 71.0f, -140.0f, 79.0f, 85.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[585] = new Triangle(-140.0f, 79.0f, 85.0f, -138.0f, 68.0f, 89.0f, -123.0f, 66.0f, 114.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[586] = new Triangle(-129.0f, 78.0f, 115.0f, -123.0f, 66.0f, 114.0f, -124.0f, 72.0f, 125.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[587] = new Triangle(-129.0f, 78.0f, 115.0f, -123.0f, 66.0f, 114.0f, -140.0f, 79.0f, 85.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[588] = new Triangle(-133.0f, 82.0f, 122.0f, -124.0f, 72.0f, 125.0f, -129.0f, 78.0f, 115.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[589] = new Triangle(-133.0f, 82.0f, 122.0f, -127.0f, 76.0f, 133.0f, -124.0f, 72.0f, 125.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[590] = new Triangle(-127.0f, 76.0f, 133.0f, -133.0f, 82.0f, 122.0f, -138.0f, 79.0f, 128.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[591] = new Triangle(-133.0f, 82.0f, 122.0f, -135.0f, 83.0f, 105.0f, -129.0f, 78.0f, 115.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[592] = new Triangle(-129.0f, 78.0f, 115.0f, -140.0f, 79.0f, 85.0f, -135.0f, 83.0f, 105.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[593] = new Triangle(-37.0f, 63.0f, 319.0f, -9.0f, 78.0f, 344.0f, -12.0f, 47.0f, 336.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[594] = new Triangle(-12.0f, 47.0f, 336.0f, 16.0f, 47.0f, 364.0f, -9.0f, 78.0f, 344.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[595] = new Triangle(-9.0f, 78.0f, 344.0f, 13.0f, 74.0f, 370.0f, 16.0f, 47.0f, 364.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[596] = new Triangle(49.0f, 33.0f, 404.0f, 13.0f, 74.0f, 370.0f, 16.0f, 47.0f, 364.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[597] = new Triangle(49.0f, 33.0f, 404.0f, 88.0f, 23.0f, 437.0f, 56.0f, 62.0f, 423.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[598] = new Triangle(49.0f, 33.0f, 404.0f, 56.0f, 62.0f, 423.0f, 42.0f, 86.0f, 407.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[599] = new Triangle(13.0f, 74.0f, 370.0f, 49.0f, 33.0f, 404.0f, 42.0f, 86.0f, 407.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[600] = new Triangle(88.0f, 23.0f, 437.0f, 120.0f, 20.0f, 451.0f, 98.0f, 65.0f, 451.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[601] = new Triangle(120.0f, 20.0f, 451.0f, 171.0f, 22.0f, 457.0f, 144.0f, 53.0f, 461.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[602] = new Triangle(171.0f, 22.0f, 457.0f, 196.0f, 22.0f, 476.0f, 201.0f, 13.0f, 442.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[603] = new Triangle(196.0f, 22.0f, 476.0f, 207.0f, 14.0f, 481.0f, 201.0f, 13.0f, 442.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[604] = new Triangle(171.0f, 22.0f, 457.0f, 196.0f, 22.0f, 476.0f, 178.0f, 54.0f, 471.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[605] = new Triangle(178.0f, 54.0f, 471.0f, 144.0f, 53.0f, 461.0f, 171.0f, 22.0f, 457.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[606] = new Triangle(120.0f, 20.0f, 451.0f, 98.0f, 65.0f, 451.0f, 144.0f, 53.0f, 461.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[607] = new Triangle(98.0f, 65.0f, 451.0f, 56.0f, 62.0f, 423.0f, 88.0f, 23.0f, 437.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[608] = new Triangle(54.0f, 95.0f, 429.0f, 56.0f, 62.0f, 423.0f, 42.0f, 86.0f, 407.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[609] = new Triangle(54.0f, 95.0f, 429.0f, 56.0f, 62.0f, 423.0f, 98.0f, 65.0f, 451.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[610] = new Triangle(98.0f, 65.0f, 451.0f, 98.0f, 95.0f, 451.0f, 54.0f, 95.0f, 429.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[611] = new Triangle(98.0f, 95.0f, 451.0f, 98.0f, 65.0f, 451.0f, 144.0f, 53.0f, 461.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[612] = new Triangle(144.0f, 53.0f, 461.0f, 145.0f, 76.0f, 470.0f, 178.0f, 54.0f, 471.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[613] = new Triangle(145.0f, 76.0f, 470.0f, 144.0f, 53.0f, 461.0f, 98.0f, 95.0f, 451.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[614] = new Triangle(-37.0f, 63.0f, 319.0f, -9.0f, 78.0f, 344.0f, -47.0f, 71.0f, 336.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[615] = new Triangle(-47.0f, 71.0f, 336.0f, -37.0f, 63.0f, 319.0f, -69.0f, 62.0f, 293.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[616] = new Triangle(-47.0f, 71.0f, 336.0f, -16.0f, 83.0f, 361.0f, -9.0f, 78.0f, 344.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[617] = new Triangle(-9.0f, 78.0f, 344.0f, 13.0f, 74.0f, 370.0f, -16.0f, 83.0f, 361.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[618] = new Triangle(-16.0f, 83.0f, 361.0f, 13.0f, 74.0f, 370.0f, 42.0f, 86.0f, 407.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[619] = new Triangle(23.0f, 97.0f, 408.0f, 42.0f, 86.0f, 407.0f, 54.0f, 95.0f, 429.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[620] = new Triangle(23.0f, 97.0f, 408.0f, -16.0f, 83.0f, 361.0f, 42.0f, 86.0f, 407.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[621] = new Triangle(-47.0f, 71.0f, 336.0f, -80.0f, 71.0f, 304.0f, -69.0f, 62.0f, 293.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[622] = new Triangle(-80.0f, 71.0f, 304.0f, -96.0f, 76.0f, 275.0f, -69.0f, 62.0f, 293.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[623] = new Triangle(-96.0f, 76.0f, 275.0f, -120.0f, 68.0f, 246.0f, -93.0f, 62.0f, 261.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[624] = new Triangle(-96.0f, 76.0f, 275.0f, -93.0f, 62.0f, 261.0f, -69.0f, 62.0f, 293.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[625] = new Triangle(98.0f, 95.0f, 451.0f, 54.0f, 95.0f, 429.0f, 46.0f, 103.0f, 453.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[626] = new Triangle(46.0f, 103.0f, 453.0f, 23.0f, 97.0f, 408.0f, 54.0f, 95.0f, 429.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[627] = new Triangle(46.0f, 103.0f, 453.0f, 5.0f, 110.0f, 434.0f, 23.0f, 97.0f, 408.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[628] = new Triangle(5.0f, 110.0f, 434.0f, 23.0f, 97.0f, 408.0f, -16.0f, 83.0f, 361.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[629] = new Triangle(-16.0f, 83.0f, 361.0f, -53.0f, 87.0f, 359.0f, 5.0f, 110.0f, 434.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[630] = new Triangle(-47.0f, 71.0f, 336.0f, -53.0f, 87.0f, 359.0f, -16.0f, 83.0f, 361.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[631] = new Triangle(-80.0f, 71.0f, 304.0f, -47.0f, 71.0f, 336.0f, -53.0f, 87.0f, 359.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[632] = new Triangle(-101.0f, 90.0f, 321.0f, -80.0f, 71.0f, 304.0f, -53.0f, 87.0f, 359.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[633] = new Triangle(-101.0f, 90.0f, 321.0f, -80.0f, 71.0f, 304.0f, -96.0f, 76.0f, 275.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[634] = new Triangle(-96.0f, 76.0f, 275.0f, -120.0f, 68.0f, 246.0f, -101.0f, 90.0f, 321.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[635] = new Triangle(-120.0f, 68.0f, 246.0f, -133.0f, 75.0f, 285.0f, -101.0f, 90.0f, 321.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[636] = new Triangle(-133.0f, 75.0f, 285.0f, -120.0f, 68.0f, 246.0f, -153.0f, 63.0f, 221.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[637] = new Triangle(-180.0f, 65.0f, 241.0f, -168.0f, 59.0f, 206.0f, -153.0f, 63.0f, 221.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[638] = new Triangle(-180.0f, 65.0f, 241.0f, -153.0f, 63.0f, 221.0f, -133.0f, 75.0f, 285.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[639] = new Triangle(-182.0f, 64.0f, 186.0f, -168.0f, 59.0f, 206.0f, -203.0f, 70.0f, 203.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[640] = new Triangle(-203.0f, 70.0f, 203.0f, -180.0f, 65.0f, 241.0f, -168.0f, 59.0f, 206.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[641] = new Triangle(-182.0f, 64.0f, 186.0f, -203.0f, 70.0f, 203.0f, -190.0f, 77.0f, 161.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[642] = new Triangle(-101.0f, 90.0f, 321.0f, -95.0f, 101.0f, 354.0f, -53.0f, 87.0f, 359.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[643] = new Triangle(-53.0f, 87.0f, 359.0f, -95.0f, 101.0f, 354.0f, -79.0f, 106.0f, 418.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[644] = new Triangle(-79.0f, 106.0f, 418.0f, -53.0f, 87.0f, 359.0f, 5.0f, 110.0f, 434.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[645] = new Triangle(5.0f, 110.0f, 434.0f, -35.0f, 113.0f, 439.0f, -79.0f, 106.0f, 418.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[646] = new Triangle(-79.0f, 106.0f, 418.0f, -57.0f, 110.0f, 459.0f, -35.0f, 113.0f, 439.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[647] = new Triangle(-35.0f, 113.0f, 439.0f, -57.0f, 110.0f, 459.0f, -22.0f, 115.0f, 483.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[648] = new Triangle(-22.0f, 115.0f, 483.0f, 5.0f, 110.0f, 434.0f, -35.0f, 113.0f, 439.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[649] = new Triangle(5.0f, 110.0f, 434.0f, 46.0f, 103.0f, 453.0f, -22.0f, 115.0f, 483.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[650] = new Triangle(-22.0f, 115.0f, 483.0f, 46.0f, 103.0f, 453.0f, 32.0f, 119.0f, 496.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[651] = new Triangle(32.0f, 119.0f, 496.0f, 46.0f, 103.0f, 453.0f, 98.0f, 95.0f, 451.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[652] = new Triangle(98.0f, 95.0f, 451.0f, 145.0f, 76.0f, 470.0f, 88.0f, 110.0f, 502.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[653] = new Triangle(88.0f, 110.0f, 502.0f, 32.0f, 119.0f, 496.0f, 98.0f, 95.0f, 451.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[654] = new Triangle(-133.0f, 75.0f, 285.0f, -101.0f, 90.0f, 321.0f, -119.0f, 101.0f, 305.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[655] = new Triangle(-133.0f, 75.0f, 285.0f, -180.0f, 65.0f, 241.0f, -161.0f, 90.0f, 273.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[656] = new Triangle(-180.0f, 65.0f, 241.0f, -203.0f, 70.0f, 203.0f, -194.0f, 93.0f, 227.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[657] = new Triangle(-101.0f, 90.0f, 321.0f, -95.0f, 101.0f, 354.0f, -103.0f, 111.0f, 333.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[658] = new Triangle(-95.0f, 101.0f, 354.0f, -79.0f, 106.0f, 418.0f, -90.0f, 125.0f, 386.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[659] = new Triangle(-98.0f, 139.0f, 357.0f, -95.0f, 101.0f, 354.0f, -103.0f, 111.0f, 333.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[660] = new Triangle(-95.0f, 101.0f, 354.0f, -90.0f, 125.0f, 386.0f, -98.0f, 139.0f, 357.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[661] = new Triangle(-108.0f, 120.0f, 319.0f, -101.0f, 90.0f, 321.0f, -119.0f, 101.0f, 305.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[662] = new Triangle(-101.0f, 90.0f, 321.0f, -103.0f, 111.0f, 333.0f, -108.0f, 120.0f, 319.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[663] = new Triangle(-135.0f, 120.0f, 292.0f, -133.0f, 75.0f, 285.0f, -161.0f, 90.0f, 273.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[664] = new Triangle(-133.0f, 75.0f, 285.0f, -135.0f, 120.0f, 292.0f, -119.0f, 101.0f, 305.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[665] = new Triangle(-180.0f, 65.0f, 241.0f, -189.0f, 106.0f, 249.0f, -161.0f, 90.0f, 273.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[666] = new Triangle(-189.0f, 106.0f, 249.0f, -180.0f, 65.0f, 241.0f, -194.0f, 93.0f, 227.0f, 1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    gameMap1[667] = new Triangle(-159.0f, 76.0f, 113.0f, -145.0f, 86.0f, 113.0f, -147.0f, 76.0f, 123.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[668] = new Triangle(-145.0f, 86.0f, 113.0f, -138.0f, 79.0f, 128.0f, -147.0f, 76.0f, 123.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[669] = new Triangle(-138.0f, 79.0f, 128.0f, -133.0f, 82.0f, 122.0f, -145.0f, 86.0f, 113.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[670] = new Triangle(-145.0f, 86.0f, 113.0f, -133.0f, 82.0f, 122.0f, -135.0f, 83.0f, 105.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[671] = new Triangle(-147.0f, 88.0f, 69.0f, -143.0f, 75.0f, 71.0f, -143.0f, 75.0f, 52.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[672] = new Triangle(-143.0f, 75.0f, 71.0f, -140.0f, 79.0f, 85.0f, -147.0f, 88.0f, 69.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[673] = new Triangle(-144.0f, 87.0f, 88.0f, -147.0f, 88.0f, 69.0f, -140.0f, 79.0f, 85.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[674] = new Triangle(-144.0f, 87.0f, 88.0f, -140.0f, 79.0f, 85.0f, -135.0f, 83.0f, 105.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[675] = new Triangle(-159.0f, 76.0f, 113.0f, -169.0f, 86.0f, 91.0f, -155.0f, 91.0f, 99.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[676] = new Triangle(-169.0f, 86.0f, 91.0f, -169.0f, 96.0f, 74.0f, -162.0f, 102.0f, 83.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[677] = new Triangle(-169.0f, 96.0f, 74.0f, -171.0f, 101.0f, 58.0f, -165.0f, 106.0f, 67.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[678] = new Triangle(-165.0f, 106.0f, 67.0f, -162.0f, 102.0f, 83.0f, -169.0f, 96.0f, 74.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[679] = new Triangle(-162.0f, 102.0f, 83.0f, -169.0f, 86.0f, 91.0f, -161.0f, 99.0f, 91.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[680] = new Triangle(-161.0f, 99.0f, 91.0f, -155.0f, 91.0f, 99.0f, -169.0f, 86.0f, 91.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[681] = new Triangle(-159.0f, 76.0f, 113.0f, -145.0f, 86.0f, 113.0f, -155.0f, 91.0f, 99.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[682] = new Triangle(-146.0f, 93.0f, 100.0f, -145.0f, 86.0f, 113.0f, -135.0f, 83.0f, 105.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[683] = new Triangle(-135.0f, 83.0f, 105.0f, -146.0f, 93.0f, 100.0f, -144.0f, 87.0f, 88.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[684] = new Triangle(-155.0f, 91.0f, 99.0f, -146.0f, 93.0f, 100.0f, -145.0f, 86.0f, 113.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[685] = new Triangle(-153.0f, 97.0f, 89.0f, -146.0f, 93.0f, 100.0f, -144.0f, 87.0f, 88.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[686] = new Triangle(-144.0f, 87.0f, 88.0f, -147.0f, 88.0f, 69.0f, -153.0f, 97.0f, 89.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[687] = new Triangle(-153.0f, 97.0f, 89.0f, -155.0f, 91.0f, 99.0f, -146.0f, 93.0f, 100.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[688] = new Triangle(-155.0f, 91.0f, 99.0f, -153.0f, 97.0f, 89.0f, -161.0f, 99.0f, 91.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[689] = new Triangle(-162.0f, 102.0f, 83.0f, -153.0f, 97.0f, 89.0f, -161.0f, 99.0f, 91.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[690] = new Triangle(-153.0f, 97.0f, 89.0f, -162.0f, 102.0f, 83.0f, -157.0f, 107.0f, 72.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[691] = new Triangle(-157.0f, 107.0f, 72.0f, -147.0f, 88.0f, 69.0f, -153.0f, 97.0f, 89.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[692] = new Triangle(-162.0f, 102.0f, 83.0f, -157.0f, 107.0f, 72.0f, -165.0f, 106.0f, 67.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    gameMap1[693] = new Triangle(-118.0f, 56.0f, 114.0f, -129.0f, 60.0f, 105.0f, -138.0f, 68.0f, 89.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);

    objects[0]->triangles[0] = *gameMap1[0];
    objects[0]->triangles[1] = *gameMap1[1];
    objects[0]->triangles[2] = *gameMap1[2];
    objects[0]->triangles[3] = *gameMap1[3];
    objects[0]->triangles[4] = *gameMap1[4];
    objects[0]->triangles[5] = *gameMap1[5];
    objects[0]->triangles[6] = *gameMap1[6];
    objects[0]->triangles[7] = *gameMap1[7];
    objects[0]->triangles[8] = *gameMap1[8];
    objects[0]->triangles[9] = *gameMap1[9];
    objects[0]->triangles[10] = *gameMap1[10];
    objects[0]->triangles[11] = *gameMap1[11];
    objects[0]->triangles[12] = *gameMap1[12];
    objects[0]->triangles[13] = *gameMap1[13];
    objects[0]->triangles[14] = *gameMap1[14];
    objects[0]->triangles[15] = *gameMap1[15];
    objects[0]->triangles[16] = *gameMap1[16];
    objects[0]->triangles[17] = *gameMap1[17];
    objects[0]->triangles[18] = *gameMap1[18];
    objects[0]->triangles[19] = *gameMap1[19];
    objects[0]->triangles[20] = *gameMap1[20];
    objects[0]->triangles[21] = *gameMap1[21];
    objects[0]->triangles[22] = *gameMap1[22];
    objects[0]->triangles[23] = *gameMap1[23];
    objects[0]->triangles[24] = *gameMap1[24];
    objects[0]->triangles[25] = *gameMap1[25];
    objects[0]->triangles[26] = *gameMap1[26];
    objects[0]->triangles[27] = *gameMap1[27];
    objects[0]->triangles[28] = *gameMap1[28];
    objects[0]->triangles[29] = *gameMap1[29];
    objects[0]->triangles[30] = *gameMap1[30];
    objects[0]->triangles[31] = *gameMap1[31];
    objects[0]->triangles[32] = *gameMap1[32];
    objects[0]->triangles[33] = *gameMap1[33];
    objects[0]->triangles[34] = *gameMap1[34];
    objects[0]->triangles[35] = *gameMap1[35];
    objects[0]->triangles[36] = *gameMap1[36];
    objects[0]->triangles[37] = *gameMap1[37];
    objects[0]->triangles[38] = *gameMap1[38];
    objects[0]->triangles[39] = *gameMap1[39];
    objects[0]->triangles[40] = *gameMap1[40];
    objects[0]->triangles[41] = *gameMap1[41];
    objects[0]->triangles[42] = *gameMap1[42];
    objects[0]->triangles[43] = *gameMap1[43];
    objects[0]->triangles[44] = *gameMap1[44];
    objects[0]->triangles[45] = *gameMap1[45];
    objects[0]->triangles[46] = *gameMap1[46];
    objects[0]->triangles[47] = *gameMap1[47];
    objects[0]->triangles[48] = *gameMap1[48];
    objects[0]->triangles[49] = *gameMap1[49];
    objects[0]->triangles[50] = *gameMap1[50];
    objects[0]->triangles[51] = *gameMap1[51];
    objects[0]->triangles[52] = *gameMap1[52];
    objects[0]->triangles[53] = *gameMap1[53];
    objects[0]->triangles[54] = *gameMap1[54];
    objects[0]->triangles[55] = *gameMap1[55];
    objects[0]->triangles[56] = *gameMap1[56];
    objects[0]->triangles[57] = *gameMap1[57];
    objects[0]->triangles[58] = *gameMap1[58];
    objects[0]->triangles[59] = *gameMap1[59];
    objects[0]->triangles[60] = *gameMap1[60];
    objects[0]->triangles[61] = *gameMap1[61];
    objects[0]->triangles[62] = *gameMap1[62];
    objects[0]->triangles[63] = *gameMap1[63];
    objects[0]->triangles[64] = *gameMap1[64];
    objects[0]->triangles[65] = *gameMap1[65];
    objects[0]->triangles[66] = *gameMap1[66];
    objects[0]->triangles[67] = *gameMap1[67];
    objects[0]->triangles[68] = *gameMap1[68];
    objects[0]->triangles[69] = *gameMap1[69];
    objects[0]->triangles[70] = *gameMap1[70];
    objects[0]->triangles[71] = *gameMap1[71];
    objects[0]->triangles[72] = *gameMap1[72];
    objects[0]->triangles[73] = *gameMap1[73];
    objects[0]->triangles[74] = *gameMap1[74];
    objects[0]->triangles[75] = *gameMap1[75];
    objects[0]->triangles[76] = *gameMap1[76];
    objects[0]->triangles[77] = *gameMap1[77];
    objects[0]->triangles[78] = *gameMap1[78];
    objects[0]->triangles[79] = *gameMap1[79];
    objects[0]->triangles[80] = *gameMap1[80];
    objects[0]->triangles[81] = *gameMap1[81];
    objects[0]->triangles[82] = *gameMap1[82];
    objects[0]->triangles[83] = *gameMap1[83];
    objects[0]->triangles[84] = *gameMap1[84];
    objects[0]->triangles[85] = *gameMap1[85];
    objects[0]->triangles[86] = *gameMap1[86];
    objects[0]->triangles[87] = *gameMap1[87];
    objects[0]->triangles[88] = *gameMap1[88];
    objects[0]->triangles[89] = *gameMap1[89];
    objects[0]->triangles[90] = *gameMap1[90];
    objects[0]->triangles[91] = *gameMap1[91];
    objects[0]->triangles[92] = *gameMap1[92];
    objects[0]->triangles[93] = *gameMap1[93];
    objects[0]->triangles[94] = *gameMap1[94];
    objects[0]->triangles[95] = *gameMap1[95];
    objects[0]->triangles[96] = *gameMap1[96];
    objects[0]->triangles[97] = *gameMap1[97];
    objects[0]->triangles[98] = *gameMap1[98];
    objects[0]->triangles[99] = *gameMap1[99];
    objects[0]->triangles[100] = *gameMap1[100];
    objects[0]->triangles[101] = *gameMap1[101];
    objects[0]->triangles[102] = *gameMap1[102];
    objects[0]->triangles[103] = *gameMap1[103];
    objects[0]->triangles[104] = *gameMap1[104];
    objects[0]->triangles[105] = *gameMap1[105];
    objects[0]->triangles[106] = *gameMap1[106];
    objects[0]->triangles[107] = *gameMap1[107];
    objects[0]->triangles[108] = *gameMap1[108];
    objects[0]->triangles[109] = *gameMap1[109];
    objects[0]->triangles[110] = *gameMap1[110];
    objects[0]->triangles[111] = *gameMap1[111];
    objects[0]->triangles[112] = *gameMap1[112];
    objects[0]->triangles[113] = *gameMap1[113];
    objects[0]->triangles[114] = *gameMap1[114];
    objects[0]->triangles[115] = *gameMap1[115];
    objects[0]->triangles[116] = *gameMap1[116];
    objects[0]->triangles[117] = *gameMap1[117];
    objects[0]->triangles[118] = *gameMap1[118];
    objects[0]->triangles[119] = *gameMap1[119];
    objects[0]->triangles[120] = *gameMap1[120];
    objects[0]->triangles[121] = *gameMap1[121];
    objects[0]->triangles[122] = *gameMap1[122];
    objects[0]->triangles[123] = *gameMap1[123];
    objects[0]->triangles[124] = *gameMap1[124];
    objects[0]->triangles[125] = *gameMap1[125];
    objects[0]->triangles[126] = *gameMap1[126];
    objects[0]->triangles[127] = *gameMap1[127];
    objects[0]->triangles[128] = *gameMap1[128];
    objects[0]->triangles[129] = *gameMap1[129];
    objects[0]->triangles[130] = *gameMap1[130];
    objects[0]->triangles[131] = *gameMap1[131];
    objects[0]->triangles[132] = *gameMap1[132];
    objects[0]->triangles[133] = *gameMap1[133];
    objects[0]->triangles[134] = *gameMap1[134];
    objects[0]->triangles[135] = *gameMap1[135];
    objects[0]->triangles[136] = *gameMap1[136];
    objects[0]->triangles[137] = *gameMap1[137];
    objects[0]->triangles[138] = *gameMap1[138];
    objects[0]->triangles[139] = *gameMap1[139];
    objects[0]->triangles[140] = *gameMap1[140];
    objects[0]->triangles[141] = *gameMap1[141];
    objects[0]->triangles[142] = *gameMap1[142];
    objects[0]->triangles[143] = *gameMap1[143];
    objects[0]->triangles[144] = *gameMap1[144];
    objects[0]->triangles[145] = *gameMap1[145];
    objects[0]->triangles[146] = *gameMap1[146];
    objects[0]->triangles[147] = *gameMap1[147];
    objects[0]->triangles[148] = *gameMap1[148];
    objects[0]->triangles[149] = *gameMap1[149];
    objects[0]->triangles[150] = *gameMap1[150];
    objects[0]->triangles[151] = *gameMap1[151];
    objects[0]->triangles[152] = *gameMap1[152];
    objects[0]->triangles[153] = *gameMap1[153];
    objects[0]->triangles[154] = *gameMap1[154];
    objects[0]->triangles[155] = *gameMap1[155];
    objects[0]->triangles[156] = *gameMap1[156];
    objects[0]->triangles[157] = *gameMap1[157];
    objects[0]->triangles[158] = *gameMap1[158];
    objects[0]->triangles[159] = *gameMap1[159];
    objects[0]->triangles[160] = *gameMap1[160];
    objects[0]->triangles[161] = *gameMap1[161];
    objects[0]->triangles[162] = *gameMap1[162];
    objects[0]->triangles[163] = *gameMap1[163];
    objects[0]->triangles[164] = *gameMap1[164];
    objects[0]->triangles[165] = *gameMap1[165];
    objects[0]->triangles[166] = *gameMap1[166];
    objects[0]->triangles[167] = *gameMap1[167];
    objects[0]->triangles[168] = *gameMap1[168];
    objects[0]->triangles[169] = *gameMap1[169];
    objects[0]->triangles[170] = *gameMap1[170];
    objects[0]->triangles[171] = *gameMap1[171];
    objects[0]->triangles[172] = *gameMap1[172];
    objects[0]->triangles[173] = *gameMap1[173];
    objects[0]->triangles[174] = *gameMap1[174];
    objects[0]->triangles[175] = *gameMap1[175];
    objects[0]->triangles[176] = *gameMap1[176];
    objects[0]->triangles[177] = *gameMap1[177];
    objects[0]->triangles[178] = *gameMap1[178];
    objects[0]->triangles[179] = *gameMap1[179];
    objects[0]->triangles[180] = *gameMap1[180];
    objects[0]->triangles[181] = *gameMap1[181];
    objects[0]->triangles[182] = *gameMap1[182];
    objects[0]->triangles[183] = *gameMap1[183];
    objects[0]->triangles[184] = *gameMap1[184];
    objects[0]->triangles[185] = *gameMap1[185];
    objects[0]->triangles[186] = *gameMap1[186];
    objects[0]->triangles[187] = *gameMap1[187];
    objects[0]->triangles[188] = *gameMap1[188];
    objects[0]->triangles[189] = *gameMap1[189];
    objects[0]->triangles[190] = *gameMap1[190];
    objects[0]->triangles[191] = *gameMap1[191];
    objects[0]->triangles[192] = *gameMap1[192];
    objects[0]->triangles[193] = *gameMap1[193];
    objects[0]->triangles[194] = *gameMap1[194];
    objects[0]->triangles[195] = *gameMap1[195];
    objects[0]->triangles[196] = *gameMap1[196];
    objects[0]->triangles[197] = *gameMap1[197];
    objects[0]->triangles[198] = *gameMap1[198];
    objects[0]->triangles[199] = *gameMap1[199];
    objects[0]->triangles[200] = *gameMap1[200];
    objects[0]->triangles[201] = *gameMap1[201];
    objects[0]->triangles[202] = *gameMap1[202];
    objects[0]->triangles[203] = *gameMap1[203];
    objects[0]->triangles[204] = *gameMap1[204];
    objects[0]->triangles[205] = *gameMap1[205];
    objects[0]->triangles[206] = *gameMap1[206];
    objects[0]->triangles[207] = *gameMap1[207];
    objects[0]->triangles[208] = *gameMap1[208];
    objects[0]->triangles[209] = *gameMap1[209];
    objects[0]->triangles[210] = *gameMap1[210];
    objects[0]->triangles[211] = *gameMap1[211];
    objects[0]->triangles[212] = *gameMap1[212];
    objects[0]->triangles[213] = *gameMap1[213];
    objects[0]->triangles[214] = *gameMap1[214];
    objects[0]->triangles[215] = *gameMap1[215];
    objects[0]->triangles[216] = *gameMap1[216];
    objects[0]->triangles[217] = *gameMap1[217];
    objects[0]->triangles[218] = *gameMap1[218];
    objects[0]->triangles[219] = *gameMap1[219];
    objects[0]->triangles[220] = *gameMap1[220];
    objects[0]->triangles[221] = *gameMap1[221];
    objects[0]->triangles[222] = *gameMap1[222];
    objects[0]->triangles[223] = *gameMap1[223];
    objects[0]->triangles[224] = *gameMap1[224];
    objects[0]->triangles[225] = *gameMap1[225];
    objects[0]->triangles[226] = *gameMap1[226];
    objects[0]->triangles[227] = *gameMap1[227];
    objects[0]->triangles[228] = *gameMap1[228];
    objects[0]->triangles[229] = *gameMap1[229];
    objects[0]->triangles[230] = *gameMap1[230];
    objects[0]->triangles[231] = *gameMap1[231];
    objects[0]->triangles[232] = *gameMap1[232];
    objects[0]->triangles[233] = *gameMap1[233];
    objects[0]->triangles[234] = *gameMap1[234];
    objects[0]->triangles[235] = *gameMap1[235];
    objects[0]->triangles[236] = *gameMap1[236];
    objects[0]->triangles[237] = *gameMap1[237];
    objects[0]->triangles[238] = *gameMap1[238];
    objects[0]->triangles[239] = *gameMap1[239];
    objects[0]->triangles[240] = *gameMap1[240];
    objects[0]->triangles[241] = *gameMap1[241];
    objects[0]->triangles[242] = *gameMap1[242];
    objects[0]->triangles[243] = *gameMap1[243];
    objects[0]->triangles[244] = *gameMap1[244];
    objects[0]->triangles[245] = *gameMap1[245];
    objects[0]->triangles[246] = *gameMap1[246];
    objects[0]->triangles[247] = *gameMap1[247];
    objects[0]->triangles[248] = *gameMap1[248];
    objects[0]->triangles[249] = *gameMap1[249];
    objects[0]->triangles[250] = *gameMap1[250];
    objects[0]->triangles[251] = *gameMap1[251];
    objects[0]->triangles[252] = *gameMap1[252];
    objects[0]->triangles[253] = *gameMap1[253];
    objects[0]->triangles[254] = *gameMap1[254];
    objects[0]->triangles[255] = *gameMap1[255];
    objects[0]->triangles[256] = *gameMap1[256];
    objects[0]->triangles[257] = *gameMap1[257];
    objects[0]->triangles[258] = *gameMap1[258];
    objects[0]->triangles[259] = *gameMap1[259];
    objects[0]->triangles[260] = *gameMap1[260];
    objects[0]->triangles[261] = *gameMap1[261];
    objects[0]->triangles[262] = *gameMap1[262];
    objects[0]->triangles[263] = *gameMap1[263];
    objects[0]->triangles[264] = *gameMap1[264];
    objects[0]->triangles[265] = *gameMap1[265];
    objects[0]->triangles[266] = *gameMap1[266];
    objects[0]->triangles[267] = *gameMap1[267];
    objects[0]->triangles[268] = *gameMap1[268];
    objects[0]->triangles[269] = *gameMap1[269];
    objects[0]->triangles[270] = *gameMap1[270];
    objects[0]->triangles[271] = *gameMap1[271];
    objects[0]->triangles[272] = *gameMap1[272];
    objects[0]->triangles[273] = *gameMap1[273];
    objects[0]->triangles[274] = *gameMap1[274];
    objects[0]->triangles[275] = *gameMap1[275];
    objects[0]->triangles[276] = *gameMap1[276];
    objects[0]->triangles[277] = *gameMap1[277];
    objects[0]->triangles[278] = *gameMap1[278];
    objects[0]->triangles[279] = *gameMap1[279];
    objects[0]->triangles[280] = *gameMap1[280];
    objects[0]->triangles[281] = *gameMap1[281];
    objects[0]->triangles[282] = *gameMap1[282];
    objects[0]->triangles[283] = *gameMap1[283];
    objects[0]->triangles[284] = *gameMap1[284];
    objects[0]->triangles[285] = *gameMap1[285];
    objects[0]->triangles[286] = *gameMap1[286];
    objects[0]->triangles[287] = *gameMap1[287];
    objects[0]->triangles[288] = *gameMap1[288];
    objects[0]->triangles[289] = *gameMap1[289];
    objects[0]->triangles[290] = *gameMap1[290];
    objects[0]->triangles[291] = *gameMap1[291];
    objects[0]->triangles[292] = *gameMap1[292];
    objects[0]->triangles[293] = *gameMap1[293];
    objects[0]->triangles[294] = *gameMap1[294];
    objects[0]->triangles[295] = *gameMap1[295];
    objects[0]->triangles[296] = *gameMap1[296];
    objects[0]->triangles[297] = *gameMap1[297];
    objects[0]->triangles[298] = *gameMap1[298];
    objects[0]->triangles[299] = *gameMap1[299];
    objects[0]->triangles[300] = *gameMap1[300];
    objects[0]->triangles[301] = *gameMap1[301];
    objects[0]->triangles[302] = *gameMap1[302];
    objects[0]->triangles[303] = *gameMap1[303];
    objects[0]->triangles[304] = *gameMap1[304];
    objects[0]->triangles[305] = *gameMap1[305];
    objects[0]->triangles[306] = *gameMap1[306];
    objects[0]->triangles[307] = *gameMap1[307];
    objects[0]->triangles[308] = *gameMap1[308];
    objects[0]->triangles[309] = *gameMap1[309];
    objects[0]->triangles[310] = *gameMap1[310];
    objects[0]->triangles[311] = *gameMap1[311];
    objects[0]->triangles[312] = *gameMap1[312];
    objects[0]->triangles[313] = *gameMap1[313];
    objects[0]->triangles[314] = *gameMap1[314];
    objects[0]->triangles[315] = *gameMap1[315];
    objects[0]->triangles[316] = *gameMap1[316];
    objects[0]->triangles[317] = *gameMap1[317];
    objects[0]->triangles[318] = *gameMap1[318];
    objects[0]->triangles[319] = *gameMap1[319];
    objects[0]->triangles[320] = *gameMap1[320];
    objects[0]->triangles[321] = *gameMap1[321];
    objects[0]->triangles[322] = *gameMap1[322];
    objects[0]->triangles[323] = *gameMap1[323];
    objects[0]->triangles[324] = *gameMap1[324];
    objects[0]->triangles[325] = *gameMap1[325];
    objects[0]->triangles[326] = *gameMap1[326];
    objects[0]->triangles[327] = *gameMap1[327];
    objects[0]->triangles[328] = *gameMap1[328];
    objects[0]->triangles[329] = *gameMap1[329];
    objects[0]->triangles[330] = *gameMap1[330];
    objects[0]->triangles[331] = *gameMap1[331];
    objects[0]->triangles[332] = *gameMap1[332];
    objects[0]->triangles[333] = *gameMap1[333];
    objects[0]->triangles[334] = *gameMap1[334];
    objects[0]->triangles[335] = *gameMap1[335];
    objects[0]->triangles[336] = *gameMap1[336];
    objects[0]->triangles[337] = *gameMap1[337];
    objects[0]->triangles[338] = *gameMap1[338];
    objects[0]->triangles[339] = *gameMap1[339];
    objects[0]->triangles[340] = *gameMap1[340];
    objects[0]->triangles[341] = *gameMap1[341];
    objects[0]->triangles[342] = *gameMap1[342];
    objects[0]->triangles[343] = *gameMap1[343];
    objects[0]->triangles[344] = *gameMap1[344];
    objects[0]->triangles[345] = *gameMap1[345];
    objects[0]->triangles[346] = *gameMap1[346];
    objects[0]->triangles[347] = *gameMap1[347];
    objects[0]->triangles[348] = *gameMap1[348];
    objects[0]->triangles[349] = *gameMap1[349];
    objects[0]->triangles[350] = *gameMap1[350];
    objects[0]->triangles[351] = *gameMap1[351];
    objects[0]->triangles[352] = *gameMap1[352];
    objects[0]->triangles[353] = *gameMap1[353];
    objects[0]->triangles[354] = *gameMap1[354];
    objects[0]->triangles[355] = *gameMap1[355];
    objects[0]->triangles[356] = *gameMap1[356];
    objects[0]->triangles[357] = *gameMap1[357];
    objects[0]->triangles[358] = *gameMap1[358];
    objects[0]->triangles[359] = *gameMap1[359];
    objects[0]->triangles[360] = *gameMap1[360];
    objects[0]->triangles[361] = *gameMap1[361];
    objects[0]->triangles[362] = *gameMap1[362];
    objects[0]->triangles[363] = *gameMap1[363];
    objects[0]->triangles[364] = *gameMap1[364];
    objects[0]->triangles[365] = *gameMap1[365];
    objects[0]->triangles[366] = *gameMap1[366];
    objects[0]->triangles[367] = *gameMap1[367];
    objects[0]->triangles[368] = *gameMap1[368];
    objects[0]->triangles[369] = *gameMap1[369];
    objects[0]->triangles[370] = *gameMap1[370];
    objects[0]->triangles[371] = *gameMap1[371];
    objects[0]->triangles[372] = *gameMap1[372];
    objects[0]->triangles[373] = *gameMap1[373];
    objects[0]->triangles[374] = *gameMap1[374];
    objects[0]->triangles[375] = *gameMap1[375];
    objects[0]->triangles[376] = *gameMap1[376];
    objects[0]->triangles[377] = *gameMap1[377];
    objects[0]->triangles[378] = *gameMap1[378];
    objects[0]->triangles[379] = *gameMap1[379];
    objects[0]->triangles[380] = *gameMap1[380];
    objects[0]->triangles[381] = *gameMap1[381];
    objects[0]->triangles[382] = *gameMap1[382];
    objects[0]->triangles[383] = *gameMap1[383];
    objects[0]->triangles[384] = *gameMap1[384];
    objects[0]->triangles[385] = *gameMap1[385];
    objects[0]->triangles[386] = *gameMap1[386];
    objects[0]->triangles[387] = *gameMap1[387];
    objects[0]->triangles[388] = *gameMap1[388];
    objects[0]->triangles[389] = *gameMap1[389];
    objects[0]->triangles[390] = *gameMap1[390];
    objects[0]->triangles[391] = *gameMap1[391];
    objects[0]->triangles[392] = *gameMap1[392];
    objects[0]->triangles[393] = *gameMap1[393];
    objects[0]->triangles[394] = *gameMap1[394];
    objects[0]->triangles[395] = *gameMap1[395];
    objects[0]->triangles[396] = *gameMap1[396];
    objects[0]->triangles[397] = *gameMap1[397];
    objects[0]->triangles[398] = *gameMap1[398];
    objects[0]->triangles[399] = *gameMap1[399];
    objects[0]->triangles[400] = *gameMap1[400];
    objects[0]->triangles[401] = *gameMap1[401];
    objects[0]->triangles[402] = *gameMap1[402];
    objects[0]->triangles[403] = *gameMap1[403];
    objects[0]->triangles[404] = *gameMap1[404];
    objects[0]->triangles[405] = *gameMap1[405];
    objects[0]->triangles[406] = *gameMap1[406];
    objects[0]->triangles[407] = *gameMap1[407];
    objects[0]->triangles[408] = *gameMap1[408];
    objects[0]->triangles[409] = *gameMap1[409];
    objects[0]->triangles[410] = *gameMap1[410];
    objects[0]->triangles[411] = *gameMap1[411];
    objects[0]->triangles[412] = *gameMap1[412];
    objects[0]->triangles[413] = *gameMap1[413];
    objects[0]->triangles[414] = *gameMap1[414];
    objects[0]->triangles[415] = *gameMap1[415];
    objects[0]->triangles[416] = *gameMap1[416];
    objects[0]->triangles[417] = *gameMap1[417];
    objects[0]->triangles[418] = *gameMap1[418];
    objects[0]->triangles[419] = *gameMap1[419];
    objects[0]->triangles[420] = *gameMap1[420];
    objects[0]->triangles[421] = *gameMap1[421];
    objects[0]->triangles[422] = *gameMap1[422];
    objects[0]->triangles[423] = *gameMap1[423];
    objects[0]->triangles[424] = *gameMap1[424];
    objects[0]->triangles[425] = *gameMap1[425];
    objects[0]->triangles[426] = *gameMap1[426];
    objects[0]->triangles[427] = *gameMap1[427];
    objects[0]->triangles[428] = *gameMap1[428];
    objects[0]->triangles[429] = *gameMap1[429];
    objects[0]->triangles[430] = *gameMap1[430];
    objects[0]->triangles[431] = *gameMap1[431];
    objects[0]->triangles[432] = *gameMap1[432];
    objects[0]->triangles[433] = *gameMap1[433];
    objects[0]->triangles[434] = *gameMap1[434];
    objects[0]->triangles[435] = *gameMap1[435];
    objects[0]->triangles[436] = *gameMap1[436];
    objects[0]->triangles[437] = *gameMap1[437];
    objects[0]->triangles[438] = *gameMap1[438];
    objects[0]->triangles[439] = *gameMap1[439];
    objects[0]->triangles[440] = *gameMap1[440];
    objects[0]->triangles[441] = *gameMap1[441];
    objects[0]->triangles[442] = *gameMap1[442];
    objects[0]->triangles[443] = *gameMap1[443];
    objects[0]->triangles[444] = *gameMap1[444];
    objects[0]->triangles[445] = *gameMap1[445];
    objects[0]->triangles[446] = *gameMap1[446];
    objects[0]->triangles[447] = *gameMap1[447];
    objects[0]->triangles[448] = *gameMap1[448];
    objects[0]->triangles[449] = *gameMap1[449];
    objects[0]->triangles[450] = *gameMap1[450];
    objects[0]->triangles[451] = *gameMap1[451];
    objects[0]->triangles[452] = *gameMap1[452];
    objects[0]->triangles[453] = *gameMap1[453];
    objects[0]->triangles[454] = *gameMap1[454];
    objects[0]->triangles[455] = *gameMap1[455];
    objects[0]->triangles[456] = *gameMap1[456];
    objects[0]->triangles[457] = *gameMap1[457];
    objects[0]->triangles[458] = *gameMap1[458];
    objects[0]->triangles[459] = *gameMap1[459];
    objects[0]->triangles[460] = *gameMap1[460];
    objects[0]->triangles[461] = *gameMap1[461];
    objects[0]->triangles[462] = *gameMap1[462];
    objects[0]->triangles[463] = *gameMap1[463];
    objects[0]->triangles[464] = *gameMap1[464];
    objects[0]->triangles[465] = *gameMap1[465];
    objects[0]->triangles[466] = *gameMap1[466];
    objects[0]->triangles[467] = *gameMap1[467];
    objects[0]->triangles[468] = *gameMap1[468];
    objects[0]->triangles[469] = *gameMap1[469];
    objects[0]->triangles[470] = *gameMap1[470];
    objects[0]->triangles[471] = *gameMap1[471];
    objects[0]->triangles[472] = *gameMap1[472];
    objects[0]->triangles[473] = *gameMap1[473];
    objects[0]->triangles[474] = *gameMap1[474];
    objects[0]->triangles[475] = *gameMap1[475];
    objects[0]->triangles[476] = *gameMap1[476];
    objects[0]->triangles[477] = *gameMap1[477];
    objects[0]->triangles[478] = *gameMap1[478];
    objects[0]->triangles[479] = *gameMap1[479];
    objects[0]->triangles[480] = *gameMap1[480];
    objects[0]->triangles[481] = *gameMap1[481];
    objects[0]->triangles[482] = *gameMap1[482];
    objects[0]->triangles[483] = *gameMap1[483];
    objects[0]->triangles[484] = *gameMap1[484];
    objects[0]->triangles[485] = *gameMap1[485];
    objects[0]->triangles[486] = *gameMap1[486];
    objects[0]->triangles[487] = *gameMap1[487];
    objects[0]->triangles[488] = *gameMap1[488];
    objects[0]->triangles[489] = *gameMap1[489];
    objects[0]->triangles[490] = *gameMap1[490];
    objects[0]->triangles[491] = *gameMap1[491];
    objects[0]->triangles[492] = *gameMap1[492];
    objects[0]->triangles[493] = *gameMap1[493];
    objects[0]->triangles[494] = *gameMap1[494];
    objects[0]->triangles[495] = *gameMap1[495];
    objects[0]->triangles[496] = *gameMap1[496];
    objects[0]->triangles[497] = *gameMap1[497];
    objects[0]->triangles[498] = *gameMap1[498];
    objects[0]->triangles[499] = *gameMap1[499];
    objects[0]->triangles[500] = *gameMap1[500];
    objects[0]->triangles[501] = *gameMap1[501];
    objects[0]->triangles[502] = *gameMap1[502];
    objects[0]->triangles[503] = *gameMap1[503];
    objects[0]->triangles[504] = *gameMap1[504];
    objects[0]->triangles[505] = *gameMap1[505];
    objects[0]->triangles[506] = *gameMap1[506];
    objects[0]->triangles[507] = *gameMap1[507];
    objects[0]->triangles[508] = *gameMap1[508];
    objects[0]->triangles[509] = *gameMap1[509];
    objects[0]->triangles[510] = *gameMap1[510];
    objects[0]->triangles[511] = *gameMap1[511];
    objects[0]->triangles[512] = *gameMap1[512];
    objects[0]->triangles[513] = *gameMap1[513];
    objects[0]->triangles[514] = *gameMap1[514];
    objects[0]->triangles[515] = *gameMap1[515];
    objects[0]->triangles[516] = *gameMap1[516];
    objects[0]->triangles[517] = *gameMap1[517];
    objects[0]->triangles[518] = *gameMap1[518];
    objects[0]->triangles[519] = *gameMap1[519];
    objects[0]->triangles[520] = *gameMap1[520];
    objects[0]->triangles[521] = *gameMap1[521];
    objects[0]->triangles[522] = *gameMap1[522];
    objects[0]->triangles[523] = *gameMap1[523];
    objects[0]->triangles[524] = *gameMap1[524];
    objects[0]->triangles[525] = *gameMap1[525];
    objects[0]->triangles[526] = *gameMap1[526];
    objects[0]->triangles[527] = *gameMap1[527];
    objects[0]->triangles[528] = *gameMap1[528];
    objects[0]->triangles[529] = *gameMap1[529];
    objects[0]->triangles[530] = *gameMap1[530];
    objects[0]->triangles[531] = *gameMap1[531];
    objects[0]->triangles[532] = *gameMap1[532];
    objects[0]->triangles[533] = *gameMap1[533];
    objects[0]->triangles[534] = *gameMap1[534];
    objects[0]->triangles[535] = *gameMap1[535];
    objects[0]->triangles[536] = *gameMap1[536];
    objects[0]->triangles[537] = *gameMap1[537];
    objects[0]->triangles[538] = *gameMap1[538];
    objects[0]->triangles[539] = *gameMap1[539];
    objects[0]->triangles[540] = *gameMap1[540];
    objects[0]->triangles[541] = *gameMap1[541];
    objects[0]->triangles[542] = *gameMap1[542];
    objects[0]->triangles[543] = *gameMap1[543];
    objects[0]->triangles[544] = *gameMap1[544];
    objects[0]->triangles[545] = *gameMap1[545];
    objects[0]->triangles[546] = *gameMap1[546];
    objects[0]->triangles[547] = *gameMap1[547];
    objects[0]->triangles[548] = *gameMap1[548];
    objects[0]->triangles[549] = *gameMap1[549];
    objects[0]->triangles[550] = *gameMap1[550];
    objects[0]->triangles[551] = *gameMap1[551];
    objects[0]->triangles[552] = *gameMap1[552];
    objects[0]->triangles[553] = *gameMap1[553];
    objects[0]->triangles[554] = *gameMap1[554];
    objects[0]->triangles[555] = *gameMap1[555];
    objects[0]->triangles[556] = *gameMap1[556];
    objects[0]->triangles[557] = *gameMap1[557];
    objects[0]->triangles[558] = *gameMap1[558];
    objects[0]->triangles[559] = *gameMap1[559];
    objects[0]->triangles[560] = *gameMap1[560];
    objects[0]->triangles[561] = *gameMap1[561];
    objects[0]->triangles[562] = *gameMap1[562];
    objects[0]->triangles[563] = *gameMap1[563];
    objects[0]->triangles[564] = *gameMap1[564];
    objects[0]->triangles[565] = *gameMap1[565];
    objects[0]->triangles[566] = *gameMap1[566];
    objects[0]->triangles[567] = *gameMap1[567];
    objects[0]->triangles[568] = *gameMap1[568];
    objects[0]->triangles[569] = *gameMap1[569];
    objects[0]->triangles[570] = *gameMap1[570];
    objects[0]->triangles[571] = *gameMap1[571];
    objects[0]->triangles[572] = *gameMap1[572];
    objects[0]->triangles[573] = *gameMap1[573];
    objects[0]->triangles[574] = *gameMap1[574];
    objects[0]->triangles[575] = *gameMap1[575];
    objects[0]->triangles[576] = *gameMap1[576];
    objects[0]->triangles[577] = *gameMap1[577];
    objects[0]->triangles[578] = *gameMap1[578];
    objects[0]->triangles[579] = *gameMap1[579];
    objects[0]->triangles[580] = *gameMap1[580];
    objects[0]->triangles[581] = *gameMap1[581];
    objects[0]->triangles[582] = *gameMap1[582];
    objects[0]->triangles[583] = *gameMap1[583];
    objects[0]->triangles[584] = *gameMap1[584];
    objects[0]->triangles[585] = *gameMap1[585];
    objects[0]->triangles[586] = *gameMap1[586];
    objects[0]->triangles[587] = *gameMap1[587];
    objects[0]->triangles[588] = *gameMap1[588];
    objects[0]->triangles[589] = *gameMap1[589];
    objects[0]->triangles[590] = *gameMap1[590];
    objects[0]->triangles[591] = *gameMap1[591];
    objects[0]->triangles[592] = *gameMap1[592];
    objects[0]->triangles[593] = *gameMap1[593];
    objects[0]->triangles[594] = *gameMap1[594];
    objects[0]->triangles[595] = *gameMap1[595];
    objects[0]->triangles[596] = *gameMap1[596];
    objects[0]->triangles[597] = *gameMap1[597];
    objects[0]->triangles[598] = *gameMap1[598];
    objects[0]->triangles[599] = *gameMap1[599];
    objects[0]->triangles[600] = *gameMap1[600];
    objects[0]->triangles[601] = *gameMap1[601];
    objects[0]->triangles[602] = *gameMap1[602];
    objects[0]->triangles[603] = *gameMap1[603];
    objects[0]->triangles[604] = *gameMap1[604];
    objects[0]->triangles[605] = *gameMap1[605];
    objects[0]->triangles[606] = *gameMap1[606];
    objects[0]->triangles[607] = *gameMap1[607];
    objects[0]->triangles[608] = *gameMap1[608];
    objects[0]->triangles[609] = *gameMap1[609];
    objects[0]->triangles[610] = *gameMap1[610];
    objects[0]->triangles[611] = *gameMap1[611];
    objects[0]->triangles[612] = *gameMap1[612];
    objects[0]->triangles[613] = *gameMap1[613];
    objects[0]->triangles[614] = *gameMap1[614];
    objects[0]->triangles[615] = *gameMap1[615];
    objects[0]->triangles[616] = *gameMap1[616];
    objects[0]->triangles[617] = *gameMap1[617];
    objects[0]->triangles[618] = *gameMap1[618];
    objects[0]->triangles[619] = *gameMap1[619];
    objects[0]->triangles[620] = *gameMap1[620];
    objects[0]->triangles[621] = *gameMap1[621];
    objects[0]->triangles[622] = *gameMap1[622];
    objects[0]->triangles[623] = *gameMap1[623];
    objects[0]->triangles[624] = *gameMap1[624];
    objects[0]->triangles[625] = *gameMap1[625];
    objects[0]->triangles[626] = *gameMap1[626];
    objects[0]->triangles[627] = *gameMap1[627];
    objects[0]->triangles[628] = *gameMap1[628];
    objects[0]->triangles[629] = *gameMap1[629];
    objects[0]->triangles[630] = *gameMap1[630];
    objects[0]->triangles[631] = *gameMap1[631];
    objects[0]->triangles[632] = *gameMap1[632];
    objects[0]->triangles[633] = *gameMap1[633];
    objects[0]->triangles[634] = *gameMap1[634];
    objects[0]->triangles[635] = *gameMap1[635];
    objects[0]->triangles[636] = *gameMap1[636];
    objects[0]->triangles[637] = *gameMap1[637];
    objects[0]->triangles[638] = *gameMap1[638];
    objects[0]->triangles[639] = *gameMap1[639];
    objects[0]->triangles[640] = *gameMap1[640];
    objects[0]->triangles[641] = *gameMap1[641];
    objects[0]->triangles[642] = *gameMap1[642];
    objects[0]->triangles[643] = *gameMap1[643];
    objects[0]->triangles[644] = *gameMap1[644];
    objects[0]->triangles[645] = *gameMap1[645];
    objects[0]->triangles[646] = *gameMap1[646];
    objects[0]->triangles[647] = *gameMap1[647];
    objects[0]->triangles[648] = *gameMap1[648];
    objects[0]->triangles[649] = *gameMap1[649];
    objects[0]->triangles[650] = *gameMap1[650];
    objects[0]->triangles[651] = *gameMap1[651];
    objects[0]->triangles[652] = *gameMap1[652];
    objects[0]->triangles[653] = *gameMap1[653];
    objects[0]->triangles[654] = *gameMap1[654];
    objects[0]->triangles[655] = *gameMap1[655];
    objects[0]->triangles[656] = *gameMap1[656];
    objects[0]->triangles[657] = *gameMap1[657];
    objects[0]->triangles[658] = *gameMap1[658];
    objects[0]->triangles[659] = *gameMap1[659];
    objects[0]->triangles[660] = *gameMap1[660];
    objects[0]->triangles[661] = *gameMap1[661];
    objects[0]->triangles[662] = *gameMap1[662];
    objects[0]->triangles[663] = *gameMap1[663];
    objects[0]->triangles[664] = *gameMap1[664];
    objects[0]->triangles[665] = *gameMap1[665];
    objects[0]->triangles[666] = *gameMap1[666];
    objects[0]->triangles[667] = *gameMap1[667];
    objects[0]->triangles[668] = *gameMap1[668];
    objects[0]->triangles[669] = *gameMap1[669];
    objects[0]->triangles[670] = *gameMap1[670];
    objects[0]->triangles[671] = *gameMap1[671];
    objects[0]->triangles[672] = *gameMap1[672];
    objects[0]->triangles[673] = *gameMap1[673];
    objects[0]->triangles[674] = *gameMap1[674];
    objects[0]->triangles[675] = *gameMap1[675];
    objects[0]->triangles[676] = *gameMap1[676];
    objects[0]->triangles[677] = *gameMap1[677];
    objects[0]->triangles[678] = *gameMap1[678];
    objects[0]->triangles[679] = *gameMap1[679];
    objects[0]->triangles[680] = *gameMap1[680];
    objects[0]->triangles[681] = *gameMap1[681];
    objects[0]->triangles[682] = *gameMap1[682];
    objects[0]->triangles[683] = *gameMap1[683];
    objects[0]->triangles[684] = *gameMap1[684];
    objects[0]->triangles[685] = *gameMap1[685];
    objects[0]->triangles[686] = *gameMap1[686];
    objects[0]->triangles[687] = *gameMap1[687];
    objects[0]->triangles[688] = *gameMap1[688];
    objects[0]->triangles[689] = *gameMap1[689];
    objects[0]->triangles[690] = *gameMap1[690];
    objects[0]->triangles[691] = *gameMap1[691];
    objects[0]->triangles[692] = *gameMap1[692];
    objects[0]->triangles[693] = *gameMap1[693];

    //TREE

    tree = new Triangle * [264];

    tree[0] = new Triangle(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 40.0f, 27.0f, 0.0f, 30.0f, -1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[1] = new Triangle(0.0f, 0.0f, 0.0f, 27.0f, 0.0f, 30.0f, 35.0f, 0.0f, 2.0f, -1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[2] = new Triangle(0.0f, 0.0f, 0.0f, -36.0f, 0.0f, 2.0f, -27.0f, 0.0f, 28.0f, -1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[3] = new Triangle(-27.0f, 0.0f, 28.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 40.0f, 1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[4] = new Triangle(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -32.0f, -24.0f, 0.0f, -24.0f, -1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[5] = new Triangle(-36.0f, 0.0f, 2.0f, -24.0f, 0.0f, -24.0f, 0.0f, 0.0f, 0.0f, 1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[6] = new Triangle(0.0f, 0.0f, -32.0f, 0.0f, 0.0f, 0.0f, 21.0f, 0.0f, -22.0f, -1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[7] = new Triangle(21.0f, 0.0f, -22.0f, 35.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f, static_cast<float>(textures[4].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[4].height), static_cast<float>(textures[4].width), static_cast<float>(textures[4].height), textures[4]);
    tree[8] = new Triangle(0.0f, 0.0f, -32.0f, -24.0f, 0.0f, -24.0f, -24.0f, 424.0f, -24.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[9] = new Triangle(0.0f, 0.0f, -32.0f, 0.0f, 423.0f, -32.0f, -24.0f, 424.0f, -24.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[10] = new Triangle(-24.0f, 424.0f, -24.0f, -24.0f, 0.0f, -24.0f, -36.0f, 0.0f, 2.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[11] = new Triangle(-36.0f, 0.0f, 2.0f, -36.0f, 422.0f, 2.0f, -24.0f, 424.0f, -24.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[12] = new Triangle(-36.0f, 422.0f, 2.0f, -36.0f, 0.0f, 2.0f, -27.0f, 0.0f, 28.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[13] = new Triangle(-27.0f, 0.0f, 28.0f, -27.0f, 421.0f, 28.0f, -36.0f, 422.0f, 2.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[14] = new Triangle(-27.0f, 421.0f, 28.0f, -27.0f, 0.0f, 28.0f, 1.0f, 0.0f, 40.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[15] = new Triangle(1.0f, 0.0f, 40.0f, 1.0f, 424.0f, 40.0f, -27.0f, 421.0f, 28.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[16] = new Triangle(1.0f, 424.0f, 40.0f, 1.0f, 0.0f, 40.0f, 27.0f, 0.0f, 30.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[17] = new Triangle(27.0f, 0.0f, 30.0f, 27.0f, 424.0f, 30.0f, 1.0f, 424.0f, 40.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[18] = new Triangle(27.0f, 424.0f, 30.0f, 27.0f, 0.0f, 30.0f, 35.0f, 0.0f, 2.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[19] = new Triangle(35.0f, 0.0f, 2.0f, 35.0f, 422.0f, 2.0f, 27.0f, 424.0f, 30.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[20] = new Triangle(35.0f, 422.0f, 2.0f, 35.0f, 0.0f, 2.0f, 21.0f, 0.0f, -22.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[21] = new Triangle(21.0f, 0.0f, -22.0f, 21.0f, 423.0f, -22.0f, 35.0f, 422.0f, 2.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[22] = new Triangle(0.0f, 423.0f, -32.0f, 21.0f, 423.0f, -22.0f, 21.0f, 0.0f, -22.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[23] = new Triangle(21.0f, 0.0f, -22.0f, 0.0f, 0.0f, -32.0f, 0.0f, 423.0f, -32.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[24] = new Triangle(0.0f, 423.0f, -32.0f, 21.0f, 423.0f, -22.0f, 39.0f, 459.0f, -81.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[25] = new Triangle(39.0f, 459.0f, -81.0f, 53.0f, 459.0f, -73.0f, 21.0f, 423.0f, -22.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[26] = new Triangle(21.0f, 423.0f, -22.0f, 27.0f, 444.0f, -22.0f, 53.0f, 459.0f, -73.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[27] = new Triangle(53.0f, 459.0f, -73.0f, 53.0f, 473.0f, -73.0f, 27.0f, 444.0f, -22.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[28] = new Triangle(27.0f, 444.0f, -22.0f, 21.0f, 423.0f, -22.0f, 35.0f, 422.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[29] = new Triangle(0.0f, 423.0f, -32.0f, 0.0f, 442.0f, -32.0f, 39.0f, 459.0f, -81.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[30] = new Triangle(39.0f, 470.0f, -81.0f, 39.0f, 459.0f, -81.0f, 0.0f, 442.0f, -32.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[31] = new Triangle(0.0f, 442.0f, -32.0f, 0.0f, 423.0f, -32.0f, -24.0f, 424.0f, -24.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[32] = new Triangle(0.0f, 442.0f, -32.0f, 27.0f, 444.0f, -22.0f, 39.0f, 470.0f, -81.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[33] = new Triangle(53.0f, 473.0f, -73.0f, 39.0f, 470.0f, -81.0f, 27.0f, 444.0f, -22.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[34] = new Triangle(27.0f, 424.0f, 30.0f, 35.0f, 422.0f, 2.0f, 98.0f, 466.0f, 60.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[35] = new Triangle(98.0f, 466.0f, 60.0f, 107.0f, 466.0f, 37.0f, 35.0f, 422.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[36] = new Triangle(35.0f, 422.0f, 2.0f, 35.0f, 440.0f, 2.0f, 27.0f, 444.0f, -22.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[37] = new Triangle(35.0f, 440.0f, 2.0f, 35.0f, 422.0f, 2.0f, 107.0f, 466.0f, 37.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[38] = new Triangle(107.0f, 466.0f, 37.0f, 83.0f, 466.0f, 26.0f, 35.0f, 440.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[39] = new Triangle(27.0f, 424.0f, 30.0f, 27.0f, 437.0f, 30.0f, 98.0f, 466.0f, 60.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[40] = new Triangle(71.0f, 466.0f, 47.0f, 98.0f, 466.0f, 60.0f, 27.0f, 437.0f, 30.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[41] = new Triangle(27.0f, 437.0f, 30.0f, 35.0f, 440.0f, 2.0f, 71.0f, 466.0f, 47.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[42] = new Triangle(71.0f, 466.0f, 47.0f, 83.0f, 466.0f, 26.0f, 35.0f, 440.0f, 2.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[43] = new Triangle(-27.0f, 421.0f, 28.0f, -36.0f, 422.0f, 2.0f, -91.0f, 468.0f, 43.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[44] = new Triangle(-95.0f, 468.0f, 26.0f, -91.0f, 468.0f, 43.0f, -36.0f, 422.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[45] = new Triangle(-36.0f, 422.0f, 2.0f, -36.0f, 439.0f, 2.0f, -95.0f, 468.0f, 26.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[46] = new Triangle(-83.0f, 468.0f, 21.0f, -95.0f, 468.0f, 26.0f, -36.0f, 439.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[47] = new Triangle(-36.0f, 439.0f, 2.0f, -36.0f, 422.0f, 2.0f, -24.0f, 424.0f, -24.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[48] = new Triangle(-27.0f, 421.0f, 28.0f, -27.0f, 436.0f, 28.0f, -79.0f, 468.0f, 38.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[49] = new Triangle(-79.0f, 468.0f, 38.0f, -91.0f, 468.0f, 43.0f, -27.0f, 421.0f, 28.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[50] = new Triangle(-27.0f, 421.0f, 28.0f, -27.0f, 436.0f, 28.0f, 1.0f, 424.0f, 40.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[51] = new Triangle(27.0f, 424.0f, 30.0f, 27.0f, 437.0f, 30.0f, 1.0f, 424.0f, 40.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[52] = new Triangle(-24.0f, 424.0f, -24.0f, 2.0f, 437.0f, 5.0f, -36.0f, 439.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[53] = new Triangle(2.0f, 437.0f, 5.0f, -24.0f, 424.0f, -24.0f, 0.0f, 442.0f, -32.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[54] = new Triangle(0.0f, 442.0f, -32.0f, 27.0f, 444.0f, -22.0f, 2.0f, 437.0f, 5.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[55] = new Triangle(2.0f, 437.0f, 5.0f, 27.0f, 444.0f, -22.0f, 35.0f, 440.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[56] = new Triangle(35.0f, 440.0f, 2.0f, 2.0f, 437.0f, 5.0f, 27.0f, 437.0f, 30.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[57] = new Triangle(1.0f, 424.0f, 40.0f, 27.0f, 437.0f, 30.0f, 2.0f, 437.0f, 5.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[58] = new Triangle(2.0f, 437.0f, 5.0f, -27.0f, 436.0f, 28.0f, 1.0f, 424.0f, 40.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[59] = new Triangle(2.0f, 437.0f, 5.0f, -27.0f, 436.0f, 28.0f, -36.0f, 439.0f, 2.0f, -1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[60] = new Triangle(-27.0f, 436.0f, 28.0f, -36.0f, 439.0f, 2.0f, -83.0f, 468.0f, 21.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[61] = new Triangle(-83.0f, 468.0f, 21.0f, -79.0f, 468.0f, 38.0f, -27.0f, 436.0f, 28.0f, 1.0f, static_cast<float>(textures[0].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[0].height), static_cast<float>(textures[0].width), static_cast<float>(textures[0].height), textures[0]);
    tree[62] = new Triangle(-95.0f, 468.0f, 26.0f, -83.0f, 468.0f, 21.0f, -98.0f, 479.0f, -19.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[63] = new Triangle(-95.0f, 468.0f, 26.0f, -131.0f, 483.0f, 49.0f, -91.0f, 468.0f, 43.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[64] = new Triangle(-91.0f, 468.0f, 43.0f, -79.0f, 468.0f, 38.0f, -74.0f, 483.0f, 71.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[65] = new Triangle(-79.0f, 468.0f, 38.0f, -83.0f, 468.0f, 21.0f, -54.0f, 479.0f, 24.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[66] = new Triangle(-74.0f, 483.0f, 71.0f, -54.0f, 479.0f, 24.0f, -79.0f, 468.0f, 38.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[67] = new Triangle(-91.0f, 468.0f, 43.0f, -74.0f, 483.0f, 71.0f, -131.0f, 483.0f, 49.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[68] = new Triangle(-131.0f, 483.0f, 49.0f, -98.0f, 479.0f, -19.0f, -95.0f, 468.0f, 26.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[69] = new Triangle(-83.0f, 468.0f, 21.0f, -98.0f, 479.0f, -19.0f, -54.0f, 479.0f, 24.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[70] = new Triangle(-177.0f, 513.0f, 69.0f, -131.0f, 483.0f, 49.0f, -98.0f, 479.0f, -19.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[71] = new Triangle(-177.0f, 513.0f, 69.0f, -74.0f, 483.0f, 71.0f, -131.0f, 483.0f, 49.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[72] = new Triangle(98.0f, 466.0f, 60.0f, 71.0f, 466.0f, 47.0f, 62.0f, 486.0f, 96.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[73] = new Triangle(157.0f, 486.0f, 72.0f, 98.0f, 466.0f, 60.0f, 107.0f, 466.0f, 37.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[74] = new Triangle(110.0f, 490.0f, -21.0f, 107.0f, 466.0f, 37.0f, 83.0f, 466.0f, 26.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[75] = new Triangle(83.0f, 466.0f, 26.0f, 44.0f, 482.0f, 16.0f, 71.0f, 466.0f, 47.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[76] = new Triangle(44.0f, 482.0f, 16.0f, 71.0f, 466.0f, 47.0f, 62.0f, 486.0f, 96.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[77] = new Triangle(62.0f, 486.0f, 96.0f, 157.0f, 486.0f, 72.0f, 98.0f, 466.0f, 60.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[78] = new Triangle(107.0f, 466.0f, 37.0f, 110.0f, 490.0f, -21.0f, 157.0f, 486.0f, 72.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[79] = new Triangle(110.0f, 490.0f, -21.0f, 44.0f, 482.0f, 16.0f, 83.0f, 466.0f, 26.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[80] = new Triangle(53.0f, 459.0f, -73.0f, 39.0f, 459.0f, -81.0f, 67.0f, 433.0f, -111.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[81] = new Triangle(39.0f, 470.0f, -81.0f, 39.0f, 459.0f, -81.0f, 24.0f, 459.0f, -118.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[82] = new Triangle(92.0f, 463.0f, -74.0f, 53.0f, 459.0f, -73.0f, 53.0f, 473.0f, -73.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[83] = new Triangle(92.0f, 463.0f, -74.0f, 67.0f, 433.0f, -111.0f, 53.0f, 459.0f, -73.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[84] = new Triangle(39.0f, 459.0f, -81.0f, 24.0f, 459.0f, -118.0f, 67.0f, 433.0f, -111.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[85] = new Triangle(53.0f, 473.0f, -73.0f, 39.0f, 470.0f, -81.0f, 36.0f, 492.0f, -61.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[86] = new Triangle(36.0f, 492.0f, -61.0f, 24.0f, 459.0f, -118.0f, 39.0f, 470.0f, -81.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[87] = new Triangle(36.0f, 492.0f, -61.0f, 92.0f, 463.0f, -74.0f, 53.0f, 473.0f, -73.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[88] = new Triangle(67.0f, 433.0f, -111.0f, 80.0f, 442.0f, -133.0f, 92.0f, 463.0f, -74.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[89] = new Triangle(80.0f, 442.0f, -133.0f, 67.0f, 433.0f, -111.0f, 24.0f, 459.0f, -118.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[90] = new Triangle(80.0f, 442.0f, -133.0f, 90.0f, 462.0f, -152.0f, 115.0f, 462.0f, -118.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[91] = new Triangle(90.0f, 462.0f, -152.0f, 57.0f, 462.0f, -160.0f, 80.0f, 442.0f, -133.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[92] = new Triangle(80.0f, 442.0f, -133.0f, 57.0f, 462.0f, -160.0f, 24.0f, 459.0f, -118.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[93] = new Triangle(115.0f, 462.0f, -118.0f, 92.0f, 463.0f, -74.0f, 80.0f, 442.0f, -133.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[94] = new Triangle(90.0f, 462.0f, -152.0f, 116.0f, 481.0f, -146.0f, 115.0f, 462.0f, -118.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[95] = new Triangle(115.0f, 462.0f, -118.0f, 128.0f, 480.0f, -84.0f, 92.0f, 463.0f, -74.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[96] = new Triangle(92.0f, 463.0f, -74.0f, 76.0f, 491.0f, -42.0f, 36.0f, 492.0f, -61.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[97] = new Triangle(76.0f, 491.0f, -42.0f, 110.0f, 490.0f, -21.0f, 44.0f, 482.0f, 16.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[98] = new Triangle(76.0f, 491.0f, -42.0f, 128.0f, 480.0f, -84.0f, 92.0f, 463.0f, -74.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[99] = new Triangle(110.0f, 490.0f, -21.0f, 76.0f, 491.0f, -42.0f, 128.0f, 480.0f, -84.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[100] = new Triangle(159.0f, 510.0f, -22.0f, 110.0f, 490.0f, -21.0f, 128.0f, 480.0f, -84.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[101] = new Triangle(159.0f, 510.0f, -22.0f, 110.0f, 490.0f, -21.0f, 157.0f, 486.0f, 72.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[102] = new Triangle(198.0f, 539.0f, 92.0f, 157.0f, 486.0f, 72.0f, 159.0f, 510.0f, -22.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[103] = new Triangle(198.0f, 539.0f, 92.0f, 157.0f, 486.0f, 72.0f, 62.0f, 486.0f, 96.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[104] = new Triangle(16.0f, 496.0f, 62.0f, 44.0f, 482.0f, 16.0f, 62.0f, 486.0f, 96.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[105] = new Triangle(16.0f, 496.0f, 62.0f, 5.0f, 496.0f, 17.0f, 44.0f, 482.0f, 16.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[106] = new Triangle(186.0f, 502.0f, -94.0f, 128.0f, 480.0f, -84.0f, 115.0f, 462.0f, -118.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[107] = new Triangle(115.0f, 462.0f, -118.0f, 116.0f, 481.0f, -146.0f, 186.0f, 502.0f, -94.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[108] = new Triangle(186.0f, 502.0f, -94.0f, 159.0f, 510.0f, -22.0f, 128.0f, 480.0f, -84.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[109] = new Triangle(159.0f, 510.0f, -22.0f, 189.0f, 545.0f, -24.0f, 198.0f, 539.0f, 92.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[110] = new Triangle(189.0f, 545.0f, -24.0f, 186.0f, 502.0f, -94.0f, 159.0f, 510.0f, -22.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[111] = new Triangle(189.0f, 545.0f, -24.0f, 203.0f, 581.0f, -14.0f, 198.0f, 539.0f, 92.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[112] = new Triangle(189.0f, 545.0f, -24.0f, 203.0f, 563.0f, -103.0f, 186.0f, 502.0f, -94.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[113] = new Triangle(186.0f, 502.0f, -94.0f, 203.0f, 563.0f, -103.0f, 116.0f, 481.0f, -146.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[114] = new Triangle(120.0f, 502.0f, -182.0f, 116.0f, 481.0f, -146.0f, 90.0f, 462.0f, -152.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[115] = new Triangle(90.0f, 462.0f, -152.0f, 57.0f, 462.0f, -160.0f, 120.0f, 502.0f, -182.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[116] = new Triangle(120.0f, 502.0f, -182.0f, 152.0f, 518.0f, -162.0f, 116.0f, 481.0f, -146.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[117] = new Triangle(116.0f, 481.0f, -146.0f, 152.0f, 518.0f, -162.0f, 203.0f, 563.0f, -103.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[118] = new Triangle(76.0f, 491.0f, -42.0f, 44.0f, 491.0f, -22.0f, 36.0f, 492.0f, -61.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[119] = new Triangle(44.0f, 491.0f, -22.0f, 76.0f, 491.0f, -42.0f, 44.0f, 482.0f, 16.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[120] = new Triangle(44.0f, 482.0f, 16.0f, 5.0f, 496.0f, 17.0f, 44.0f, 491.0f, -22.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[121] = new Triangle(62.0f, 486.0f, 96.0f, -5.0f, 521.0f, 112.0f, 16.0f, 496.0f, 62.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[122] = new Triangle(16.0f, 496.0f, 62.0f, -25.0f, 504.0f, 53.0f, 5.0f, 496.0f, 17.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[123] = new Triangle(-54.0f, 479.0f, 24.0f, -25.0f, 504.0f, 53.0f, -74.0f, 483.0f, 71.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[124] = new Triangle(-74.0f, 483.0f, 71.0f, -25.0f, 504.0f, 53.0f, -5.0f, 521.0f, 112.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[125] = new Triangle(-5.0f, 521.0f, 112.0f, 16.0f, 496.0f, 62.0f, -25.0f, 504.0f, 53.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[126] = new Triangle(-74.0f, 483.0f, 71.0f, -94.0f, 525.0f, 112.0f, -177.0f, 513.0f, 69.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[127] = new Triangle(-94.0f, 525.0f, 112.0f, -74.0f, 483.0f, 71.0f, -5.0f, 521.0f, 112.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[128] = new Triangle(71.0f, 546.0f, 133.0f, 62.0f, 486.0f, 96.0f, 198.0f, 539.0f, 92.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[129] = new Triangle(71.0f, 546.0f, 133.0f, 62.0f, 486.0f, 96.0f, -5.0f, 521.0f, 112.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[130] = new Triangle(-5.0f, 521.0f, 112.0f, -12.0f, 577.0f, 154.0f, 71.0f, 546.0f, 133.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[131] = new Triangle(-12.0f, 577.0f, 154.0f, -94.0f, 525.0f, 112.0f, -5.0f, 521.0f, 112.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[132] = new Triangle(-177.0f, 513.0f, 69.0f, -161.0f, 519.0f, -14.0f, -98.0f, 479.0f, -19.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[133] = new Triangle(-98.0f, 479.0f, -19.0f, -36.0f, 503.0f, 0.0f, -54.0f, 479.0f, 24.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[134] = new Triangle(-54.0f, 479.0f, 24.0f, -36.0f, 503.0f, 0.0f, -25.0f, 504.0f, 53.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[135] = new Triangle(-36.0f, 503.0f, 0.0f, -25.0f, 504.0f, 53.0f, 5.0f, 496.0f, 17.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[136] = new Triangle(5.0f, 496.0f, 17.0f, 44.0f, 491.0f, -22.0f, -11.0f, 498.0f, -37.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[137] = new Triangle(-11.0f, 498.0f, -37.0f, -36.0f, 503.0f, 0.0f, 5.0f, 496.0f, 17.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[138] = new Triangle(44.0f, 491.0f, -22.0f, 36.0f, 492.0f, -61.0f, -11.0f, 498.0f, -37.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[139] = new Triangle(-11.0f, 498.0f, -37.0f, -9.0f, 493.0f, -86.0f, 36.0f, 492.0f, -61.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[140] = new Triangle(36.0f, 492.0f, -61.0f, -9.0f, 493.0f, -86.0f, 24.0f, 459.0f, -118.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[141] = new Triangle(3.0f, 496.0f, -136.0f, 24.0f, 459.0f, -118.0f, 57.0f, 462.0f, -160.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[142] = new Triangle(24.0f, 459.0f, -118.0f, 3.0f, 496.0f, -136.0f, -9.0f, 493.0f, -86.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[143] = new Triangle(-55.0f, 499.0f, -53.0f, -36.0f, 503.0f, 0.0f, -11.0f, 498.0f, -37.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[144] = new Triangle(-11.0f, 498.0f, -37.0f, -9.0f, 493.0f, -86.0f, -55.0f, 499.0f, -53.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[145] = new Triangle(-55.0f, 499.0f, -53.0f, -98.0f, 479.0f, -19.0f, -36.0f, 503.0f, 0.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[146] = new Triangle(-98.0f, 479.0f, -19.0f, -99.0f, 511.0f, -65.0f, -55.0f, 499.0f, -53.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[147] = new Triangle(-99.0f, 511.0f, -65.0f, -161.0f, 519.0f, -14.0f, -98.0f, 479.0f, -19.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[148] = new Triangle(-55.0f, 499.0f, -53.0f, -64.0f, 523.0f, -124.0f, -9.0f, 493.0f, -86.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[149] = new Triangle(-9.0f, 493.0f, -86.0f, 3.0f, 496.0f, -136.0f, -64.0f, 523.0f, -124.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[150] = new Triangle(-64.0f, 523.0f, -124.0f, -99.0f, 511.0f, -65.0f, -55.0f, 499.0f, -53.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[151] = new Triangle(-128.0f, 540.0f, -83.0f, -161.0f, 519.0f, -14.0f, -99.0f, 511.0f, -65.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[152] = new Triangle(-99.0f, 511.0f, -65.0f, -128.0f, 540.0f, -83.0f, -64.0f, 523.0f, -124.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[153] = new Triangle(-64.0f, 523.0f, -124.0f, -18.0f, 519.0f, -174.0f, 3.0f, 496.0f, -136.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[154] = new Triangle(57.0f, 462.0f, -160.0f, 37.0f, 507.0f, -188.0f, 3.0f, 496.0f, -136.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[155] = new Triangle(3.0f, 496.0f, -136.0f, 37.0f, 507.0f, -188.0f, -18.0f, 519.0f, -174.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[156] = new Triangle(37.0f, 507.0f, -188.0f, 57.0f, 462.0f, -160.0f, 120.0f, 502.0f, -182.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[157] = new Triangle(203.0f, 563.0f, -103.0f, 203.0f, 581.0f, -14.0f, 189.0f, 545.0f, -24.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[158] = new Triangle(-177.0f, 513.0f, 69.0f, -123.0f, 577.0f, 121.0f, -94.0f, 525.0f, 112.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[159] = new Triangle(-94.0f, 525.0f, 112.0f, -87.0f, 593.0f, 143.0f, -12.0f, 577.0f, 154.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[160] = new Triangle(-87.0f, 593.0f, 143.0f, -123.0f, 577.0f, 121.0f, -94.0f, 525.0f, 112.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[161] = new Triangle(-182.0f, 548.0f, -6.0f, -161.0f, 519.0f, -14.0f, -128.0f, 540.0f, -83.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[162] = new Triangle(-182.0f, 548.0f, -6.0f, -161.0f, 519.0f, -14.0f, -177.0f, 513.0f, 69.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[163] = new Triangle(-203.0f, 606.0f, 85.0f, -177.0f, 513.0f, 69.0f, -182.0f, 548.0f, -6.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[164] = new Triangle(-177.0f, 513.0f, 69.0f, -123.0f, 577.0f, 121.0f, -203.0f, 606.0f, 85.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[165] = new Triangle(-182.0f, 548.0f, -6.0f, -182.0f, 584.0f, -58.0f, -128.0f, 540.0f, -83.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[166] = new Triangle(-128.0f, 540.0f, -83.0f, -98.0f, 591.0f, -143.0f, -64.0f, 523.0f, -124.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[167] = new Triangle(-64.0f, 523.0f, -124.0f, -98.0f, 591.0f, -143.0f, -18.0f, 519.0f, -174.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[168] = new Triangle(-18.0f, 519.0f, -174.0f, 16.0f, 568.0f, -191.0f, 37.0f, 507.0f, -188.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[169] = new Triangle(79.0f, 555.0f, -190.0f, 120.0f, 502.0f, -182.0f, 37.0f, 507.0f, -188.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[170] = new Triangle(135.0f, 568.0f, -164.0f, 152.0f, 518.0f, -162.0f, 120.0f, 502.0f, -182.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[171] = new Triangle(152.0f, 518.0f, -162.0f, 135.0f, 568.0f, -164.0f, 203.0f, 563.0f, -103.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[172] = new Triangle(135.0f, 568.0f, -164.0f, 79.0f, 555.0f, -190.0f, 120.0f, 502.0f, -182.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[173] = new Triangle(37.0f, 507.0f, -188.0f, 16.0f, 568.0f, -191.0f, 79.0f, 555.0f, -190.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[174] = new Triangle(16.0f, 568.0f, -191.0f, -18.0f, 519.0f, -174.0f, -98.0f, 591.0f, -143.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[175] = new Triangle(-98.0f, 591.0f, -143.0f, -182.0f, 584.0f, -58.0f, -128.0f, 540.0f, -83.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[176] = new Triangle(-182.0f, 584.0f, -58.0f, -182.0f, 548.0f, -6.0f, -203.0f, 606.0f, 85.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[177] = new Triangle(-168.0f, 680.0f, 65.0f, -203.0f, 606.0f, 85.0f, -177.0f, 638.0f, 29.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[178] = new Triangle(-177.0f, 638.0f, 29.0f, -182.0f, 584.0f, -58.0f, -203.0f, 606.0f, 85.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[179] = new Triangle(-123.0f, 577.0f, 121.0f, -135.0f, 655.0f, 98.0f, -203.0f, 606.0f, 85.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[180] = new Triangle(-203.0f, 606.0f, 85.0f, -168.0f, 680.0f, 65.0f, -135.0f, 655.0f, 98.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[181] = new Triangle(-87.0f, 593.0f, 143.0f, -123.0f, 577.0f, 121.0f, -135.0f, 655.0f, 98.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[182] = new Triangle(80.0f, 601.0f, 139.0f, 71.0f, 546.0f, 133.0f, -12.0f, 577.0f, 154.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[183] = new Triangle(71.0f, 546.0f, 133.0f, 198.0f, 539.0f, 92.0f, 80.0f, 601.0f, 139.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[184] = new Triangle(-6.0f, 622.0f, 140.0f, -12.0f, 577.0f, 154.0f, -87.0f, 593.0f, 143.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[185] = new Triangle(-12.0f, 577.0f, 154.0f, 80.0f, 601.0f, 139.0f, -6.0f, 622.0f, 140.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[186] = new Triangle(179.0f, 629.0f, 81.0f, 198.0f, 539.0f, 92.0f, 203.0f, 581.0f, -14.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[187] = new Triangle(198.0f, 539.0f, 92.0f, 179.0f, 629.0f, 81.0f, 80.0f, 601.0f, 139.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[188] = new Triangle(77.0f, 666.0f, 114.0f, 80.0f, 601.0f, 139.0f, 179.0f, 629.0f, 81.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[189] = new Triangle(80.0f, 601.0f, 139.0f, -6.0f, 622.0f, 140.0f, 77.0f, 666.0f, 114.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[190] = new Triangle(-74.0f, 660.0f, 120.0f, -87.0f, 593.0f, 143.0f, -135.0f, 655.0f, 98.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[191] = new Triangle(-74.0f, 660.0f, 120.0f, -87.0f, 593.0f, 143.0f, -6.0f, 622.0f, 140.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[192] = new Triangle(-7.0f, 677.0f, 116.0f, -6.0f, 622.0f, 140.0f, -74.0f, 660.0f, 120.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[193] = new Triangle(-7.0f, 677.0f, 116.0f, 77.0f, 666.0f, 114.0f, -6.0f, 622.0f, 140.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[194] = new Triangle(160.0f, 622.0f, -98.0f, 135.0f, 568.0f, -164.0f, 203.0f, 563.0f, -103.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[195] = new Triangle(203.0f, 563.0f, -103.0f, 203.0f, 581.0f, -14.0f, 160.0f, 622.0f, -98.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[196] = new Triangle(178.0f, 644.0f, -22.0f, 203.0f, 581.0f, -14.0f, 160.0f, 622.0f, -98.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[197] = new Triangle(178.0f, 644.0f, -22.0f, 203.0f, 581.0f, -14.0f, 179.0f, 629.0f, 81.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[198] = new Triangle(79.0f, 555.0f, -190.0f, 79.0f, 613.0f, -176.0f, 135.0f, 568.0f, -164.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[199] = new Triangle(79.0f, 555.0f, -190.0f, 79.0f, 613.0f, -176.0f, 16.0f, 568.0f, -191.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[200] = new Triangle(-152.0f, 655.0f, -41.0f, -182.0f, 584.0f, -58.0f, -98.0f, 591.0f, -143.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[201] = new Triangle(-182.0f, 584.0f, -58.0f, -152.0f, 655.0f, -41.0f, -177.0f, 638.0f, 29.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[202] = new Triangle(-2.0f, 630.0f, -167.0f, 16.0f, 568.0f, -191.0f, -98.0f, 591.0f, -143.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[203] = new Triangle(79.0f, 613.0f, -176.0f, -2.0f, 630.0f, -167.0f, 16.0f, 568.0f, -191.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[204] = new Triangle(117.0f, 639.0f, -142.0f, 135.0f, 568.0f, -164.0f, 160.0f, 622.0f, -98.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[205] = new Triangle(117.0f, 639.0f, -142.0f, 135.0f, 568.0f, -164.0f, 79.0f, 613.0f, -176.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[206] = new Triangle(-78.0f, 661.0f, -123.0f, -98.0f, 591.0f, -143.0f, -152.0f, 655.0f, -41.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[207] = new Triangle(-78.0f, 661.0f, -123.0f, -98.0f, 591.0f, -143.0f, -2.0f, 630.0f, -167.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[208] = new Triangle(-2.0f, 630.0f, -167.0f, 60.0f, 662.0f, -138.0f, 79.0f, 613.0f, -176.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[209] = new Triangle(79.0f, 613.0f, -176.0f, 117.0f, 639.0f, -142.0f, 60.0f, 662.0f, -138.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[210] = new Triangle(-2.0f, 630.0f, -167.0f, -4.0f, 684.0f, -134.0f, -78.0f, 661.0f, -123.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[211] = new Triangle(-4.0f, 684.0f, -134.0f, 60.0f, 662.0f, -138.0f, -2.0f, 630.0f, -167.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[212] = new Triangle(160.0f, 622.0f, -98.0f, 119.0f, 668.0f, -106.0f, 117.0f, 639.0f, -142.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[213] = new Triangle(117.0f, 639.0f, -142.0f, 119.0f, 668.0f, -106.0f, 60.0f, 662.0f, -138.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[214] = new Triangle(119.0f, 668.0f, -106.0f, 160.0f, 622.0f, -98.0f, 178.0f, 644.0f, -22.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[215] = new Triangle(178.0f, 644.0f, -22.0f, 179.0f, 629.0f, 81.0f, 143.0f, 680.0f, 52.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[216] = new Triangle(143.0f, 680.0f, 52.0f, 179.0f, 629.0f, 81.0f, 77.0f, 666.0f, 114.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[217] = new Triangle(-62.0f, 702.0f, -89.0f, -78.0f, 661.0f, -123.0f, -4.0f, 684.0f, -134.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[218] = new Triangle(-62.0f, 702.0f, -89.0f, -78.0f, 661.0f, -123.0f, -152.0f, 655.0f, -41.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[219] = new Triangle(-140.0f, 708.0f, 3.0f, -177.0f, 638.0f, 29.0f, -168.0f, 680.0f, 65.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[220] = new Triangle(-140.0f, 708.0f, 3.0f, -177.0f, 638.0f, 29.0f, -152.0f, 655.0f, -41.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[221] = new Triangle(-152.0f, 655.0f, -41.0f, -140.0f, 708.0f, 3.0f, -62.0f, 702.0f, -89.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[222] = new Triangle(-104.0f, 708.0f, 83.0f, -135.0f, 655.0f, 98.0f, -74.0f, 660.0f, 120.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[223] = new Triangle(-104.0f, 708.0f, 83.0f, -74.0f, 660.0f, 120.0f, -7.0f, 677.0f, 116.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[224] = new Triangle(-135.0f, 655.0f, 98.0f, -104.0f, 708.0f, 83.0f, -168.0f, 680.0f, 65.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[225] = new Triangle(-111.0f, 721.0f, 40.0f, -104.0f, 708.0f, 83.0f, -168.0f, 680.0f, 65.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[226] = new Triangle(-168.0f, 680.0f, 65.0f, -140.0f, 708.0f, 3.0f, -111.0f, 721.0f, 40.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[227] = new Triangle(-111.0f, 721.0f, 40.0f, -97.0f, 728.0f, -20.0f, -140.0f, 708.0f, 3.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[228] = new Triangle(-140.0f, 708.0f, 3.0f, -62.0f, 702.0f, -89.0f, -97.0f, 728.0f, -20.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[229] = new Triangle(77.0f, 666.0f, 114.0f, 65.0f, 707.0f, 77.0f, -7.0f, 677.0f, 116.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[230] = new Triangle(77.0f, 666.0f, 114.0f, 65.0f, 707.0f, 77.0f, 143.0f, 680.0f, 52.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[231] = new Triangle(143.0f, 680.0f, 52.0f, 178.0f, 644.0f, -22.0f, 139.0f, 696.0f, -21.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[232] = new Triangle(139.0f, 696.0f, -21.0f, 178.0f, 644.0f, -22.0f, 119.0f, 668.0f, -106.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[233] = new Triangle(119.0f, 668.0f, -106.0f, 86.0f, 700.0f, -82.0f, 139.0f, 696.0f, -21.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[234] = new Triangle(86.0f, 700.0f, -82.0f, 119.0f, 668.0f, -106.0f, 60.0f, 662.0f, -138.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[235] = new Triangle(60.0f, 662.0f, -138.0f, 21.0f, 714.0f, -108.0f, 86.0f, 700.0f, -82.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[236] = new Triangle(21.0f, 714.0f, -108.0f, -4.0f, 684.0f, -134.0f, 60.0f, 662.0f, -138.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[237] = new Triangle(-4.0f, 684.0f, -134.0f, 21.0f, 714.0f, -108.0f, -62.0f, 702.0f, -89.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[238] = new Triangle(-62.0f, 702.0f, -89.0f, -34.0f, 735.0f, -56.0f, 21.0f, 714.0f, -108.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[239] = new Triangle(-34.0f, 735.0f, -56.0f, -62.0f, 702.0f, -89.0f, -97.0f, 728.0f, -20.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[240] = new Triangle(-104.0f, 708.0f, 83.0f, -15.0f, 722.0f, 75.0f, -7.0f, 677.0f, 116.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[241] = new Triangle(-7.0f, 677.0f, 116.0f, -15.0f, 722.0f, 75.0f, 65.0f, 707.0f, 77.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[242] = new Triangle(143.0f, 680.0f, 52.0f, 139.0f, 696.0f, -21.0f, 100.0f, 696.0f, 18.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[243] = new Triangle(100.0f, 696.0f, 18.0f, 143.0f, 680.0f, 52.0f, 65.0f, 707.0f, 77.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[244] = new Triangle(100.0f, 696.0f, 18.0f, 139.0f, 696.0f, -21.0f, 86.0f, 700.0f, -82.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[245] = new Triangle(-71.0f, 730.0f, 39.0f, -97.0f, 728.0f, -20.0f, -111.0f, 721.0f, 40.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[246] = new Triangle(-111.0f, 721.0f, 40.0f, -71.0f, 730.0f, 39.0f, -104.0f, 708.0f, 83.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[247] = new Triangle(-104.0f, 708.0f, 83.0f, -71.0f, 730.0f, 39.0f, -15.0f, 722.0f, 75.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[248] = new Triangle(-34.0f, 747.0f, -6.0f, -34.0f, 735.0f, -56.0f, -97.0f, 728.0f, -20.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[249] = new Triangle(-97.0f, 728.0f, -20.0f, -34.0f, 747.0f, -6.0f, -71.0f, 730.0f, 39.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[250] = new Triangle(-4.0f, 734.0f, 31.0f, -15.0f, 722.0f, 75.0f, -71.0f, 730.0f, 39.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[251] = new Triangle(-4.0f, 734.0f, 31.0f, -34.0f, 747.0f, -6.0f, -71.0f, 730.0f, 39.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[252] = new Triangle(44.0f, 732.0f, 39.0f, -4.0f, 734.0f, 31.0f, -15.0f, 722.0f, 75.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[253] = new Triangle(-15.0f, 722.0f, 75.0f, 44.0f, 732.0f, 39.0f, 65.0f, 707.0f, 77.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[254] = new Triangle(65.0f, 707.0f, 77.0f, 44.0f, 732.0f, 39.0f, 100.0f, 696.0f, 18.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[255] = new Triangle(51.0f, 729.0f, -2.0f, -4.0f, 734.0f, 31.0f, 44.0f, 732.0f, 39.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[256] = new Triangle(44.0f, 732.0f, 39.0f, 51.0f, 729.0f, -2.0f, 100.0f, 696.0f, 18.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[257] = new Triangle(100.0f, 696.0f, 18.0f, 51.0f, 729.0f, -2.0f, 86.0f, 700.0f, -82.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[258] = new Triangle(86.0f, 700.0f, -82.0f, 51.0f, 729.0f, -2.0f, 15.0f, 735.0f, -39.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[259] = new Triangle(15.0f, 735.0f, -39.0f, -4.0f, 734.0f, 31.0f, 51.0f, 729.0f, -2.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[260] = new Triangle(-4.0f, 734.0f, 31.0f, -34.0f, 747.0f, -6.0f, 15.0f, 735.0f, -39.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[261] = new Triangle(15.0f, 735.0f, -39.0f, -34.0f, 735.0f, -56.0f, -34.0f, 747.0f, -6.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[262] = new Triangle(15.0f, 735.0f, -39.0f, 21.0f, 714.0f, -108.0f, -34.0f, 735.0f, -56.0f, 1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);
    tree[263] = new Triangle(15.0f, 735.0f, -39.0f, 21.0f, 714.0f, -108.0f, 86.0f, 700.0f, -82.0f, -1.0f, static_cast<float>(textures[1].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[1].height), static_cast<float>(textures[1].width), static_cast<float>(textures[1].height), textures[1]);

    objects[1]->triangles[0] = *tree[0];
    objects[1]->triangles[1] = *tree[1];
    objects[1]->triangles[2] = *tree[2];
    objects[1]->triangles[3] = *tree[3];
    objects[1]->triangles[4] = *tree[4];
    objects[1]->triangles[5] = *tree[5];
    objects[1]->triangles[6] = *tree[6];
    objects[1]->triangles[7] = *tree[7];
    objects[1]->triangles[8] = *tree[8];
    objects[1]->triangles[9] = *tree[9];
    objects[1]->triangles[10] = *tree[10];
    objects[1]->triangles[11] = *tree[11];
    objects[1]->triangles[12] = *tree[12];
    objects[1]->triangles[13] = *tree[13];
    objects[1]->triangles[14] = *tree[14];
    objects[1]->triangles[15] = *tree[15];
    objects[1]->triangles[16] = *tree[16];
    objects[1]->triangles[17] = *tree[17];
    objects[1]->triangles[18] = *tree[18];
    objects[1]->triangles[19] = *tree[19];
    objects[1]->triangles[20] = *tree[20];
    objects[1]->triangles[21] = *tree[21];
    objects[1]->triangles[22] = *tree[22];
    objects[1]->triangles[23] = *tree[23];
    objects[1]->triangles[24] = *tree[24];
    objects[1]->triangles[25] = *tree[25];
    objects[1]->triangles[26] = *tree[26];
    objects[1]->triangles[27] = *tree[27];
    objects[1]->triangles[28] = *tree[28];
    objects[1]->triangles[29] = *tree[29];
    objects[1]->triangles[30] = *tree[30];
    objects[1]->triangles[31] = *tree[31];
    objects[1]->triangles[32] = *tree[32];
    objects[1]->triangles[33] = *tree[33];
    objects[1]->triangles[34] = *tree[34];
    objects[1]->triangles[35] = *tree[35];
    objects[1]->triangles[36] = *tree[36];
    objects[1]->triangles[37] = *tree[37];
    objects[1]->triangles[38] = *tree[38];
    objects[1]->triangles[39] = *tree[39];
    objects[1]->triangles[40] = *tree[40];
    objects[1]->triangles[41] = *tree[41];
    objects[1]->triangles[42] = *tree[42];
    objects[1]->triangles[43] = *tree[43];
    objects[1]->triangles[44] = *tree[44];
    objects[1]->triangles[45] = *tree[45];
    objects[1]->triangles[46] = *tree[46];
    objects[1]->triangles[47] = *tree[47];
    objects[1]->triangles[48] = *tree[48];
    objects[1]->triangles[49] = *tree[49];
    objects[1]->triangles[50] = *tree[50];
    objects[1]->triangles[51] = *tree[51];
    objects[1]->triangles[52] = *tree[52];
    objects[1]->triangles[53] = *tree[53];
    objects[1]->triangles[54] = *tree[54];
    objects[1]->triangles[55] = *tree[55];
    objects[1]->triangles[56] = *tree[56];
    objects[1]->triangles[57] = *tree[57];
    objects[1]->triangles[58] = *tree[58];
    objects[1]->triangles[59] = *tree[59];
    objects[1]->triangles[60] = *tree[60];
    objects[1]->triangles[61] = *tree[61];
    objects[1]->triangles[62] = *tree[62];
    objects[1]->triangles[63] = *tree[63];
    objects[1]->triangles[64] = *tree[64];
    objects[1]->triangles[65] = *tree[65];
    objects[1]->triangles[66] = *tree[66];
    objects[1]->triangles[67] = *tree[67];
    objects[1]->triangles[68] = *tree[68];
    objects[1]->triangles[69] = *tree[69];
    objects[1]->triangles[70] = *tree[70];
    objects[1]->triangles[71] = *tree[71];
    objects[1]->triangles[72] = *tree[72];
    objects[1]->triangles[73] = *tree[73];
    objects[1]->triangles[74] = *tree[74];
    objects[1]->triangles[75] = *tree[75];
    objects[1]->triangles[76] = *tree[76];
    objects[1]->triangles[77] = *tree[77];
    objects[1]->triangles[78] = *tree[78];
    objects[1]->triangles[79] = *tree[79];
    objects[1]->triangles[80] = *tree[80];
    objects[1]->triangles[81] = *tree[81];
    objects[1]->triangles[82] = *tree[82];
    objects[1]->triangles[83] = *tree[83];
    objects[1]->triangles[84] = *tree[84];
    objects[1]->triangles[85] = *tree[85];
    objects[1]->triangles[86] = *tree[86];
    objects[1]->triangles[87] = *tree[87];
    objects[1]->triangles[88] = *tree[88];
    objects[1]->triangles[89] = *tree[89];
    objects[1]->triangles[90] = *tree[90];
    objects[1]->triangles[91] = *tree[91];
    objects[1]->triangles[92] = *tree[92];
    objects[1]->triangles[93] = *tree[93];
    objects[1]->triangles[94] = *tree[94];
    objects[1]->triangles[95] = *tree[95];
    objects[1]->triangles[96] = *tree[96];
    objects[1]->triangles[97] = *tree[97];
    objects[1]->triangles[98] = *tree[98];
    objects[1]->triangles[99] = *tree[99];
    objects[1]->triangles[100] = *tree[100];
    objects[1]->triangles[101] = *tree[101];
    objects[1]->triangles[102] = *tree[102];
    objects[1]->triangles[103] = *tree[103];
    objects[1]->triangles[104] = *tree[104];
    objects[1]->triangles[105] = *tree[105];
    objects[1]->triangles[106] = *tree[106];
    objects[1]->triangles[107] = *tree[107];
    objects[1]->triangles[108] = *tree[108];
    objects[1]->triangles[109] = *tree[109];
    objects[1]->triangles[110] = *tree[110];
    objects[1]->triangles[111] = *tree[111];
    objects[1]->triangles[112] = *tree[112];
    objects[1]->triangles[113] = *tree[113];
    objects[1]->triangles[114] = *tree[114];
    objects[1]->triangles[115] = *tree[115];
    objects[1]->triangles[116] = *tree[116];
    objects[1]->triangles[117] = *tree[117];
    objects[1]->triangles[118] = *tree[118];
    objects[1]->triangles[119] = *tree[119];
    objects[1]->triangles[120] = *tree[120];
    objects[1]->triangles[121] = *tree[121];
    objects[1]->triangles[122] = *tree[122];
    objects[1]->triangles[123] = *tree[123];
    objects[1]->triangles[124] = *tree[124];
    objects[1]->triangles[125] = *tree[125];
    objects[1]->triangles[126] = *tree[126];
    objects[1]->triangles[127] = *tree[127];
    objects[1]->triangles[128] = *tree[128];
    objects[1]->triangles[129] = *tree[129];
    objects[1]->triangles[130] = *tree[130];
    objects[1]->triangles[131] = *tree[131];
    objects[1]->triangles[132] = *tree[132];
    objects[1]->triangles[133] = *tree[133];
    objects[1]->triangles[134] = *tree[134];
    objects[1]->triangles[135] = *tree[135];
    objects[1]->triangles[136] = *tree[136];
    objects[1]->triangles[137] = *tree[137];
    objects[1]->triangles[138] = *tree[138];
    objects[1]->triangles[139] = *tree[139];
    objects[1]->triangles[140] = *tree[140];
    objects[1]->triangles[141] = *tree[141];
    objects[1]->triangles[142] = *tree[142];
    objects[1]->triangles[143] = *tree[143];
    objects[1]->triangles[144] = *tree[144];
    objects[1]->triangles[145] = *tree[145];
    objects[1]->triangles[146] = *tree[146];
    objects[1]->triangles[147] = *tree[147];
    objects[1]->triangles[148] = *tree[148];
    objects[1]->triangles[149] = *tree[149];
    objects[1]->triangles[150] = *tree[150];
    objects[1]->triangles[151] = *tree[151];
    objects[1]->triangles[152] = *tree[152];
    objects[1]->triangles[153] = *tree[153];
    objects[1]->triangles[154] = *tree[154];
    objects[1]->triangles[155] = *tree[155];
    objects[1]->triangles[156] = *tree[156];
    objects[1]->triangles[157] = *tree[157];
    objects[1]->triangles[158] = *tree[158];
    objects[1]->triangles[159] = *tree[159];
    objects[1]->triangles[160] = *tree[160];
    objects[1]->triangles[161] = *tree[161];
    objects[1]->triangles[162] = *tree[162];
    objects[1]->triangles[163] = *tree[163];
    objects[1]->triangles[164] = *tree[164];
    objects[1]->triangles[165] = *tree[165];
    objects[1]->triangles[166] = *tree[166];
    objects[1]->triangles[167] = *tree[167];
    objects[1]->triangles[168] = *tree[168];
    objects[1]->triangles[169] = *tree[169];
    objects[1]->triangles[170] = *tree[170];
    objects[1]->triangles[171] = *tree[171];
    objects[1]->triangles[172] = *tree[172];
    objects[1]->triangles[173] = *tree[173];
    objects[1]->triangles[174] = *tree[174];
    objects[1]->triangles[175] = *tree[175];
    objects[1]->triangles[176] = *tree[176];
    objects[1]->triangles[177] = *tree[177];
    objects[1]->triangles[178] = *tree[178];
    objects[1]->triangles[179] = *tree[179];
    objects[1]->triangles[180] = *tree[180];
    objects[1]->triangles[181] = *tree[181];
    objects[1]->triangles[182] = *tree[182];
    objects[1]->triangles[183] = *tree[183];
    objects[1]->triangles[184] = *tree[184];
    objects[1]->triangles[185] = *tree[185];
    objects[1]->triangles[186] = *tree[186];
    objects[1]->triangles[187] = *tree[187];
    objects[1]->triangles[188] = *tree[188];
    objects[1]->triangles[189] = *tree[189];
    objects[1]->triangles[190] = *tree[190];
    objects[1]->triangles[191] = *tree[191];
    objects[1]->triangles[192] = *tree[192];
    objects[1]->triangles[193] = *tree[193];
    objects[1]->triangles[194] = *tree[194];
    objects[1]->triangles[195] = *tree[195];
    objects[1]->triangles[196] = *tree[196];
    objects[1]->triangles[197] = *tree[197];
    objects[1]->triangles[198] = *tree[198];
    objects[1]->triangles[199] = *tree[199];
    objects[1]->triangles[200] = *tree[200];
    objects[1]->triangles[201] = *tree[201];
    objects[1]->triangles[202] = *tree[202];
    objects[1]->triangles[203] = *tree[203];
    objects[1]->triangles[204] = *tree[204];
    objects[1]->triangles[205] = *tree[205];
    objects[1]->triangles[206] = *tree[206];
    objects[1]->triangles[207] = *tree[207];
    objects[1]->triangles[208] = *tree[208];
    objects[1]->triangles[209] = *tree[209];
    objects[1]->triangles[210] = *tree[210];
    objects[1]->triangles[211] = *tree[211];
    objects[1]->triangles[212] = *tree[212];
    objects[1]->triangles[213] = *tree[213];
    objects[1]->triangles[214] = *tree[214];
    objects[1]->triangles[215] = *tree[215];
    objects[1]->triangles[216] = *tree[216];
    objects[1]->triangles[217] = *tree[217];
    objects[1]->triangles[218] = *tree[218];
    objects[1]->triangles[219] = *tree[219];
    objects[1]->triangles[220] = *tree[220];
    objects[1]->triangles[221] = *tree[221];
    objects[1]->triangles[222] = *tree[222];
    objects[1]->triangles[223] = *tree[223];
    objects[1]->triangles[224] = *tree[224];
    objects[1]->triangles[225] = *tree[225];
    objects[1]->triangles[226] = *tree[226];
    objects[1]->triangles[227] = *tree[227];
    objects[1]->triangles[228] = *tree[228];
    objects[1]->triangles[229] = *tree[229];
    objects[1]->triangles[230] = *tree[230];
    objects[1]->triangles[231] = *tree[231];
    objects[1]->triangles[232] = *tree[232];
    objects[1]->triangles[233] = *tree[233];
    objects[1]->triangles[234] = *tree[234];
    objects[1]->triangles[235] = *tree[235];
    objects[1]->triangles[236] = *tree[236];
    objects[1]->triangles[237] = *tree[237];
    objects[1]->triangles[238] = *tree[238];
    objects[1]->triangles[239] = *tree[239];
    objects[1]->triangles[240] = *tree[240];
    objects[1]->triangles[241] = *tree[241];
    objects[1]->triangles[242] = *tree[242];
    objects[1]->triangles[243] = *tree[243];
    objects[1]->triangles[244] = *tree[244];
    objects[1]->triangles[245] = *tree[245];
    objects[1]->triangles[246] = *tree[246];
    objects[1]->triangles[247] = *tree[247];
    objects[1]->triangles[248] = *tree[248];
    objects[1]->triangles[249] = *tree[249];
    objects[1]->triangles[250] = *tree[250];
    objects[1]->triangles[251] = *tree[251];
    objects[1]->triangles[252] = *tree[252];
    objects[1]->triangles[253] = *tree[253];
    objects[1]->triangles[254] = *tree[254];
    objects[1]->triangles[255] = *tree[255];
    objects[1]->triangles[256] = *tree[256];
    objects[1]->triangles[257] = *tree[257];
    objects[1]->triangles[258] = *tree[258];
    objects[1]->triangles[259] = *tree[259];
    objects[1]->triangles[260] = *tree[260];
    objects[1]->triangles[261] = *tree[261];
    objects[1]->triangles[262] = *tree[262];
    objects[1]->triangles[263] = *tree[263];

    objects[1]->scale(objects[1]->scaleVal, objects[1]->scaleVal * 0.13f, objects[1]->pos);
    objects[1]->movePos({20.0f,15.0f,200.0f});

    objects[2] = new Object(*objects[1]);
    objects[2]->movePos({ 0.1f,20.0f,250.0f });

    objects[3] = new Object(*objects[1]);
    objects[3]->movePos({ -120.0f,60.0f,150.0f });

    objects[4] = new Object(*objects[1]);
    objects[4]->movePos({ -155.0f,79.0f,90.0f });

    Triangle testTriangle1(500.0f, 100.0f, 250.0f, -200.0f, 0.0f, 2.0f, 50.0f, -100.0f, 50.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    Triangle testTriangle2(50.0f, 0.0f, 250.0f, -200.0f, 0.0f, 2.0f, 50.0f, 0.0f, 500.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    Triangle testTriangle3(50.0f, -100.0f, 225.0f, -200.0f, 10.0f, -2.0f, 50.0f, 20.0f, -50.0f, 1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    

    Triangle triangleEmemyTri(1.0f, 0.0f, 8.0f, 16.0f, 0.0f, -13.0f, -14.0f, 0.0f, -13.0f, -1.0f, static_cast<float>(textures[7].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[7].height), static_cast<float>(textures[7].width), static_cast<float>(textures[7].height), textures[7]);
    Triangle triangleEmemyTri1(1.0f, 0.0f, 8.0f, -14.0f, 0.0f, -13.0f, 2.0f, 30.0f, -4.0f, -1.0f, static_cast<float>(textures[6].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[6].height), static_cast<float>(textures[6].width), static_cast<float>(textures[6].height), textures[6]);
    Triangle triangleEmemyTri2(2.0f, 30.0f, -4.0f, -14.0f, 0.0f, -13.0f, 16.0f, 0.0f, -13.0f, -1.0f, static_cast<float>(textures[3].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[3].height), static_cast<float>(textures[3].width), static_cast<float>(textures[3].height), textures[3]);
    Triangle triangleEmemyTri3(16.0f, 0.0f, -13.0f, 1.0f, 0.0f, 8.0f, 2.0f, 30.0f, -4.0f, -1.0f, static_cast<float>(textures[5].width) / 2.0f, 0.0f, 0.0f, static_cast<float>(textures[5].height), static_cast<float>(textures[5].width), static_cast<float>(textures[5].height), textures[5]);
    
    triangleEnemy->triangles[0] = triangleEmemyTri;
    triangleEnemy->triangles[1] = triangleEmemyTri1;
    triangleEnemy->triangles[2] = triangleEmemyTri2;
    triangleEnemy->triangles[3] = triangleEmemyTri3;
    
    /*
    Object* copiedTriangleEnemy = new Object(*triangleEnemy);
    Object* copiedTriangleEnemy2 = new Object(*triangleEnemy);
    Object* copiedTriangleEnemy3 = new Object(*triangleEnemy);
    */

    enemies = new Enemy * [maxEnemies];

    srand(10423);

    for (int i = 0; i < 1; i++) {
        Object* copiedTriangleEnemy = new Object(*triangleEnemy);
        newEnemy(new Enemy(rand()%1000, rand() % 1000, rand() % 1000, 0.0f, 0.0f, 0.0f, 20.0f, 10.0f, 20.0f, 1.0f, 60.0f, *copiedTriangleEnemy, 500.0f, 500.0f));
    }

    //newEnemy(new Enemy(0.0f, 1000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f, 30.0f, 20.0f, 1.0f, 60.0f, *copiedTriangleEnemy, 500.0f, 500.0f));
    //newEnemy(new Enemy(100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 20.0f, 30.0f, 10.0f, 3.0f, 60.0f, *copiedTriangleEnemy2, 400.0f, 400.0f));
    //newEnemy(new Enemy(-50.0f, 10.0f, 100.0f, 0.0f, 0.0f, 0.0f, 20.0f, 30.0f, 20.0f, 1.0f, 60.0f, *copiedTriangleEnemy3, 500.0f, 500.0f));

    //bits setting???

    std::cout << "work pls" << std::endl;
    
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            size_t pixel_start = (x + (y * screenWidth)) * 4;
            //Blue - Green - Red - Alpha
            bits[pixel_start] = 255;
            bits[pixel_start + 1] = 255;
            bits[pixel_start + 2] = 255;
            bits[pixel_start + 3] = 255;
        }
    }

    SetBitmapBits(hBitmap, size, bits);
    
    ReleaseDC(hWnd, hDC);

    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMEENGINE));

    MSG msg;

    DWORD lastTime = GetTickCount();
    
    // Main message loop:
    SetTimer(hWnd, 999, 15, NULL);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyWindow(hWnd);

    ReleaseDC(hWnd, hDC);

    free(bits);

    for (int i = 0; i < numObjects; i++) {
        delete objects[i];
    }
    delete[] objects;

    for (int i = 0; i < 37; i++) {
        delete font1[i];
    }
    delete[] font1;

    for (int i = 0; i < numWeapons; i++) {
        delete weaponArr[i];
    }
    delete[] weaponArr;

    for (int i = 0; i < maxEnemies; i++) {
        delete enemies[i];
    }
    delete[] enemies;

    for (int i = 0; i < numButtons; i++) {
        delete buttons[i];
    }
    delete[] buttons;

    for (int i = 0; i < 694; i++) {
        delete gameMap1[i];
    }
    delete[] gameMap1;

    UnregisterClass(szWindowClass, hInstance);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEENGINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            if (currentMode == 0) {

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                //Gets the coords of the mouse on screen
                POINT screenCursorPos;
                GetCursorPos(&screenCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.left));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.right));

                POINT clientCursorPos;
                clientCursorPos.x = screenCursorPos.x - clientRect.left;
                clientCursorPos.y = screenCursorPos.y - clientRect.top;

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                bool mouseDown = GetKeyState(VK_LBUTTON) & 0x8000;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }

                //draw background
                drawBMP(bits, 0, 0, screenWidth, screenHeight, &mainMenuTexture, screenWidth, screenHeight);
                drawBMP(bits, floor(screenWidth/2.7), floor(screenHeight / 20), floor(screenWidth / 1.3), floor(screenHeight / 5), &gameLogoTexture, screenWidth, screenHeight);
                
                int newMode = currentMode;

                for (int i = 0; i < numButtons; i++) {
                    int result = buttons[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                    if (result != currentMode) {
                        newMode = result;
                    }
                }

                currentMode = newMode;

                /*
                    buttons[0] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.2), floor(screenWidth * 0.8), floor(screenHeight * 0.3), buttonHoverTexture, buttonTexture, false, 1);
                    buttons[1] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.35), floor(screenWidth * 0.8), floor(screenHeight * 0.45), buttonHoverTexture, buttonTexture, false, 0);
                    buttons[2] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.5), floor(screenWidth * 0.8), floor(screenHeight * 0.6), buttonHoverTexture, buttonTexture, false, 0);

                */

                if (currentMode == 1) {
                    ShowCursor(FALSE);
                    SetCursorPos(centreX, centreY);
                }

                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                //main menu

                if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) {
                    //MOUSE CLICKED
                    //currentMode = 1;
                }

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);

            } else if (currentMode == 1) {

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                POINT currentCursorPos;
                GetCursorPos(&currentCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                SetCursorPos(centreX, centreY);

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }

                // Mouse wheel movement
                int currentIndex = player.getWeaponIndex();

                if (wheelDeltaG >= 120) {
                    // Wheel moved upwards
                    if (currentIndex < numWeapons - 1) {
                        currentIndex++;
                    }
                    else {
                        currentIndex = 0;
                    }

                    player.setWeaponPtr(weaponArr[currentIndex]);
                    player.setWeaponIndex(currentIndex);
                }
                else if (wheelDeltaG <= -120) {
                    // Wheel moved downwards
                    if (currentIndex > 0) {
                        currentIndex--;
                    }
                    else {
                        currentIndex = numWeapons - 1;
                    }

                    player.setWeaponPtr(weaponArr[currentIndex]);
                    player.setWeaponIndex(currentIndex);
                }
                
                if (totalFrameCount % 200 == 0) {
                    Object* copiedTriangleEnemy = new Object(*triangleEnemy);
                    newEnemy(new Enemy(rand() % 100, rand() % 100, rand() % 100, 0.0f, 0.0f, 0.0f, 20.0f, 30.0f, 20.0f, 1.0f, 60.0f, *copiedTriangleEnemy, 500.0f, 500.0f));
               }
               
               
                //if (frameCount == 0) {
                
                //objects[0]->moveFacingX(objects[0]->pos, objects[0]->facingX, objects[0]->facingX + 0.005f);

                player.update(GetAsyncKeyState('W') & 0x8000, GetAsyncKeyState('A') & 0x8000, GetAsyncKeyState('S') & 0x8000, GetAsyncKeyState('D') & 0x8000, GetAsyncKeyState(VK_SPACE) & 0x8000, GetAsyncKeyState('Y') & 0x8000, GetAsyncKeyState('1') & 0x8000, GetAsyncKeyState('2') & 0x8000, GetKeyState(VK_LBUTTON) & 0x8000, GetAsyncKeyState('R') & 0x8000, objects, numObjects, currentCursorPos.x - centreX, currentCursorPos.y - centreY, enemies, currentEnemies);
                player.Draw(bits, frameCount, screenHeight, screenWidth, zBuffer);

                for (int i = 0; i < currentEnemies; i++) {
                    enemies[i]->update(objects, numObjects, player.getPos());
                    enemies[i]->draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos(), player.getFOV());
                }
                
                deleteEnemies();

                for (int i = 0; i < numObjects; i++) {
                    objects[i]->draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos(), player.getFOV());
                }
                
                // crosshair
                
                drawBMP(bits, floor(screenWidth * 0.5) - floor(screenWidth * 0.05), floor(screenHeight * 0.5) - floor(screenWidth * 0.05), floor(screenWidth * 0.5) + floor(screenWidth * 0.05), floor(screenHeight * 0.5) + floor(screenWidth * 0.05), &crosshairTexture, screenWidth, screenHeight);

                // HP bar
                
                drawBMP(bits, floor(screenWidth * 0.01), floor(screenHeight * 0.93), floor(screenWidth * 0.3), floor(screenHeight * 0.99), &HPbarTexture, screenWidth, screenHeight);
                
                float percentageDiff = player.getHP() / player.getMaxHP();
                drawBMP(bits, floor(screenWidth * 0.066), floor(screenHeight * 0.935), floor(screenWidth * 0.066 + screenWidth * 0.229 * percentageDiff ), floor(screenHeight * 0.985), &HPfullTexture, screenWidth, screenHeight);
                

                // icon holder
                drawBMP(bits, floor(screenWidth * 0.99 - screenHeight * 0.18), floor(screenHeight * 0.81), floor(screenWidth * 0.99), floor(screenHeight * 0.99), &iconHolderTexture, screenWidth, screenHeight);

                // the players current weapon
                player.drawWeaponBMP(bits, font1, screenWidth, screenHeight);

                if (player.getBulletIndex() >= player.getMaxBullets()) {
                    char reloadText[] = "RELOAD";
                    textDraw(reloadText, bits, floor(screenWidth * 0.4), 0, floor(screenWidth * 0.6), floor(screenHeight * 0.04), font1, screenWidth, screenHeight);
                }

                //drawRECT(bits, floor(screenWidth * 0.01), floor(screenHeight * 0.87), floor(screenWidth * 0.3), floor(screenHeight * 0.93), &HPbarTexture, screenWidth, screenHeight);

                //testTriangle.Draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos());

                //}
                
                // Update and repaint bits array every frame

                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    if (!escapeDown) {
                        currentMode = 2;
                        ShowCursor(TRUE);
                        escapeDown = true;
                    }
                    escapeDown = true;
                }
                else {
                    escapeDown = false;
                }

                if (GetAsyncKeyState('E') & 0x8000) {
                    if (!eDown) {
                        currentMode = 3;
                        ShowCursor(TRUE);
                        eDown = true;
                    }
                    eDown = true;
                }
                else {
                    eDown = false;
                }

                // Bullet counter
                char bulletText[] = "AMMO: ";
                char ammoChar[20];

                sprintf_s(ammoChar, sizeof(ammoChar), "%d", player.getMaxBullets() - player.getBulletIndex());

                char combinedBulletText[50];
                snprintf(combinedBulletText, sizeof(combinedBulletText), "%s%s", bulletText, ammoChar);

                textDraw(combinedBulletText, bits, floor(screenWidth * 0.88) - screenHeight * 0.18, floor(screenHeight * 0.93), floor(screenWidth * 0.99) - screenHeight * 0.18, floor(screenHeight * 0.99), font1, screenWidth, screenHeight);

                // FPS
                char myText[] = "FPS ";
                char fpsChar[20];

                sprintf_s(fpsChar, sizeof(fpsChar), "%d", fps);

                char combined[50];
                snprintf(combined, sizeof(combined), "%s%s", myText, fpsChar);

                textDraw(combined, bits, 0, 0, floor(screenWidth * 0.15), floor(screenHeight * 0.04), font1, screenWidth, screenHeight);


                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                cursorPos = currentCursorPos;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);

            } else if (currentMode == 2) {

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                POINT currentCursorPos;
                GetCursorPos(&currentCursorPos);

                //Gets the coords of the mouse on screen
                POINT screenCursorPos;
                GetCursorPos(&screenCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.left));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.right));

                POINT clientCursorPos;
                clientCursorPos.x = screenCursorPos.x - clientRect.left;
                clientCursorPos.y = screenCursorPos.y - clientRect.top;

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }
                
                //if (frameCount == 0) {
                player.Draw(bits, frameCount, screenHeight, screenWidth, zBuffer);

                for (int i = 0; i < currentEnemies; i++) {     
                    enemies[i]->draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos(), player.getFOV());
                }
                for (int i = 0; i < numObjects; i++) {
                    objects[i]->draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos(), player.getFOV());
                }

                //testTriangle.Draw(bits, frameCount, screenHeight, screenWidth, zBuffer, player.getFacingX(), player.getFacingY(), player.getPos());

                //}

                // Update and repaint bits array every frame

                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    if (!escapeDown) {
                        currentMode = 1;
                        ShowCursor(FALSE);
                        SetCursorPos(centreX, centreY);
                        escapeDown = true;
                    }
                    escapeDown = true;
                }
                else {
                    escapeDown = false;
                }
                //Make the background darker

                float darknessFactor = 0.3f; //Dark value in range [0,1], where 0 is completely black

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        // Blue - Green - Red - Alpha
                        bits[pixel_start] = (unsigned char)(bits[pixel_start] * darknessFactor);
                        bits[pixel_start + 1] = (unsigned char)(bits[pixel_start + 1] * darknessFactor);
                        bits[pixel_start + 2] = (unsigned char)(bits[pixel_start + 2] * darknessFactor);
                        bits[pixel_start + 3] = 255;
                    }
                }

                //PAUSE TEXT
                char pauseText[] = "PAUSED";
                textDraw(pauseText, bits, floor(screenWidth * 0.2), floor(screenHeight * 0.4), floor(screenWidth * 0.8), floor(screenHeight * 0.6), font1, screenWidth, screenHeight);

                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                cursorPos = currentCursorPos;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);
            } else if (currentMode == 3) {

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                //Gets the coords of the mouse on screen
                POINT screenCursorPos;
                GetCursorPos(&screenCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.left));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.right));

                POINT clientCursorPos;
                clientCursorPos.x = screenCursorPos.x - clientRect.left;
                clientCursorPos.y = screenCursorPos.y - clientRect.top;

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                bool mouseDown = GetKeyState(VK_LBUTTON) & 0x8000;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }

                //draw background
                drawBMP(bits, 0, 0, screenWidth, screenHeight, &inventoryTexture, screenWidth, screenHeight);

                int newMode = currentMode;

                for (int i = 0; i < numButtons; i++) {
                    int result = buttons[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                    if (result != currentMode) {
                        newMode = result;
                    }
                }

                //char myText[] = "BACK";
                //textDraw(myText, bits, floor(screenWidth * 0.01), floor(screenHeight * 0.01), floor(screenWidth * 0.19), floor(screenHeight * 0.09), font1, screenWidth, screenHeight);

               // char myText2[] = "GUNS";
                //textDraw(myText2, bits, floor(screenWidth * 0.21), floor(screenHeight * 0.01), floor(screenWidth * 0.39), floor(screenHeight * 0.09), font1, screenWidth, screenHeight);

               // char myText3[] = "SHOP";
                //textDraw(myText3, bits, floor(screenWidth * 0.81), floor(screenHeight * 0.01), floor(screenWidth * 0.99), floor(screenHeight * 0.09), font1, screenWidth, screenHeight);
                currentMode = newMode;

                /*
                    buttons[0] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.2), floor(screenWidth * 0.8), floor(screenHeight * 0.3), buttonHoverTexture, buttonTexture, false, 1);
                    buttons[1] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.35), floor(screenWidth * 0.8), floor(screenHeight * 0.45), buttonHoverTexture, buttonTexture, false, 0);
                    buttons[2] = new Button(floor(screenWidth * 0.35), floor(screenHeight * 0.5), floor(screenWidth * 0.8), floor(screenHeight * 0.6), buttonHoverTexture, buttonTexture, false, 0);

                */

                if (currentMode == 1) {
                    ShowCursor(FALSE);
                    SetCursorPos(centreX, centreY);
                }

                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                //main menu

                if (GetAsyncKeyState('E') & 0x8000) {
                    if (!eDown) {
                        currentMode = 1;
                        ShowCursor(FALSE);
                        SetCursorPos(centreX, centreY);
                        eDown = true;
                    }
                    eDown = true;
                }
                else {
                    eDown = false;
                }

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);
            }
            else if (currentMode == 4) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                //Gets the coords of the mouse on screen
                POINT screenCursorPos;
                GetCursorPos(&screenCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.left));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.right));

                POINT clientCursorPos;
                clientCursorPos.x = screenCursorPos.x - clientRect.left;
                clientCursorPos.y = screenCursorPos.y - clientRect.top;

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                bool mouseDown = GetKeyState(VK_LBUTTON) & 0x8000;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }
                
                //draw background
                drawBMP(bits, 0, 0, screenWidth, screenHeight, &inventoryTexture, screenWidth, screenHeight);

                int newMode = currentMode;
                
                for (int i = 0; i < numButtons; i++) {
                    int result = buttons[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                    if (result != currentMode) {
                        newMode = result;
                    }
                }

                for (int i = 0; i < numSliders; i++) {
                    sliders[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                }

                currentMode = newMode;

                if (currentMode == 1) {
                    ShowCursor(FALSE);
                    SetCursorPos(centreX, centreY);
                }

                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);
            } 
            else if (currentMode == 5) {
                PostQuitMessage(0);
            }
            else if (currentMode == 6) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                //Gets the coords of the mouse on screen
                POINT screenCursorPos;
                GetCursorPos(&screenCursorPos);

                int centreX = (screenWidth / 2);
                int centreY = (screenHeight / 2);

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.left));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&clientRect.right));

                POINT clientCursorPos;
                clientCursorPos.x = screenCursorPos.x - clientRect.left;
                clientCursorPos.y = screenCursorPos.y - clientRect.top;

                // Allocate memory for the zBuffer if it's not allocated or if the size has changed

                if (zBuffer == nullptr || screenWidth * screenHeight != currentBufferSize) {
                    if (zBuffer != nullptr) {
                        delete[] zBuffer; // Release the previous buffer
                    }
                    zBuffer = new float[screenWidth * screenHeight];
                    currentBufferSize = screenWidth * screenHeight; // Remember the current size
                }

                for (int i = 0; i < currentBufferSize; i++) {
                    zBuffer[i] = 10000000000000000.0f; //just huge number (when its that far away it probably wont be seen)
                }

                for (int y = 0; y < screenHeight; y++) {
                    for (int x = 0; x < screenWidth; x++) {
                        size_t pixel_start = (x + (y * screenWidth)) * 4;
                        //Blue - Green - Red - Alpha
                        bits[pixel_start] = 255;
                        bits[pixel_start + 1] = 255;
                        bits[pixel_start + 2] = 255;
                        bits[pixel_start + 3] = 255;
                    }
                }

                DWORD currentTime = GetTickCount64();
                DWORD elapsedTime = currentTime - lastTime;

                bool mouseDown = GetKeyState(VK_LBUTTON) & 0x8000;

                if (elapsedTime >= 1000) {
                    // One second has passed
                    fps = (frameCount * 1000) / elapsedTime;

                    // Output the FPS to the console
                    std::cout << "FPS: " << fps << std::endl;

                    // Reset frame count and time
                    frameCount = 0;
                    lastTime = currentTime;
                }

                //draw background
                drawBMP(bits, 0, 0, screenWidth, screenHeight, &inventoryTexture, screenWidth, screenHeight);

                int newMode = currentMode;

                for (int i = 0; i < numButtons; i++) {
                    int result = buttons[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                    if (result != currentMode) {
                        newMode = result;
                    }
                }

                for (int i = 0; i < numSliders; i++) {
                    sliders[i]->draw(bits, screenWidth, screenHeight, clientCursorPos.x, clientCursorPos.y, mouseDown, currentMode, font1);
                }

                currentMode = newMode;

                if (currentMode == 1) {
                    ShowCursor(FALSE);
                    SetCursorPos(centreX, centreY);
                }

                SetBitmapBits(hBitmap, size, bits);

                // Increment the frame count
                frameCount++;
                totalFrameCount++;

                BITMAP bm;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

                GetObject(hBitmap, sizeof(bm), &bm);
                BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                EndPaint(hWnd, &ps);
                }
            // Reset global mouse wheel variable
            wheelDeltaG = 0;
        } 
        break;
    case WM_TIMER:
        {
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }
    case WM_MOUSEWHEEL:
        {
        // Mouse movement
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        // Store in global variable
        wheelDeltaG = wheelDelta;

        return 0;
        }
    case WM_ERASEBKGND:
        {
            return 1;
        }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}