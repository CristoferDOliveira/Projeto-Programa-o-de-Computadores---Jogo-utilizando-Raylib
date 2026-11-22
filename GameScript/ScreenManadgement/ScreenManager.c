#include "scrmgr.h"
#include "menuscr.h"
#include "playscr.h"
#include "crtscr.h"

static GameScreen currentScreen = SCREEN_MENU;

void InitScreens(int screenWidth, int screenHeight) {
    InitMenuScreen(screenWidth);
    InitPlayScreen(screenWidth, screenHeight);
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
        case SCREEN_PLAY:
            currentScreen = UpdatePlayScreen();
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
        case SCREEN_PLAY:
            DrawPlayScreen();
            break;
        case SCREEN_CREATORS:
            DrawCreatorsScreen();
            break;
    }
}
