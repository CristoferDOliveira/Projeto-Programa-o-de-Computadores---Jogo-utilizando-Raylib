#include "scrmgr.h"
#include "menuscr.h" 
#include "crtscr.h"  

// Guarda qual tela está sendo exibida no momento
static GameScreen currentScreen = SCREEN_MENU;

void InitScreens(int screenWidth) {
    InitMenuScreen(screenWidth);
    InitCreatorsScreen(screenWidth);
}

void ChangeScreen(GameScreen newScreen) {
    currentScreen = newScreen;
}

void UpdateCurrentScreen(void) {
    switch (currentScreen) {
        case SCREEN_MENU:
            currentScreen = UpdateMenuScreen();
            break;
        case SCREEN_CREATORS:
            currentScreen = UpdateCreatorsScreen();
            break;
    }
}

void DrawCurrentScreen(void) {
    switch (currentScreen) {
        case SCREEN_MENU:
            DrawMenuScreen();
            break;
        case SCREEN_CREATORS:
            DrawCreatorsScreen();
            break;
    }
}
